#include "calibrationui.h"
#include "ui_calibrationui.h"

CalibrationUI::CalibrationUI(Core *core, QWidget *parent): QMainWindow(parent), ui(new Ui::CalibrationUI)
{
    this->core = core;
    ui->setupUi(this);
    connect(this, SIGNAL(closeWindow()), this->parent(), SLOT(setSpinners()));
}

CalibrationUI::~CalibrationUI()
{
    delete ui;
}

void CalibrationUI::on_button_calibration_clicked()
{
    qDebug() << "нажата кнопка Выполнить калибровку";
    double error = 0.0;
    int boardW = (int)ui->spin_box_width->value() - 1;
    int boardH = (int)ui->spin_box_height->value() - 1;

    //список файлов в каталоге
    QDir dir(this->folderName);
    dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList filelist = dir.entryInfoList();
    qDebug() << "Файлов в директории: " << filelist.size();

    //если выбран тип планара Шахматная доска
    if(ui->radio_button_chess->isChecked())
        error = core->calibrationCamera(filelist, Core::TypePlanar::CHESSBOARD, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

    //если выбран тип планара Сетка с окружностями
    else if(ui->radio_button_circle_grid->isChecked())
        error = core->calibrationCamera(filelist, Core::TypePlanar::CIRCLESGRID, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

    //если выбран тип планара ассиметричная сетка с окружностями
    else if(ui->radio_button_as_circle_grid->isChecked())
        error = core->calibrationCamera(filelist, Core::TypePlanar::ASYMETRIC_CIRCLESGRID, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

    qDebug() << "Ошибка после калибровки: " << to_string(error).c_str();
    if(QMessageBox::information(this, "Успех", QString("Общая ошибка после калибровки: %1").arg(error)) == QMessageBox::Ok)
        emit closeWindow();
}

void CalibrationUI::on_open_folder_triggered()
{
    qDebug() << "нажата кнопка Открыть папку";
    this->folderName = FileManager::openFolder(this);
    const QString message = tr("Текущая директория: \"%1\"").arg(QDir::toNativeSeparators(this->folderName));
    statusBar()->showMessage(message);
}

void CalibrationUI::on_exit_calibration_triggered()
{
    this->close();
}
