#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>

#include "idl.h"


QList<IDL::Entry> IDL::load(const QString &filename)
{
  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly))
    throw QObject::tr("Could not open file %1").arg(filename);

  QDir imageDir = QFileInfo(filename).dir();

  QTextStream fin(&f);
  QList<Entry> res;
  while (fin.skipWhiteSpace(), !fin.atEnd())
  {
    QString item;
    int y0, x0, y1, x1;
    fin >> item >> y0 >> x0 >> y1 >> x1;

    if (fin.status() != QTextStream::Ok)
      throw QObject::tr("Error reading file %1").arg(filename);

    QString itemFilename = imageDir.filePath(item + ".png");
    res << Entry(item, QRect(x0, y0, x1-x0+1, y1-y0+1));
  }
  
  return res;
}
