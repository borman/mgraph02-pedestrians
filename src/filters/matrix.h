#ifndef MATRIX_H
#define MATRIX_H

template<class V>
class Matrix
{
public:
  Matrix(int w, int h)
    : m_width(w), m_height(h), m_d(new V[w*h])
  {
  }
  Matrix(const Matrix &other)
    : m_width(other.m_width), m_height(other.m_height),
      m_d(new V[other.m_width*other.m_height])
  {
    memcpy(m_d, other.m_d, m_width*m_height*sizeof(V));
  }
  ~Matrix()
  {
    delete[] m_d;
  }

  int width() const { return m_width; }
  int height() const { return m_height; }

  V& at(int x, int y) { return m_d[m_width*y + x]; }
  const V& at(int x, int y) const { return m_d[m_width*y + x]; }

private:
  int m_width;
  int m_height;
  V *m_d;
};

#endif // MATRIX_H
