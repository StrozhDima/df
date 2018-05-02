#ifndef CALICRATIONUI_H
#define CALICRATIONUI_H

#include <QMainWindow>
#include "filemanager.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <QDebug>
#include <QThread>

namespace Ui {
class CalibrationUI;
}

using namespace cv;
using namespace std;

class CalibrationUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit CalibrationUI(QWidget *parent = 0);
    ~CalibrationUI();
    QFileInfoList getFilesFromDir(QString folderPath);
    void getIntristicParams(QString folderPath);
    // открываем изображения шахматной доски и извлекаем угловые точки
    int addChessboardPoints(
            const QFileInfoList &filelist, // список с именами файлов
            Size &boardSize); // размер планара
    // добавляем точки сцены и соответствующие точки изображения
    void addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners);
    // Калибровка камеры
    // возвращает ошибку повторного проецирования
    double calibrate(Size &imageSize);
    // удалить искажение изображения (после калибровки)
    Mat remapImage(const Mat &image);
    // установка параметров калибровки
    void setCalibrationFlag(bool radial8CoeffEnabled = false, bool tangentialParamEnabled = false);
    // Getters
    Mat getCameraMatrix() { return cameraMatrix; }
    Mat getDistCoeffs()   { return distCoeffs; }
    // Setters
    void setCameraMatrix(const Mat &cameraMatrix) {this->cameraMatrix = cameraMatrix;}
    void setDistCoeffs(const Mat &distCoeffs) {this->distCoeffs = distCoeffs;}
private slots:
    void on_button_calibration_clicked();
    void on_open_folder_triggered();
    void on_exit_calibration_triggered();
private:
    Ui::CalibrationUI *ui;
    QFileDialog *fileDialog;
    QString folderName;
    QString params;

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
    // используется при искажении изображения
    Mat map1,map2;
    bool mustInitUndistort;
};

#endif // CALICRATIONUI_H
