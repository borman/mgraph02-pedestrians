#include "dataset.h"

using namespace DataSet;

void Model::clear()
{
  beginResetModel();
  m_items.clear();
  endResetModel();
}
void Model::append(const Item &item)
{
  beginInsertRows(QModelIndex(), m_items.size(), m_items.size());
  m_items << item;
  endInsertRows();
}



void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QStyledItemDelegate::paint(painter, option, index);

  const Item &item = m_mdl->itemAt(index.row());
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

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
  QSize sh = QStyledItemDelegate::sizeHint(option, index);
  return QSize(sh.width(), 220);
}


void DataSet::evaluate(const QList<Item> &data, double &precision, double &recall)
{
  int totalHits = 0;
  int correctHits = 0;

  int totalPeds = 0;
  int correctPeds = 0;

  foreach (const Item &item, data)
  {
    totalHits += item.hits.size();
    totalPeds += item.peds.size();

    foreach (const QRect &hit, item.hits)
      foreach (const QRect &ped, item.peds)
        if (hit.intersected(ped).width() > 40)
        {
          correctHits++;
          break;
        }

    foreach (const QRect &ped, item.peds)
      foreach (const QRect &hit, item.hits)
        if (hit.intersected(ped).width() > 40)
        {
          correctPeds++;
          break;
        }
  }

  precision = totalHits==0? 0 : double(correctHits) / totalHits;
  recall =    totalPeds==0? 0 : double(correctPeds) / totalPeds;
}
