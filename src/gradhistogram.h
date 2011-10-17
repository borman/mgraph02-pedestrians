#ifndef GRADHISTOGRAM_H
#define GRADHISTOGRAM_H

#include <QVector>

class GradHistogram
{
public:
  GradHistogram(int nbuckets = 0)
    : m_buckets(nbuckets, 0) {}

  void add(double phi, double r);
  QVector<double> data() const { return m_buckets; }

  double norm() const;
  GradHistogram &operator *=(double k);
private:
  QVector<double> m_buckets;
};

#endif // GRADHISTOGRAM_H
