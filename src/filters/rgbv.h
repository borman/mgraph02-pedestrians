#ifndef RGBV_H
#define RGBV_H

// Floating-point RGB [0.0, 1.0] with arithmetics
struct RGBV
{
    RGBV(double _r=0.0, double _g=0.0, double _b=0.0) 
      : r(_r), g(_g), b(_b) {}
    RGBV(QRgb rgb)
    {
      r = qRed(rgb)   / 255.0;
      g = qGreen(rgb) / 255.0;
      b = qBlue(rgb)  / 255.0;
    }
    
    QRgb toQRgb() const
    {
      return qRgb(r*255, g*255, b*255);
    }

    void mulv(RGBV c)
    {
      r *= c.r;
      g *= c.g;
      b *= c.b;
    }
    void mul(double k)
    {
      r *= k;
      g *= k;
      b *= k;
    }
    void add(RGBV c)
    {
      r += c.r;
      g += c.g;
      b += c.b;
    }
    void addk(RGBV c, double k)
    {
      r += c.r * k;
      g += c.g * k;
      b += c.b * k;
    }
    void clamp()
    {
      r = qBound(0.0, r, 1.0);
      g = qBound(0.0, g, 1.0);
      b = qBound(0.0, b, 1.0);
    }
    double dot(RGBV c) const
    {
      return r*c.r + g*c.g + b*c.b;
    }

    double r, g, b;
};

#endif // RGBV_H
