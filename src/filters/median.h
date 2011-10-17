#ifndef CONVOLUTION_H
#define CONVOLUTION_H

#include <QImage>

template<class V>
class Matrix
{
  public:
    Matrix(int size)
      : m_size(size), m_d(new V[size*size])
    {
      memset(m_d, 0, size*size*sizeof(V));
    }
    Matrix(const Matrix &other)
      : m_size(other.m_size), m_d(new V[other.m_size*other.m_size])
    {
      memcpy(m_d, other.m_d, m_size*m_size*sizeof(V));
    }
    ~Matrix()
    {
      delete[] m_d;
    }

    int size() const { return m_size; }

    V at(int x, int y) const { return m_d[m_size*y + x]; }
    void set(int x, int y, V v) { m_d[m_size*y + x] = v; }

  private: 
    int m_size;
    V *m_d;
};

// Filter generators. Matrix size is 2*halfsize+1
Matrix<double> unsharp(int halfsize, double sigma, double alpha);
Matrix<double> gaussian(int halfsize, double sigma);

void convolve(QImage &img, const QRect &rect, const Matrix<double> &m);

void median(QImage &img, const QRect &rect, int size);

#endif // CONVOLUTION_H
