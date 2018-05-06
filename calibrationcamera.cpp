#include "calibrationcamera.h"

CalibrationCamera::CalibrationCamera():flag(0), mustInitUndistort(true)
{

}

int CalibrationCamera::addChessboardPoints(const QFileInfoList &filelist, Planar type, int numHeight, int numWidth)
{
    qDebug() << "Сработала функция addChessboardPoints()";
    //размерность планара
    Size boardSize(numHeight, numWidth);
    // точки на шахматной доске
    vector<Point2f> imageCorners;
    vector<Point3f> objectCorners;
    clearPoints();
    // точки 3D-сцены:
    // инициализация углов шахматной доски в системе отсчета шахматной доски
    // углы находятся в трехмерном местоположении (X, Y, Z) = (i, j, 0)
    for (int i = 0; i < boardSize.height; i++) {
        for (int j = 0; j < boardSize.width; j++) {
            objectCorners.push_back(Point3f(i, j, 0.0f));
        }
    }

    // 2D точки изображения:
    Mat image; // содержит изображение шахматной доски
    Mat grayImage; //содержит изображение шахматной доски в оттенках серого
    int successes = 0;
    // для всех изображений в списке файлов
    for (int i = 0; i < filelist.size(); i++)
    {
        // открываем изображение
        String file = filelist[i].absoluteFilePath().toUtf8().constData();

        if((filelist[i].suffix() != "jpg") &&
                (filelist[i].suffix() != "jpeg") &&
                (filelist[i].suffix() != "png") &&
                (filelist[i].suffix() != "tiff") &&
                (filelist[i].suffix() != "bmp") &&
                (filelist[i].suffix() != "jp2") &&
                (filelist[i].suffix() != "gif")) break;

        image = cv::imread(file, IMREAD_COLOR);

        bool found = false;

        //если тип планара - шахматная доска
        if(type == Planar::CHESSBOARD)
        {
            // получаем углы шахматной доски
            found = findChessboardCorners(
                        image, // изображение планара
                        boardSize, // размеры планара
                        imageCorners,
                        CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE); // список обнаруженных углов
        }
        //если тип планара - сетка с окружностями
        else if(type == Planar::CIRCLESGRID)
        {
            found = findCirclesGrid(image, boardSize, imageCorners);
        }
        //если тип планара - асиметричная сетка с окружностями
        else if(type == Planar::ASYMETRIC_CIRCLESGRID)
        {
            found = findCirclesGrid(image, boardSize, imageCorners, CALIB_CB_ASYMMETRIC_GRID);
        }

        // получаем точность субпикселей по углам
        cvtColor(image, grayImage, CV_BGR2GRAY);
        cornerSubPix(grayImage, imageCorners, Size(5, 5), Size(-1, -1), TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS, 30, 0.1));

        // если у нас хорошее изображение планара, добавляеи его к нашим данным

        if (imageCorners.size() == boardSize.area()) {
            // добавление изображений и точек сцены из одного вида
            addPoints(imageCorners, objectCorners);
            globalImageSize = image.size();
            successes++;
        }

        qDebug() << "i = " << i <<". Found = " << found;

        // рисуем углы
        drawChessboardCorners(image, boardSize, imageCorners, found);
        imshow(file.c_str(), image);
        waitKey(100);
        // закрыть окно с планаром
        destroyWindow(file.c_str());
    }
    return successes;
}

void CalibrationCamera::addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners)
{
    // 2D-изображение точек с одного вида
    this->imagePoints.push_back(imageCorners);
    // соответствующие точки 3D-сцены
    this->objectPoints.push_back(objectCorners);
}

void CalibrationCamera::clearPoints()
{
    // 2D-изображение точек с одного вида
    this->imagePoints.clear();
    // соответствующие точки 3D-сцены
    this->objectPoints.clear();
}

// Калибровка камеры
// возвращает ошибку повторного проецирования
double CalibrationCamera::calibrate(Mat &cameraMatrix, Mat &distCoeffs)
{
    if(this->imagePoints.size() < 1 or this->objectPoints.size() < 1)
        return -1.0;
    // выходные вращения и сдвиги
    vector<Mat> rvecs, tvecs;
    // запуск калибровки
    return calibrateCamera(this->objectPoints, // 3D точки
                           this->imagePoints,  // точки изображения
                           this->globalImageSize,    // размер изображения
                           cameraMatrix, // матрица выходных параметров камеры
                           distCoeffs,   // выходная матрица дисторсии
                           rvecs, tvecs, // параметры Rs, Ts
                           this->flag);        // опции при калибровке
}

// удаление искажений изображения (после калибровки)
Mat CalibrationCamera::remapImage(const Mat &image, const Mat &cameraMatrix, const Mat &distCoeffs)
{
    // используется при искажении изображения
    Mat map1, map2;
    Mat undistorted;
    initUndistortRectifyMap(cameraMatrix,  // вычисленная матрица выходных параметров камеры
                            distCoeffs,    // вычисленная выходная матрица дисторсии
                            Mat(),     // опциональное исправление (нет)
                            Mat(),     // матрица камеры для генерации неискаженных
                            image.size(),  // size of undistorted
                            CV_32FC1,      // type of output map
                            map1, map2);   // the x and y mapping functions
    // Применяем функции отображения
    remap(image, undistorted, map1, map2, INTER_LINEAR); // тип интерполяции
    return undistorted;
}

// установка параметров калибровки
// radial8CoeffEnabled должно быть true, если требуется 8 радиальных коэффициентов (по умолчанию 5)
// tangentialParamEnabled должно быть true, если присутствует тангенциальное искажение
void CalibrationCamera::setCalibrationFlag(bool radial8CoeffEnabled, bool tangentialParamEnabled) {
    // установка флага, используемого в calibrateCamera()
    flag = 0;
    if (!tangentialParamEnabled) flag += CV_CALIB_ZERO_TANGENT_DIST;
    if (radial8CoeffEnabled) flag += CV_CALIB_RATIONAL_MODEL;
}
