#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QPainter>

#include "gradient.h"
#include "hog.h"
#include "classify.h"

QImage probMap(const QImage &img, Model *mdl)
{
  QImage map(img.width(), img.height(), QImage::Format_RGB32);

  HOG hog(gradient(img, QRect(0, 0, img.width(), img.height())));
  hog.normalize();

  QVector<double> ps(hog.width()-9);
  double upper = 0.1;
  double lower = -0.1;
  for (int x=0; x<ps.size(); x++)
  {
    ps[x] = mdl->predict(hog.serialize(x, 0, 10, hog.height()));
    upper = qMax(upper, ps[x]);
    lower = qMin(lower, ps[x]);
  }

  QPainter p;
  p.begin(&map);
  for (int x=0; x<hog.width()-9; x++)
    p.fillRect(x*8, 0, 80, map.height(),
               QColor(qMin(0.0, ps[x])*255.0/lower,
                      qMax(0.0, ps[x])*255.0/upper,
                      0,
                      ps[x]>0? 100 : 40));
  p.end();

  return map;
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);

  try
  {
    Model *mdl = Model::load("model.txt");
    if (!mdl)
      throw QObject::tr("Could not load model");

    QImage img(argv[1]);
    if (img.isNull())
      throw QObject::tr("Could not load image");


    QLabel lbl(0, Qt::Dialog);
    lbl.setPixmap(QPixmap::fromImage(probMap(img, mdl)));
    lbl.show();

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << QObject::tr("Fatal exception: %1").arg(e);

    return 1;
  }
}
