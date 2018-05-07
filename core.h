#ifndef CORE_H
#define CORE_H
#include "calibrationcamera.h"
#include "manualcorrect.h"
#include "utils.h"

class Core
{
public:
    Core();
    enum TypePlanar {CHESSBOARD, CIRCLESGRID, ASYMETRIC_CIRCLESGRID};
    //Getters
    Mat getCameraMatrix();
    Mat getDistCoeffs();
    //Setters
    void setCameraMatrix(const Mat &cameraMatrix);
    void setDistCoeffs(const Mat &distCoeffs);
    void rotateImage(QImage &image, int angle);
    void setMustInitUndistort(bool doUndistort);
    double calibrationCamera(const QFileInfoList &filelist, int &success, TypePlanar type, bool isRadial, bool isTangenc, int boardW, int boardH);
    void undistortCameraCalibration(const QImage &imageSource, QImage &imageDest);
    void manualCorrection(const float strength, const float zoom, const QImage &inputImage, QImage &outputImage, bool antialias);
    void cropImage(const QImage &inputImage, QImage &outputImage, int height, int width);
private:
    CalibrationCamera calibCamera;
    ManualCorrect manualCorrecting;
    Mat cameraMatrix;
    Mat distCoeffs;
};

#endif // CORE_H
