#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <QColor>
#include <QPixmap>
#include <QVector>

// Pixel property accessor (R, G, B, Luma, etc) -> double [0, 1]
typedef double (*ColorProp)(QRgb color);

QPixmap drawHistogram(const QImage &img, const QRect &rect,
                      ColorProp prop, int w, int h,
                      const QColor &bg, const QColor &fg);

// Make histogram with resolution w
// First and last percentiles optionally returned in qmin, qmax
QVector<double> makeHistogram(const QImage &img, const QRect &rect,
                              ColorProp prop, int w,
                              int *qmin, int *qmax);

double getLuma(QRgb rgb);
double getRed(QRgb rgb);
double getGreen(QRgb rgb);
double getBlue(QRgb rgb);

#endif // HISTOGRAM_H
