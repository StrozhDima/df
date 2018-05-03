#ifndef UI_H
#define UI_H

#include "ui_ui.h"
#include <QDebug>
#include <QMainWindow>
#include <QLabel>
#include "filemanager.h"
#include "calibrationui.h"
#include "settings.h"
#include "utils.h"

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

    /*void on_spin_box_fx_diag_valueChanged(double arg1);
    void on_spin_box_cx_diag_valueChanged(double arg1);
    void on_spin_box_fy_diag_valueChanged(double arg1);
    void on_spin_box_cy_diag_valueChanged(double arg1);
    void on_spin_box_k1_diag_valueChanged(double arg1);
    void on_spin_box_k2_diag_valueChanged(double arg1);
    void on_spin_box_k3_diag_valueChanged(double arg1);
    void on_spin_box_p1_diag_valueChanged(double arg1);
    void on_spin_box_p2_diag_valueChanged(double arg1);

    void on_spin_box_fx_cir_valueChanged(double arg1);
    void on_spin_box_fy_cir_valueChanged(double arg1);
    void on_spin_box_cx_cir_valueChanged(double arg1);
    void on_spin_box_cy_cir_valueChanged(double arg1);
    void on_spin_box_k1_cir_valueChanged(double arg1);
    void on_spin_box_k2_cir_valueChanged(double arg1);
    void on_spin_box_k3_cir_valueChanged(double arg1);
    void on_spin_box_p1_cir_valueChanged(double arg1);
    void on_spin_box_p2_cir_valueChanged(double arg1);
*/
public slots:
    void updateSpinners();
private:
    Ui::UI *ui;
    QFileDialog *fileDialog;
    double scaleFactor;
    QImage image;
    QImage processedImage;
    QLabel *labelImage;
    QString fileName;
    QPixmap *pixmap;
    CalibrationUI *calibUI;
};

#endif // UI_H
