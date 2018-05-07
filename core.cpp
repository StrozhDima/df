#include "core.h"

Core::Core():cameraMatrix((Mat1d(3, 3) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1)),
    distCoeffs((Mat1d(1, 14) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0))
{
}

Mat Core::getCameraMatrix()
{
    return this->cameraMatrix;
}

Mat Core::getDistCoeffs()
{
    return this->distCoeffs;
}

void Core::setMustInitUndistort(bool doUndistort)
{
    this->calibCamera.setMustInitUndistort(doUndistort);
}

double Core::calibrationCamera(const QFileInfoList &filelist, int &success, TypePlanar type, bool isRadial, bool isTangenc, int boardW, int boardH)
{
    //если выбран тип планара Шахматная доска
    if(type == TypePlanar::CHESSBOARD)
    {
        success = calibCamera.addChessboardPoints(filelist, CalibrationCamera::CHESSBOARD, boardW, boardH);
    }
    //если выбран тип планара Сетка с окружностями
    else if(type == TypePlanar::CIRCLESGRID)
    {
        success = calibCamera.addChessboardPoints(filelist, CalibrationCamera::CIRCLESGRID, boardW, boardH);
    }
    //если выбран тип планара ассиметричная сетка с окружностями
    else if(type == TypePlanar::ASYMETRIC_CIRCLESGRID)
    {
        success = calibCamera.addChessboardPoints(filelist, CalibrationCamera::ASYMETRIC_CIRCLESGRID, boardW, boardH);
    }

    //если выбран тип искажения радиальное
    if(isRadial)
        calibCamera.setCalibrationFlag(true, false);
    //если выбран тип искажения тангенциальное
    if(isTangenc)
        calibCamera.setCalibrationFlag(false, true);

    this->cameraMatrix = ((Mat1d(3, 3) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
    this->distCoeffs = ((Mat1d(1, 14) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
    double error = calibCamera.calibrate(this->cameraMatrix, this->distCoeffs);
    qDebug() << "Ошибка после калибровки: " << to_string(error).c_str();
    return error;
}

void Core::undistortCameraCalibration(const QImage &imageSource, QImage &imageDest)
{
    Mat matImage;
    Utils::qImageToMat(imageSource, matImage);
    Utils::matToQImage(calibCamera.remapImage(matImage, this->cameraMatrix, this->distCoeffs), imageDest);
}

void Core::manualCorrection(const float strength, const float zoom, const QImage &inputImage, QImage &outputImage, bool antialias)
{
    float s = strength;
    float z = zoom;
    if(s <= 0.0) s = 0.00001f;
    if(z < 1.0) z = 1.0f;
    QImage newImage = this->manualCorrecting.undistorsing(s, z, inputImage, antialias);
    outputImage = newImage;
}

void Core::cropImage(const QImage &inputImage, QImage &outputImage, int height, int width)
{
    QImage newImage = this->manualCorrecting.cropBlackBorder(inputImage, height, width);
    outputImage = newImage;
}

void Core::setCameraMatrix(const Mat &cameraMatrix)
{
    this->cameraMatrix = ((Mat1d(3, 3) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0));
    this->cameraMatrix = cameraMatrix;
}

void Core::setDistCoeffs(const Mat &distCoeffs)
{
    this->distCoeffs = ((Mat1d(1, 14) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
    this->distCoeffs = distCoeffs;
}

void Core::rotateImage(QImage &image, int angle)
{
    Utils::rotateImage(image, angle);
}
