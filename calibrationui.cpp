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
    if(this->folderName.length() > 1)
    {
        double error = 0.0;
        int success = 0;
        int boardW = (int)ui->spin_box_width->value() - 1;
        int boardH = (int)ui->spin_box_height->value() - 1;

        //список файлов в каталоге
        QDir dir(this->folderName);
        dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
        dir.setSorting(QDir::Name | QDir::Reversed);
        QFileInfoList filelist = dir.entryInfoList();
        qDebug() << "Файлов в директории: " << filelist.size();

        //если выбран тип планара Шахматная доска
        if(ui->radio_button_chess->isChecked())
            error = core->calibrationCamera(filelist, success, Core::TypePlanar::CHESSBOARD, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

        //если выбран тип планара Сетка с окружностями
        else if(ui->radio_button_circle_grid->isChecked())
            error = core->calibrationCamera(filelist, success, Core::TypePlanar::CIRCLESGRID, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

        //если выбран тип планара ассиметричная сетка с окружностями
        else if(ui->radio_button_as_circle_grid->isChecked())
            error = core->calibrationCamera(filelist, success, Core::TypePlanar::ASYMETRIC_CIRCLESGRID, ui->check_box_radial->isChecked(), ui->check_box_tangenc->isChecked(), boardW, boardH);

        if(error < 0.0)
        {
            QMessageBox::warning(this, "Ошибка", QString("Ничего не найдено в директории: %1").arg(this->folderName));
            return;
        }
        qDebug() << "Ошибка после калибровки: " << to_string(error).c_str();
        if(QMessageBox::information(this, "Успех", QString("Распознано %1 объектов из %2. Общая ошибка после калибровки: %3").arg(success).arg(filelist.size()).arg(error)) == QMessageBox::Ok)
            emit closeWindow();
    }
    else QMessageBox::warning(this, "Ошибка", QString("Неверная директория с данными: %1").arg(this->folderName));
}

void CalibrationUI::on_open_folder_triggered()
{
    qDebug() << "нажата кнопка Открыть папку";
    this->folderName = FileManager::openFolder(this);
    const QString message = tr("Открыт: \"%1\"").arg(QDir::toNativeSeparators(this->folderName));
    statusBar()->showMessage(message);
}

void CalibrationUI::on_exit_calibration_triggered()
{
    this->close();
}
