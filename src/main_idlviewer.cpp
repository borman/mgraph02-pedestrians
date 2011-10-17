#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QtConcurrentMap>
#include <QFuture>

#include "idl.h"
#include "gradient.h"
#include "median.h"
#include "colorcorrect.h"
#include "classify.h"
#include "dataset.h"

class DataLoader: public QObject
{
  Q_OBJECT
public:
  DataLoader(DataSet::Model *mdl);
  void load(const QDir &dir, const QString &idlFile);

private slots:
  void onProgress(int progress);
  void onFinished();

private:
  QFutureWatcher<DataSet::Item> *m_watcher;
};

DataLoader::DataLoader(DataSet::Model *mdl)
  : QObject(mdl)
{
  m_watcher = new QFutureWatcher<DataSet::Item>(this);
  connect(m_watcher, SIGNAL(progressValueChanged(int)), SLOT(onProgress(int)));
  connect(m_watcher, SIGNAL(finished()), SLOT(onFinished()));
}


void DataLoader::onProgress(int)
{
  DataSet::Model *mdl = static_cast<DataSet::Model *>(parent());
  int oldSize = mdl->rowCount();
  int newSize = m_watcher->future().resultCount();
  for (int i=oldSize; i<newSize; i++)
    mdl->append(m_watcher->future().resultAt(i));
}

void DataLoader::onFinished()
{
  DataSet::Model *mdl = static_cast<DataSet::Model *>(parent());
  double precision, recall;
  DataSet::evaluate(mdl->items(), precision, recall);
  qDebug() << "Precision:" << precision;
  qDebug() << "Recall:" << recall;
}


Model *classifier = 0;

static DataSet::Item loadImage(const IDL::Entry &e)
{
  DataSet::Item item;

  item.name = e.name;
  item.baseImage.load(e.filename);

  //median(item.baseImage, item.baseImage.rect(), 3);

  item.gradient = showGradient(gradient(item.baseImage, item.baseImage.rect()));
  rgb_stretch(item.gradient, item.gradient.rect());

  item.probMap = classifier->probMap(item.baseImage);
  item.peds = e.rects;

  item.hits = classifier->detect(item.baseImage);

  return item;
}

void DataLoader::load(const QDir &dir, const QString &idlFile)
{
  DataSet::Model *mdl = static_cast<DataSet::Model *>(parent());
  mdl->clear();
  QList<IDL::Entry> entries = IDL::load(dir, idlFile);
  qSort(entries);
  m_watcher->setFuture(QtConcurrent::mapped(entries, loadImage));
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  if (argc < 2)
  {
    qWarning() << "No arguments supplied";
    return 1;
  }

  try
  {
    classifier = Model::load("model.txt");
    Q_ASSERT(classifier != 0);

    DataSet::Model mdl;

    DataLoader loader(&mdl);
    loader.load(QFileInfo(argv[1]).dir(), argv[1]);

    QListView view;
    view.setModel(&mdl);
    view.setItemDelegate(new DataSet::ItemDelegate(&mdl, &view));
    view.setUniformItemSizes(true);
    view.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view.show();

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << "Fatal exception: " << e;

    return 1;
  }
}

#include "main_idlviewer.moc"
