#ifndef COLORCORRECT_H
#define COLORCORRECT_H

#include <QImage>

void whitebalance(QImage &img, const QRect &rect);
void luma_stretch(QImage &img, const QRect &rect);
void rgb_stretch(QImage &img, const QRect &rect);

#endif // COLORCORRECT_H

