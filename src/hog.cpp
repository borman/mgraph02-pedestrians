#include "hog.h"

const int BLOCK_SIZE = 8;
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

QVector<double> HOG::serialize() const
{
  QVector<double> res;
  res.reserve(HIST_SIZE * width() * height());
  for (int y=0; y<height(); y++)
    for (int x=0; x<width(); x++)
      res += at(x, y).data();
  return res;
}
