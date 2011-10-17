#ifndef IDL_H
#define IDL_H

#include <QString>
#include <QRect>
#include <QList>

namespace IDL
{
  struct Entry
  {
    Entry(const QString &_name, const QString &_filename, const QList<QRect> &_rects)
      : name(_name), filename(_filename), rects(_rects) {}

    QString name;
    QString filename;
    QList<QRect> rects;
  };

  QList<Entry> load(const QString &filename);
}

#endif // IDL_H
