#include <cmath>
#include <QPainter>
#include "histogram.h"

QPixmap drawHistogram(const QImage &img, const QRect &rect,
                      ColorProp prop, int w, int h,
                      const QColor &bg, const QColor &fg)
{
  int qmin, qmax;
  QVector<double> stats = makeHistogram(img, rect, prop, w, &qmin, &qmax);

  double smax = stats[0];
  for (int i=1; i<w; i++)
    smax = qMax(smax, stats[i]);

  QPixmap res(w, h);
  QPainter p;
  p.begin(&res);

  QColor highlight(fg.red(), fg.green(), fg.blue(), fg.alpha()*0.4);

  p.fillRect(res.rect(), bg);
  p.fillRect(qmin, 0, qmax-qmin+1, h, highlight);

  p.setPen(fg);
  for (int i=0; i<w; i++)
  {
    double val = stats[i]/smax;
    p.drawLine(i, h-1, i, h-1 - val*(h-2));
  }

  p.end();

  return res;
}

QVector<double> makeHistogram(const QImage &img, const QRect &rect,
                              ColorProp prop, int w,
                              int *qmin, int *qmax)
{
  static const double quantile = 0.01;

  QVector<double> stats(w, 0);
  for (int y=rect.top(); y<=rect.bottom(); y++)
    for (int x=rect.left(); x<=rect.right(); x++)
    {
      int pos = qBound(0, int(prop(img.pixel(x, y)) * (w-1)), w-1);
      stats[pos] += 1;
    }

  double k = 1.0/(rect.width()*rect.height());
  for (int p=0; p<w; p++)
    stats[p] *= k;

  if (qmin)
  {
    int q = 0;
    double s = 0;
    while (s<quantile)
      s += stats[q++];
    *qmin = q;
  }

  if (qmax)
  {
    int q = w-1;
    double s = 0;
    while (s<quantile)
      s += stats[q--];
    *qmax = q;
  }

  return stats;
}

double getLuma(QRgb rgb)
{
  return 0.2125*getRed(rgb) + 0.7154*getGreen(rgb) + 0.0721*getBlue(rgb); // BT.709
}

double getRed(QRgb rgb)
{
  return qRed(rgb)/255.0;
}

double getGreen(QRgb rgb)
{
  return qGreen(rgb)/255.0;
}

double getBlue(QRgb rgb)
{
  return qBlue(rgb)/255.0;
}
