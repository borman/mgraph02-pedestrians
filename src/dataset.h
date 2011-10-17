#ifndef DATASET_H
#define DATASET_H

#include <QFutureWatcher>
#include <QAbstractListModel>
#include <QListView>
#include <QStyledItemDelegate>
#include <QPainter>

namespace DataSet {

struct Item
{
  QString name;
  QImage baseImage;
  QImage gradient;
  QImage probMap;
  QList<QRect> peds;
  QList<QRect> hits;
};

class Model: public QAbstractListModel
{
  Q_OBJECT
public:

  Model(QObject *parent = 0) : QAbstractListModel(parent) {}

  QVariant data(const QModelIndex &, int) const { return QVariant(); }
  int rowCount(const QModelIndex & = QModelIndex()) const { return m_items.size(); }

  const Item &itemAt(int i) const { return m_items[i]; }
  const QList<Item> &items() const { return m_items; }
  void clear();
  void append(const Item &item);

private:
  QList<Item> m_items;
};


class ItemDelegate: public QStyledItemDelegate
{
public:
  ItemDelegate(Model *mdl, QObject *parent = 0): QStyledItemDelegate(parent), m_mdl(mdl) {}

  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
  Model *m_mdl;
};

void evaluate(const QList<Item> &data, double &precision, double &recall);

} // namespace DataSet

#endif // DATASET_H
