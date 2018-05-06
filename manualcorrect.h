#ifndef MANUALCORRECT_H
#define MANUALCORRECT_H
#include <QImage>
#include <QDebug>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "utils.h"

class ManualCorrect
{
public:
    ManualCorrect();
    QImage undistorsing(const float strength, const float zoom, const QImage &inputImage, bool antialias);
    QImage cropBlackBorder(const QImage &inputImage, int height, int width);
};

#endif // MANUALCORRECT_H
