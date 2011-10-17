#ifndef HOG_H
#define HOG_H

#include "matrix.h"
#include "gradhistogram.h"
#include "gradient.h"

class HOG: public Matrix<GradHistogram>
{
  public:
    HOG(const Matrix<Polar> &gr);

    HOG &normalize();

    QVector<double> serialize() const { return serialize(0, 0, width(), height()); }
    QVector<double> serialize(int x1, int y1, int w, int h) const;
};

#endif // HOG_H
