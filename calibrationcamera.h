#ifndef CALIBRATIONCAMERA_H
#define CALIBRATIONCAMERA_H
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
#include <QFileInfoList>

using namespace cv;
using namespace std;

class CalibrationCamera
{
public:
    CalibrationCamera();
    //перечисление типов планаров
    enum Planar {CHESSBOARD, CIRCLESGRID, ASYMETRIC_CIRCLESGRID};
    void setMustInitUndistort(bool value) { mustInitUndistort = value; }
    // Калибровка камеры
    // открываем изображения шахматной доски и извлекаем угловые точки
    int addChessboardPoints(const QFileInfoList &filelist, // список с именами файлов
                            CalibrationCamera::Planar type, // тип планара
                            int numHeight, int numWidth); // размерность планара
    // возвращает ошибку повторного проецирования
    double calibrate(Mat &cameraMatrix, Mat &distCoeffs);
    // удалить искажение изображения (после калибровки)
    Mat remapImage(const Mat &image, const Mat &cameraMatrix, const Mat &distCoeffs);
    // установка параметров калибровки
    void setCalibrationFlag(bool radial8CoeffEnabled = false, bool tangentialParamEnabled = false);
private:
    // Getters
    Mat getCameraMatrix() { return cameraMatrix; }
    Mat getDistCoeffs()   { return distCoeffs; }
    bool getMustInitUndistort() const { return mustInitUndistort; }
    // Setters
    void setCameraMatrix(const Mat &cameraMatrix) {this->cameraMatrix = cameraMatrix;}
    void setDistCoeffs(const Mat &distCoeffs) {this->distCoeffs = distCoeffs;}
    // добавляем точки сцены и соответствующие точки изображения
    void addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners);
    //удаляем точки сцены и соответствующие точки изображения
    void clearPoints();
    // входные точки:
    // точки в мировых координатах
    // (каждый квадрат - одна единица)
    vector<vector<Point3f>> objectPoints;
    // позиции точек изображения в пикселях
    vector<vector<cv::Point2f>> imagePoints;
    // содержит изображение шахматной доски
    Size globalImageSize;
    // выходные матрицы
    Mat cameraMatrix;
    Mat distCoeffs;
    // флаг, чтобы указать, как выполняется калибровка
    int flag;
    //переменная, следящая за применением параметров один раз
    bool mustInitUndistort;
};

#endif // CALIBRATIONCAMERA_H
