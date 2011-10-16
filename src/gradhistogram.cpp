#include <cmath>
#include "gradhistogram.h"

void GradHistogram::add(double phi, double r)
{
  int n = m_buckets.size();
  double step = M_PI / n;

  phi = abs(phi);
  double p = phi / step;
  int p1 = floor(p - 0.5);
  int p2 = floor(p + 0.5);

  double k = p - p1;

  if (p1 < 0) p1 = n-1;
  if (p2 > n-1) p2 = 0;
  m_buckets[p1] += r*(1-k);
  m_buckets[p2] += r*k;
}
