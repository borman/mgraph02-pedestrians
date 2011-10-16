#ifndef HOG_H
#define HOG_H

#include "matrix.h"
#include "gradhistogram.h"
#include "gradient.h"

class HOG: public Matrix<GradHistogram>
{
  public:
    HOG(const Matrix<Polar> &gr);

    QVector<double> serialize() const;
};

#endif // HOG_H
