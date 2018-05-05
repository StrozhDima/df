#ifndef CALICRATIONUI_H
#define CALICRATIONUI_H

#include <QMainWindow>
#include "filemanager.h"
#include "core.h"
#include <QDebug>
#include <QThread>

namespace Ui {
class CalibrationUI;
}

class CalibrationUI : public QMainWindow
{
    Q_OBJECT

public:
    CalibrationUI(Core *core, QWidget *parent = 0);
    ~CalibrationUI();
private slots:
    void on_button_calibration_clicked();
    void on_open_folder_triggered();
    void on_exit_calibration_triggered();
signals:
    void closeWindow();
private:
    QFileInfoList getFilesFromDir(QString folderPath);
    Ui::CalibrationUI *ui;
    Core *core;
    QFileDialog *fileDialog;
    QString folderName;
};

#endif // CALICRATIONUI_H
