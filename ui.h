#ifndef UI_H
#define UI_H

#include <QDebug>
#include<QMainWindow>
#include<QLabel>
#include"filemanager.h"

namespace Ui {
class UI;
}

class UI : public QMainWindow
{
    Q_OBJECT

public:
    explicit UI(QWidget *parent = 0);
    ~UI();
    void setImageToLabel(const QImage &image);
    void scaleImage(double factor);
    void updateActions();
private slots:
    void on_open_file_triggered();
    void on_save_file_as_triggered();
    void on_close_file_triggered();
    void on_exit_app_triggered();
    void on_zoom_in_triggered();
    void on_zoom_out_triggered();
    void on_normal_size_triggered();
    void on_fil_to_window_triggered();
    void on_calibr_camera_triggered();
private:
    Ui::UI *ui;
    QFileDialog *fileDialog;
    double scaleFactor;
    QImage image;
    QLabel *labelImage;
    QString fileName;
    QPixmap pixmap;
};

#endif // UI_H