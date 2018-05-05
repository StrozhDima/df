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

double Core::calibrationCamera(const QFileInfoList &filelist, TypePlanar type, bool isRadial, bool isTangenc, int boardW, int boardH)
{
    //если выбран тип планара Шахматная доска
    if(type == TypePlanar::CHESSBOARD)
    {
        calibCamera.addChessboardPoints(filelist, CalibrationCamera::CHESSBOARD, boardW, boardH);
    }
    //если выбран тип планара Сетка с окружностями
    else if(type == TypePlanar::CIRCLESGRID)
    {
        calibCamera.addChessboardPoints(filelist, CalibrationCamera::CIRCLESGRID, boardW, boardH);
    }
    //если выбран тип планара ассиметричная сетка с окружностями
    else if(type == TypePlanar::ASYMETRIC_CIRCLESGRID)
    {
        calibCamera.addChessboardPoints(filelist, CalibrationCamera::ASYMETRIC_CIRCLESGRID, boardW, boardH);
    }

    //если выбран тип искажения радиальное
    if(isRadial)
        calibCamera.setCalibrationFlag(true, false);
    //если выбран тип искажения тангенциальное
    if(isTangenc)
        calibCamera.setCalibrationFlag(false, true);

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

bool Core::setCameraMatrix(const Mat &cameraMatrix)
{
    Mat diff = this->cameraMatrix != cameraMatrix;
    if(countNonZero(diff) != 0)
    {
        this->cameraMatrix = cameraMatrix;
        return true;
    }
    else return false;
}

bool Core::setDistCoeffs(const Mat &distCoeffs)
{
    this->distCoeffs = distCoeffs;
    return true;
}
