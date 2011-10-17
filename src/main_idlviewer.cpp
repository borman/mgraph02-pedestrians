#include <QApplication>
#include <QDir>
#include <QDebug>
#include <QtConcurrentMap>
#include <QFuture>
#include <QFutureWatcher>
#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>

#include "idl.h"
#include "gradient.h"
#include "colorcorrect.h"
#include "classify.h"

class DataSetModel: public QAbstractListModel
{
  Q_OBJECT
public:
  struct Item
  {
    QString name;
    QImage baseImage;
    QImage gradient;
    QImage probMap;
    QList<QRect> peds;
    QList<QRect> hits;
  };

  DataSetModel(QObject *parent = 0) : QAbstractListModel(parent) {}

  QVariant data(const QModelIndex &index, int role) const { return QVariant(); }
  int rowCount(const QModelIndex &parent = QModelIndex()) const { return m_items.size(); }

  const Item &itemAt(int i) const { return m_items[i]; }

  void clear()
  {
    beginResetModel();
    m_items.clear();
    endResetModel();
  }

  void append(const Item &item)
  {
    beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
    m_items << item;
    endInsertRows();
  }

private:
  QList<Item> m_items;
};



class DataSetModelPopulator: public QObject
{
  Q_OBJECT
public:
  DataSetModelPopulator(DataSetModel *mdl)
    : QObject(mdl)
  {
    m_watcher = new QFutureWatcher<DataSetModel::Item>(this);
    connect(m_watcher, SIGNAL(progressValueChanged(int)), SLOT(onProgress(int)));
  }

  void setFuture(const QFuture<DataSetModel::Item> &fut)
  {
    DataSetModel *mdl = static_cast<DataSetModel *>(parent());
    mdl->clear();
    m_watcher->setFuture(fut);
  }

private slots:
  void onProgress(int progress)
  {
    DataSetModel *mdl = static_cast<DataSetModel *>(parent());
    int oldSize = mdl->rowCount();
    int newSize = m_watcher->future().resultCount();
    for (int i=oldSize; i<newSize; i++)
      mdl->append(m_watcher->future().resultAt(i));
  }

private:
  QFutureWatcher<DataSetModel::Item> *m_watcher;
};



class DataSetItemDelegate: public QStyledItemDelegate
{
public:
  DataSetItemDelegate(DataSetModel *mdl, QObject *parent = 0): QStyledItemDelegate(parent), m_mdl(mdl) {}

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QStyledItemDelegate::paint(painter, option, index);

    const DataSetModel::Item &item = m_mdl->itemAt(index.row());
    QPoint base = option.rect.topLeft() + QPoint(10, 10);

    painter->save();

    painter->drawImage(base, item.baseImage);

    painter->setOpacity(0.5);
    painter->drawImage(base, item.gradient);

    painter->setOpacity(0.5);
    painter->drawImage(base, item.probMap);

    painter->setOpacity(1);
    painter->setPen(QPen(Qt::green, 2));
    foreach (const QRect &r, item.peds)
      painter->drawRect(r.translated(base).adjusted(0, -5, 0, 5));

    painter->setPen(QPen(Qt::blue, 2));
    foreach (const QRect &r, item.hits)
      painter->drawRect(r.translated(base).adjusted(0, -3, 0, 3));

    painter->setPen(Qt::white);
    painter->setFont(QFont("Sans", 16, QFont::Bold));
    painter->drawText(option.rect.adjusted(10, 10, -10, -10),
                      Qt::AlignLeft | Qt::AlignTop,
                      item.name);

    painter->restore();
  }

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
  {
    QSize sh = QStyledItemDelegate::sizeHint(option, index);
    return QSize(sh.width(), 220);
  }
private:
  DataSetModel *m_mdl;
};



Model *classifier = 0;

DataSetModel::Item loadImage(const IDL::Entry &e)
{
  DataSetModel::Item item;

  item.name = e.name;
  item.baseImage.load(e.filename);

  QRect rect(0, 0, item.baseImage.width(), item.baseImage.height());
  item.gradient = showGradient(gradient(item.baseImage, rect));
  rgb_stretch(item.gradient, rect);

  item.probMap = classifier->probMap(item.baseImage);
  item.peds = e.rects;

  item.hits = classifier->detect(item.baseImage);

  return item;
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
    QDir srcDir = QFileInfo(argv[1]).dir();
    QList<IDL::Entry> entries = IDL::load(srcDir, argv[1]);
    qSort(entries);

    classifier = Model::load("model.txt");
    Q_ASSERT(classifier != 0);

    DataSetModel mdl;

    QListView view;
    view.setModel(&mdl);
    view.setItemDelegate(new DataSetItemDelegate(&mdl, &view));
    view.setUniformItemSizes(true);
    view.setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    view.show();

    DataSetModelPopulator loader(&mdl);
    loader.setFuture(QtConcurrent::mapped(entries, loadImage));

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << "Fatal exception: " << e;

    return 1;
  }
}

#include "main_idlviewer.moc"
