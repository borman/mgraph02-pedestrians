#ifndef IDL_H
#define IDL_H

#include <QString>
#include <QRect>
#include <QList>

namespace IDL
{
  struct Entry
  {
    Entry(const QString &_n, const QList<QRect> &_rs)
      : name(_n), rects(_rs) {}

    QString name;
    QList<QRect> rects;
  };

  QList<Entry> load(const QString &filename);
}

#endif // IDL_H
