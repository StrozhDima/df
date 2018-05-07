#ifndef UI_H
#define UI_H

#include "ui_ui.h"
#include <QDebug>
#include <QMainWindow>
#include <QLabel>
#include "filemanager.h"
#include "calibrationui.h"
#include "settings.h"

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
    void setMatrixes();
    void applySettings();
    void updateManualSpinners(bool zero);
    void applyManualSetting();
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
    void on_push_button_apply_clicked();
    void on_save_settings_triggered();
    void on_open_settings_triggered();
    void on_slider_strength_sliderMoved(int position);
    void on_slider_zoom_sliderMoved(int position);
    void on_save_file_triggered();
    void on_rotate_right_triggered();
    void on_rotate_left_triggered();
    void on_reset_triggered();
public slots:
    void setSpinners();
private:
    Ui::UI *ui;
    Core *core;
    QFileDialog *fileDialog;
    double scaleFactor;
    QImage image;
    QImage processedImage;
    QImage originImage;
    QLabel *labelImage;
    QString fileName;
    QPixmap *pixmap;
    CalibrationUI *calibUI;
};

#endif // UI_H
