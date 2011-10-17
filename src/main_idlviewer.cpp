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

Model *classifier = 0;

class DataSetModel: public QAbstractListModel
{
  Q_OBJECT
public:
  struct Item
  {
    QImage baseImage;
    QImage gradient;
    QImage probMap;
    QList<QRect> peds;
  };

  DataSetModel(const QFuture<Item> &items)
    : m_fut(items)
  {
    QFutureWatcher<Item> *w = new QFutureWatcher<Item>(this);
    w->setFuture(m_fut);
    connect(w, SIGNAL(progressValueChanged(int)), SLOT(onProgress(int)));
  }

  QVariant data(const QModelIndex &index, int role) const { return QVariant(); }
  int rowCount(const QModelIndex &parent) const { return m_items.size(); }

  const Item &itemAt(int i) const { return m_items[i]; }

private slots:
  void onProgress(int progress)
  {
    int oldSize = m_items.size();
    int newSize = m_fut.resultCount();
    beginInsertRows(QModelIndex(), oldSize, newSize-1);
    for (int i=oldSize; i<newSize; i++)
      m_items << m_fut.resultAt(i);
    endInsertRows();
  }

private:
  QFuture<Item> m_fut;
  QList<Item> m_items;
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

    painter->setOpacity(0.7);
    painter->drawImage(base, item.gradient);

    painter->setOpacity(0.5);
    painter->drawImage(base, item.probMap);

    painter->setOpacity(1);
    painter->setPen(QPen(Qt::green, 2));
    foreach (const QRect &r, item.peds)
      painter->drawRect(r.translated(base).adjusted(0, -5, 0, 5));

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

DataSetModel::Item loadImage(const IDL::Entry &e)
{
  DataSetModel::Item item;

  item.baseImage.load(e.filename);

  QRect rect(0, 0, item.baseImage.width(), item.baseImage.height());
  item.gradient = showGradient(gradient(item.baseImage, rect));
  rgb_stretch(item.gradient, rect);

  item.probMap = classifier->probMap(item.baseImage);
  item.peds = e.rects;

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
    QList<IDL::Entry> entries = IDL::load(argv[1]);

    classifier = Model::load("model.txt");
    Q_ASSERT(classifier != 0);

    DataSetModel mdl(QtConcurrent::mapped(entries, loadImage));

    QListView view;
    view.setModel(&mdl);
    view.setItemDelegate(new DataSetItemDelegate(&mdl, &view));
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
