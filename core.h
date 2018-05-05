#ifndef CORE_H
#define CORE_H
#include "calibrationcamera.h"
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
    bool setCameraMatrix(const Mat &cameraMatrix);
    bool setDistCoeffs(const Mat &distCoeffs);

    void setMustInitUndistort(bool doUndistort);
    double calibrationCamera(const QFileInfoList &filelist, TypePlanar type, bool isRadial, bool isTangenc, int boardW, int boardH);
    void undistortCameraCalibration(const QImage &imageSource, QImage &imageDest);
private:
    CalibrationCamera calibCamera;
    Mat cameraMatrix;
    Mat distCoeffs;
};

#endif // CORE_H
