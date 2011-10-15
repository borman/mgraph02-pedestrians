#ifndef IDL_H
#define IDL_H

#include <QString>
#include <QRect>
#include <QList>

namespace IDL
{
  struct Entry
  {
    Entry(const QString &_n, const QRect &_r)
      : name(_n), rect(_r) {}

    QString name;
    QRect rect;
  };

  QList<Entry> load(const QString &filename);
}

#endif // IDL_H
