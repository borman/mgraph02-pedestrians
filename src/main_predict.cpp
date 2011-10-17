#include <QApplication>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QPainter>

#include "gradient.h"
#include "hog.h"
#include "classify.h"


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
    lbl.setPixmap(QPixmap::fromImage(mdl->probMap(img)));
    lbl.show();

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << QObject::tr("Fatal exception: %1").arg(e);

    return 1;
  }
}
