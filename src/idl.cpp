#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QHash>
#include <QDebug>

#include "idl.h"


QList<IDL::Entry> IDL::load(const QDir &dir, const QString &filename)
{
  if (!dir.isReadable())
    throw QObject::tr("Could not read directory %1").arg(dir.absolutePath());

  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly))
    throw QObject::tr("Could not open file %1").arg(filename);

  // Find all images
  QHash<QString, Entry> entries;
  foreach (const QString &s, dir.entryList(QStringList() << "*.png"))
  {
    QString name = s;
    name.chop(4);
    entries[name] = Entry(name, dir.absoluteFilePath(s), QList<QRect>());
  }

  QTextStream fin(&f);
  while (fin.skipWhiteSpace(), !fin.atEnd())
  {
    QString item;
    int y0, x0, y1, x1;
    fin >> item >> y0 >> x0 >> y1 >> x1;

    if (fin.status() != QTextStream::Ok)
      throw QObject::tr("Error reading file %1").arg(filename);

    if (entries.count(item))
      entries[item].rects << QRect(x0, y0, x1-x0+1, y1-y0+1);
    else
      qWarning() << QObject::tr("IDL: warning: file %1.png not found").arg(item);
  }
  
  return entries.values();
}
