#include <iostream>

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
#include "classify.h"

using namespace std;

class DescriptorConsumer: public QObject
{
  Q_OBJECT
public:
  typedef QPair<QList<QVector<double> >, QList<QVector<double> > > Result; // I <3 C++

  DescriptorConsumer(const QFuture<Result> &_fut)
    : fut(_fut), prevReported(0)
  {
    QFutureWatcher<Result> *w = new QFutureWatcher<Result>(this);
    w->setFuture(fut);
    connect(w, SIGNAL(progressValueChanged(int)), SLOT(onProgress(int)));
    connect(w, SIGNAL(finished()), SLOT(onCompleted()));

    cout << "Calculating " << fut.progressMaximum() << " descriptors: ";
    cout.flush();
  }
public slots:
  void onProgress(int p)
  {
    if (p - prevReported < 10)
      return;
    cout << fut.progressValue() << "... ";
    cout.flush();
    prevReported = p;
  }
  void onCompleted()
  {
    cout << "done!" << endl;

    QList<QVector<double> > bgs, peds;
    foreach(const Result &r, fut)
    {
      bgs += r.first;
      peds += r.second;
    }
    cout << "Training model: "
         << bgs.size() << " background samples, "
         << peds.size() << " pedestrian samples" << endl;
    Model *mdl = Model::train(bgs, peds);
    mdl->save("model.txt");

    QCoreApplication::exit();
  }
private:
  QFuture<Result> fut;
  int prevReported;
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
