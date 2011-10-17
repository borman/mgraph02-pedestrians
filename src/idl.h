#ifndef IDL_H
#define IDL_H

#include <QString>
#include <QRect>
#include <QList>
#include <QDir>

namespace IDL
{
  struct Entry
  {
    Entry(const QString &_name = QString::null,
          const QString &_filename = QString::null,
          const QList<QRect> &_rects = QList<QRect>())
      : name(_name), filename(_filename), rects(_rects) {}

    bool operator<(const Entry &other) const { return name < other.name; }

    QString name;
    QString filename;
    QList<QRect> rects;
  };

  QList<Entry> load(const QDir &dir, const QString &filename);
}

#endif // IDL_H
