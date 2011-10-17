#include <cmath>
#include "hog.h"

const int BLOCK_SIZE = 8;
const int CLUSTER_SIZE = 3;
const int HIST_SIZE = 9;

HOG::HOG(const Matrix<Polar> &gr)
  : Matrix<GradHistogram>((gr.width() + BLOCK_SIZE-1) / BLOCK_SIZE,
                          (gr.height() + BLOCK_SIZE-1) / BLOCK_SIZE)
{
  for (int y=0; y<height(); y++)
    for (int x=0; x<width(); x++)
    {
      at(x, y) = GradHistogram(HIST_SIZE);
      int x0m = qMin(gr.width(), (x+1)*BLOCK_SIZE);
      int y0m = qMin(gr.height(), (y+1)*BLOCK_SIZE);
      for (int y0=y*BLOCK_SIZE; y0<y0m; y0++)
        for (int x0=x*BLOCK_SIZE; x0<x0m; x0++)
          at(x, y).add(gr.at(x0, y0).phi, gr.at(x0, y0).r);
    }
}

QVector<double> HOG::serialize(int x1, int y1, int w, int h) const
{
  QVector<double> res;
  res.reserve(HIST_SIZE * w * h);
  for (int y=0; y<h; y++)
    for (int x=0; x<w; x++)
      res += at(x1+x, y1+y).data();
  return res;
}

HOG &HOG::normalize()
{
  Matrix<double> coeffs(width(), height());
  const int hsize = (CLUSTER_SIZE - 1)/2;

  for (int y=0; y<height(); y++)
    for (int x=0; x<width(); x++)
    {
      double norm = 0;

      int x1 = qMax(0, x-hsize);
      int y1 = qMax(0, y-hsize);
      int x2 = qMin(x+hsize+1, width());
      int y2 = qMin(y+hsize+1, height());

      for (int py=y1; py<y2; py++)
        for (int px=x1; px<x2; px++)
          norm += at(px, py).norm();

      coeffs.at(x, y) = 1/sqrt(norm + 1e-8);
    }

  for (int y=0; y<height(); y++)
    for (int x=0; x<width(); x++)
      at(x, y) *= coeffs.at(x, y);

  return *this;
}
