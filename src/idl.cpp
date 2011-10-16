#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTextStream>
#include <QHash>

#include "idl.h"


QList<IDL::Entry> IDL::load(const QString &filename)
{
  typedef QHash<QString, QList<QRect> > EHash;

  QFile f(filename);
  if (!f.open(QIODevice::ReadOnly))
    throw QObject::tr("Could not open file %1").arg(filename);

  //QDir imageDir = QFileInfo(filename).dir();

  QTextStream fin(&f);
  EHash eHash;
  while (fin.skipWhiteSpace(), !fin.atEnd())
  {
    QString item;
    int y0, x0, y1, x1;
    fin >> item >> y0 >> x0 >> y1 >> x1;

    if (fin.status() != QTextStream::Ok)
      throw QObject::tr("Error reading file %1").arg(filename);

    eHash[item] << QRect(x0, y0, x1-x0+1, y1-y0+1);
    //QString itemFilename = imageDir.filePath(item + ".png");
    //res << Entry(item, QRect(x0, y0, x1-x0+1, y1-y0+1));
  }

  QList<Entry> res;
  for (EHash::iterator it = eHash.begin(); it!=eHash.end(); ++it)
    res << Entry(it.key(), it.value());
  
  return res;
}
