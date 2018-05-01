#include "calibrationui.h"
#include "ui_calibrationui.h"

CalibrationUI::CalibrationUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CalibrationUI),
    flag(0),
    mustInitUndistort(true)
{
    ui->setupUi(this);
    calibrationPattern = NOT_EXISTING;
}

CalibrationUI::~CalibrationUI()
{
    delete ui;
}

QFileInfoList CalibrationUI::getFilesFromDir(QString folderPath)
{
    QDir dir(folderPath);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();
    qDebug() << "Files in folder: " << list.size();
    return list;
}

void CalibrationUI::getIntristicParams(QString folderPath)
{
    int boardW = (int)ui->spin_box_width->value();
    int boardH = (int)ui->spin_box_height->value();

    //количество внутренних углов на шахматной доске
    Size boardSize(boardW - 1, boardH - 1);
    //список файлов в каталоге
    QFileInfoList list = getFilesFromDir(folderPath);
    addChessboardPoints(list, boardSize);

    if(ui->radio_button_radial->isChecked())
        setCalibrationFlag(true, false);

    if(ui->radio_button_tangencial->isChecked())
        setCalibrationFlag(false, true);

    double error = calibrate(globalImageSize);

    qDebug() << "Error after calibrate: " << to_string(error).c_str();
    QMessageBox::information(this, "Успех", QString("Общая ошибка после калибровки: %1").arg(error));
}

int CalibrationUI::addChessboardPoints(const QFileInfoList &filelist, Size &boardSize)
{
    // точки на шахматной доске
    vector<Point2f> imageCorners;
    vector<Point3f> objectCorners;
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
        String file = filelist[i].absoluteFilePath().toUtf8().constData();;
        image = cv::imread(file, IMREAD_COLOR);

        bool found = false;

        if(ui->radio_button_chess->isChecked())
        {
            // получаем углы шахматной доски
            found = findChessboardCorners(
                        image, // изображение планара
                        boardSize, // размеры планара
                        imageCorners,
                        CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE); // список обнаруженных углов
        }

        else if(ui->radio_button_circle_grid->isChecked())
        {
            found = findCirclesGrid(image, boardSize, imageCorners);
        }

        else if(ui->radio_button_as_circle_grid->isChecked())
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
        waitKey(500);
        // закрыть окно с планаром
        destroyWindow(file.c_str());
    }
    return successes;
}

void CalibrationUI::on_button_calibration_clicked()
{
    qDebug() << "нажата кнопка Выполнить калибровку";
    getIntristicParams(this->folderName);
}

void CalibrationUI::on_open_folder_triggered()
{
    qDebug() << "нажата кнопка Открыть папку";
    this->folderName = FileManager::openFolder(this);
    const QString message = tr("Opened \"%1\"").arg(QDir::toNativeSeparators(this->folderName));
    statusBar()->showMessage(message);
}

void CalibrationUI::addPoints(const vector<Point2f> &imageCorners, const vector<Point3f> &objectCorners)
{
    // 2D-изображение точек с одного вида
    imagePoints.push_back(imageCorners);
    // соответствующие точки 3D-сцены
    objectPoints.push_back(objectCorners);
}

// Калибровка камеры
// возвращает ошибку повторного проецирования
double CalibrationUI::calibrate(Size &imageSize)
{
    // mustInitUndistort должен быть повторно инициализирован
    mustInitUndistort = true;
    // выходные вращения и сдвиги
    vector<Mat> rvecs, tvecs;
    // запуск калибровки
    return calibrateCamera(objectPoints, // 3D точки
                           imagePoints,  // точки изображения
                           imageSize,    // размер изображения
                           cameraMatrix, // матрица выходных параметров камеры
                           distCoeffs,   // выходная матрица дисторсии
                           rvecs, tvecs, // параметры Rs, Ts
                           flag);        // опции при калибровке
    //					,CV_CALIB_USE_INTRINSIC_GUESS);
}

// удаление искажений изображения (после калибровки)
Mat CalibrationUI::remapImage(const Mat &image)
{
    Mat undistorted;

    if (mustInitUndistort) { // вызывается один раз для каждой калибровки

        initUndistortRectifyMap(cameraMatrix,  // вычисленная матрица выходных параметров камеры
                                distCoeffs,    // вычисленная выходная матрица дисторсии
                                Mat(),     // опциональное исправление (нет)
                                Mat(),     // матрица камеры для генерации неискаженных
                                image.size(),  // size of undistorted
                                CV_32FC1,      // type of output map
                                map1, map2);   // the x and y mapping functions
        mustInitUndistort = false;
    }
    // Применяем функции отображения
    remap(image, undistorted, map1, map2, INTER_LINEAR); // тип интерполяции
    return undistorted;
}

// установка параметров калибровки
// radial8CoeffEnabled должно быть true, если требуется 8 радиальных коэффициентов (по умолчанию 5)
// tangentialParamEnabled должно быть true, если присутствует тангенциальное искажение
void CalibrationUI::setCalibrationFlag(bool radial8CoeffEnabled, bool tangentialParamEnabled) {
    // установка флага, используемого в calibrateCamera()
    flag = 0;
    if (!tangentialParamEnabled) flag += CV_CALIB_ZERO_TANGENT_DIST;
    if (radial8CoeffEnabled) flag += CV_CALIB_RATIONAL_MODEL;
}

void CalibrationUI::on_exit_calibration_triggered()
{
    this->close();
}
