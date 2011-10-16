#include <QCoreApplication>
#include <QDir>
#include <QDebug>
#include <QList>

#include <QtConcurrentMap>
#include <QFuture>
#include <QFutureWatcher>

#include "idl.h"
#include "gradient.h"
#include "colorcorrect.h"
#include "hog.h"

class DescriptorConsumer: public QObject
{
  Q_OBJECT
public:
  typedef QPair<QList<QVector<double> >, QList<QVector<double> > > Result; // I <3 C++

  DescriptorConsumer(const QFuture<Result> &_fut)
    : fut(_fut)
  {
    QFutureWatcher<Result> *w = new QFutureWatcher<Result>(this);
    w->setFuture(fut);
    connect(w, SIGNAL(progressValueChanged(int)), SLOT(onProgress(int)));
    connect(w, SIGNAL(finished()), SLOT(onCompleted()));
  }
public slots:
  void onProgress(int p)
  {
    qWarning() << "Descriptors: " << fut.progressValue() << "of" << fut.progressMaximum();
  }
  void onCompleted()
  {
    qWarning() << "Descriptors done";
    QCoreApplication::exit();
  }
private:
  QFuture<Result> fut;
};

QDir srcDir;
DescriptorConsumer::Result makeDescriptor(const IDL::Entry &e)
{
  QImage img(srcDir.filePath(e.name + ".png"));

  QList<QVector<double> > bgSamples;
  QRect bgRect(0, 0, 80, 200);
  while (bgRect.right() < img.width())
  {
    bool goodRect = true;
    foreach (const QRect &r, e.rects)
      if (bgRect.intersects(r))
      {
        goodRect = false;
        break;
      }
    if (goodRect)
      bgSamples << HOG(gradient(img, bgRect)).serialize();
    bgRect.translate(60, 0);
  }

  QList<QVector<double> > pedSamples;
  foreach (const QRect &r, e.rects)
    pedSamples << HOG(gradient(img, r.adjusted(0, 0, -1, -1))).serialize();

  return qMakePair(bgSamples, pedSamples);
}

int main(int argc, char **argv)
{
  QCoreApplication app(argc, argv);

  if (argc < 2)
  {
    qWarning() << "No arguments supplied";
    return 1;
  }

  try
  {
    srcDir = QFileInfo(argv[1]).dir();
    QList<IDL::Entry> objects = IDL::load(argv[1]);

    DescriptorConsumer consumer(QtConcurrent::mapped(objects, makeDescriptor));

    return app.exec();
  }
  catch (const QString &e)
  {
    qWarning() << "Fatal exception: " << e;

    return 1;
  }
}

#include "main_train.moc"
