#include <cmath>
#include <QtAlgorithms>

#include "convolution.h"
#include "rgbv.h"

static inline QRgb &pixel(QImage &img, int x, int y)
{
  return ((QRgb *) img.scanLine(y))[x];
}

static inline QRgb pixel(const QImage &img, int x, int y)
{
  return ((QRgb *) img.scanLine(y))[x];
}

static QImage grow(const QImage &img, int size)
{
  QImage res(img.width() + size*2, img.height() + size*2, img.format());
  for (int y=0; y<res.height(); y++)
    for (int x=0; x<res.width(); x++)
    {
      int ox = qBound(0, x-size, img.width()-1);
      int oy = qBound(0, y-size, img.height()-1);
      pixel(res, x, y) = pixel(img, ox, oy);
    }
  return res;
}

static QRgb apply(const QImage &img, const Matrix<double> &m, int x, int y)
{
  RGBV acc;
  int size = (m.size()-1)/2;
  for (int dy=0; dy<m.size(); dy++)
    for (int dx=0; dx<m.size(); dx++)
      acc.addk(pixel(img, x+dx-size, y+dy-size), m.at(dx, dy));
  acc.clamp();
  return acc.toQRgb();
}

void convolve(QImage &img, const QRect &rect, const Matrix<double> &m)
{
  int size = (m.size()-1)/2;
  QImage tmp = grow(img, size);

  for (int y=rect.top(); y<=rect.bottom(); y++)
    for (int x=rect.left(); x<=rect.right(); x++)
      pixel(img, x, y) = apply(tmp, m, x+size, y+size);
}

// ===========

Matrix<double> gaussian(int size, double sigma)
{
  Matrix<double> m(size*2+1);
  double k1 = 2*sigma*sigma;
  double sum = 0;
  for (int y=0; y<m.size(); y++)
    for (int x=0; x<m.size(); x++)
    {
      m.set(x, y, exp(-((x-size)*(x-size) + (y-size)*(y-size))/k1));
      sum += m.at(x, y);
    }

  // Normalize
  for (int y=0; y<m.size(); y++)
    for (int x=0; x<m.size(); x++)
      m.set(x, y, m.at(x, y)/sum);

  return m;
}

Matrix<double> unsharp(int size, double sigma, double alpha)
{
  Matrix<double> m = gaussian(size, sigma);

  for (int y=0; y<m.size(); y++)
    for (int x=0; x<m.size(); x++)
      m.set(x, y, -alpha * m.at(x, y));

  m.set(size, size, m.at(size, size) + 1 + alpha); // Add identity

  return m;  
}

// ==========

static uchar findMedian(uchar *vs, int size)
{
  qSort(vs, vs+size);
  return vs[size/2];
}

static QRgb doMedian(const QImage &img, int size, int x, int y)
{
  int fsize = size*size;
  int hsize = (size-1)/2;
  Q_ASSERT(fsize <= 256);
  uchar vs[256];

  // Red
  QRgb res = qRgb(0, 0, 0);
  QRgb mask = 0xff;
  int shift = 0;
  for (int i=0; i<3; i++)
  {
    int p = 0;
    for (int dx=-hsize; dx<=hsize; dx++)
      for (int dy=-hsize; dy<=hsize; dy++)
        vs[p++] = (img.pixel(x+dx, y+dy) & mask) >> shift;

    int m = findMedian(vs, fsize);
    res |= m << shift;
    mask <<= 8;
    shift += 8;
  }
  return res;
}

void median(QImage &img, const QRect &rect, int size)
{
  int hsize = (size-1)/2;
  QImage tmp = grow(img, hsize);

  for (int y=rect.top(); y<=rect.bottom(); y++)
    for (int x=rect.left(); x<=rect.right(); x++)
      img.setPixel(x, y, doMedian(tmp, size, x+hsize, y+hsize));
}



