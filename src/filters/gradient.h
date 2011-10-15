#ifndef GRADIENT_H
#define GRADIENT_H

#include <QImage>
#include "matrix.h"

struct Polar
{
  double phi;
  double r;
};

Matrix<Polar> gradient(const QImage &src, const QRect &rect);
QImage showGradient(const Matrix<Polar> &gr);

#endif // GRADIENT_H
