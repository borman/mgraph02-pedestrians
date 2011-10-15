#include <cmath>
#include "gradient.h"
#include "rgbv.h"

static inline double getLuma(QRgb rgb)
{
  return 0.2125*qRed(rgb) + 0.7154*qGreen(rgb) + 0.0721*qBlue(rgb); // BT.709
}

static inline RGBV grad(QRgb c1, QRgb c2, QRgb c3)
{
  RGBV res(c3);
  res.addk(c1, -1);
  return res;
}

static inline double len(double y, double x)
{
  return sqrt(x*x + y*y);
}

Matrix<Polar> gradient(const QImage &src, const QRect &rect)
{
  Matrix<Polar> dest(rect.width(), rect.height());
  for (int y0=0; y0<rect.height(); y0++)
    for (int x0=0; x0<rect.width(); x0++)
    {
      int x = x0+rect.left();
      int y = y0+rect.top();

      int x1 = x==0? 1 : x-1;
      int y1 = y==0? 1 : y-1;
      int x2 = x==src.width()-1? x-1 : x+1;
      int y2 = y==src.height()-1? y-1 : y+1;

      RGBV vgx = grad(src.pixel(x1, y), src.pixel(x, y), src.pixel(x2, y));
      RGBV vgy = grad(src.pixel(x, y1), src.pixel(x, y), src.pixel(x, y2));

      double vgphi[] = {
        atan2(vgy.r, vgx.r),
        atan2(vgy.g, vgx.g),
        atan2(vgy.b, vgx.b)
      };
      double vgl[] = {
        len(vgy.r, vgx.r),
        len(vgy.g, vgx.g),
        len(vgy.b, vgx.b)
      };

      int maxi = 0;
      if (vgl[1] > vgl[maxi])
        maxi = 1;
      if (vgl[2] > vgl[maxi])
        maxi = 2;

      dest.at(x0, y0).r = vgl[maxi];
      dest.at(x0, y0).phi = vgphi[maxi];
    }
  return dest;
}

QImage showGradient(const Matrix<Polar> &gr)
{
  QImage res(gr.width(), gr.height(), QImage::Format_RGB32);
  for (int y=0; y<gr.height(); y++)
    for (int x=0; x<gr.width(); x++)
    {
      double v = gr.at(x, y).r / sqrt(2);
      /*
      double kr = abs(sin(gr.at(x, y).phi));
      double kg = abs(cos(gr.at(x, y).phi));
      RGBV c(v, v, v);
      c.clamp();
      */
      res.setPixel(x, y, qRgb(v*255, v*255, v*255));
    }
  return res;
}

