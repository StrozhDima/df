#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QImage>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

class Utils
{
public:
    Utils();
    static QString MatToString(const Mat &matrix);
    static void qImageToMat(const QImage &image, OutputArray out);
    static void matToQImage(InputArray image, QImage &out);
    static void rotateImage(QImage &image, int angle);
};

#endif // UTILS_H
