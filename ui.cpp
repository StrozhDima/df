#include "ui.h"

UI::UI(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::UI),
	labelImage(new QLabel(this)),
	fileDialog(new QFileDialog),
	scaleFactor(1.0),
	calibUI(new CalibrationUI(this))
{
	ui->setupUi(this);
	this->calibUI->setParent(this);
	ui->scrollArea->setWidget(this->labelImage);
	this->labelImage->setAlignment(Qt::AlignCenter);
	this->labelImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	this->calibUI->setCameraMatrix((Mat1d(3, 3) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1));
	this->calibUI->setDistCoeffs((Mat1d(1, 5) << 0.0, 0.0, 0.0, 0.0, 0.0));
	updateActions();
}

UI::~UI(){delete ui;}

//установить изображение и показать
void UI::setImageToLabel(const QImage &image)
{
	*this->pixmap = QPixmap::fromImage(image);
	this->labelImage->setPixmap(*this->pixmap);
	this->labelImage->resize(this->labelImage->pixmap()->size());
}

//изменение размера показываемого изображения
void UI::scaleImage(double factor)
{
	scaleFactor *= factor;
	this->labelImage->setPixmap(this->pixmap->scaled(this->pixmap->size() * scaleFactor));
	this->labelImage->resize(this->labelImage->pixmap()->size());
	ui->zoom_in->setEnabled(scaleFactor < 3.0);
	ui->zoom_out->setEnabled(scaleFactor > 0.25);
}

//обновить статусы всех кнопок меню
void UI::updateActions()
{
	ui->save_file->setEnabled(!this->fileName.isNull());
	ui->save_file_as->setEnabled(!this->fileName.isNull());
	ui->close_file->setEnabled(!this->fileName.isNull());
	ui->zoom_in->setEnabled(!this->fileName.isNull());
	ui->zoom_out->setEnabled(!this->fileName.isNull());
	ui->normal_size->setEnabled(!this->fileName.isNull());
	ui->fil_to_window->setEnabled(!this->fileName.isNull());
	ui->push_button_apply->setEnabled(!this->fileName.isNull());
}

//применить параметры
void UI::applySettings()
{
	this->processedImage = this->image;
	qDebug() << "Pressed button Apply";
	Mat matImage;
	Mat newMatImage;
	Utils::qImageToMat(this->processedImage, matImage);

	//вкладка Диагональный тип
	if((ui->tabWidget->currentIndex() == 0) && this->calibUI->getMustInitUndistort())
	{
		newMatImage = this->calibUI->remapImage(matImage);
		Utils::matToQImage(newMatImage, this->processedImage);
		setImageToLabel(this->processedImage);
	}
	//вкладка Циркулярный тип
	if((ui->tabWidget->currentIndex() == 1) && this->calibUI->getMustInitUndistort())
	{
		newMatImage = this->calibUI->remapImage(matImage);
		Utils::matToQImage(newMatImage, this->processedImage);
		setImageToLabel(this->processedImage);
	}
}

//нажата кнопка меню "Открыть"
void UI::on_open_file_triggered()
{
	FileManager::initialize(fileDialog, QFileDialog::AcceptOpen, "image/jpeg", "jpg");
	if(this->fileDialog->exec() == QDialog::Accepted) {
		this->fileName = this->fileDialog->selectedFiles().first();
		this->processedImage = this->image = FileManager::loadImageFile(this->fileName);
		const QString message = tr("Открыт \"%1\", %2x%3, Глубина: %4").arg(QDir::toNativeSeparators(this->fileName)).arg(this->image.width()).arg(this->image.height()).arg(this->image.depth());
		statusBar()->showMessage(message);
		setImageToLabel(this->image);
		updateActions();
		this->calibUI->setMustInitUndistort(true);
	}
}

//нажата кнопка меню "Сохранить как..."
void UI::on_save_file_as_triggered()
{
	FileManager::initialize(fileDialog, QFileDialog::AcceptSave, "image/jpeg", "jpg");
	if(this->fileDialog->exec() == QDialog::Accepted) {
		this->fileName = this->fileDialog->selectedFiles().first();
		FileManager::saveImageFile(this->fileName, this->processedImage);
		const QString message = tr("Сохранен \"%1\"").arg(QDir::toNativeSeparators(this->fileName));
		statusBar()->showMessage(message);
		updateActions();
	}
}

//нажата кнопка меню "Закрыть"
void UI::on_close_file_triggered()
{
	this->fileName = nullptr;
	this->pixmap = new QPixmap();
	this->labelImage->setPixmap(*this->pixmap);
	this->labelImage->resize(ui->scrollArea->size());
	updateActions();
}

//нажата кнопка "Выйти"
void UI::on_exit_app_triggered(){this->close();}

//нажата кнопка "Увеличить"
void UI::on_zoom_in_triggered(){scaleImage(1.25);}

//нажата кпонка "Уменьшить"
void UI::on_zoom_out_triggered(){scaleImage(0.85);}

//нажата кнопка "Нормальный размер"
void UI::on_normal_size_triggered()
{
	this->labelImage->setPixmap(QPixmap::fromImage(this->processedImage));
	this->labelImage->resize(this->labelImage->pixmap()->size());
}

//нажата кнопка "Вписать в окно"
void UI::on_fil_to_window_triggered()
{
	this->labelImage->setPixmap(this->pixmap->scaled(ui->scrollArea->width(), ui->scrollArea->height(),  Qt::KeepAspectRatio));
	this->labelImage->resize(this->labelImage->pixmap()->size());
}

//нажата кнопка "Калибровка камеры"
void UI::on_calibr_camera_triggered()
{
	qDebug() << "нажата кнопка Калибровка камеры";
	this->calibUI->setWindowModality(Qt::ApplicationModal);
	this->calibUI->show();
}

//нажата кнопка "Применить"
void UI::on_push_button_apply_clicked()
{
	setMatrixes();
	applySettings();
}

//нажата кнопка "Сохранить настройки"
void UI::on_save_settings_triggered()
{
	QString fileName =  QFileDialog::getSaveFileName(0, "Сохранение параметров камеры", QDir::currentPath(), "Matrix (*.yml);;Matrix (*.xml);;All files (*.*)", new QString("Matrix (*.yml)"));

	qDebug() << "Save matrix to file:" << fileName;

	if(fileName.length() > 1)
	{
		if(Settings::saveSettings(this->calibUI->getCameraMatrix(), this->calibUI->getDistCoeffs(),fileName.toStdString().c_str()))
			QMessageBox::information(this, "Успех", QString("Настройки успешно сохранены в файл: %1").arg(fileName));
		else
			QMessageBox::critical(this, "Ошибка!", QString("Настройки не были сохранены в файл: %1").arg(fileName));

		QString cameraMatrix = Utils::settingsToString(this->calibUI->getCameraMatrix());
		QString distorsCoeff = Utils::settingsToString(this->calibUI->getDistCoeffs());
		qDebug() << "Saved camera matrix:" << cameraMatrix;
		qDebug() << "Saved distors coeffs:" << distorsCoeff;
	}
}

//нажата кнопка "Загрузить настройки"
void UI::on_open_settings_triggered()
{
	QString fileName = QFileDialog::getOpenFileName(0, "Загрузка параметров камеры", QDir::currentPath(), "Matrix (*.yml);;Matrix (*.xml);;All files (*.*)", new QString("Matrix (*.yml)"));

	qDebug() << "Open matrix from file:" << fileName;

	if(fileName.length() > 1)
	{
		Mat cameraMatrix;
		Mat distorsCoeff;
		if(Settings::loadSettings(fileName.toStdString().c_str(), cameraMatrix, distorsCoeff))
		{
			QMessageBox::information(this, "Успех", QString("Настройки успешно загружены из файла: %1").arg(fileName));
			this->calibUI->setCameraMatrix(cameraMatrix);
			this->calibUI->setDistCoeffs(distorsCoeff);
			updateSpinners();
			this->calibUI->setMustInitUndistort(true);
		}
		else
			QMessageBox::critical(this, "Ошибка!", QString("Настройки не были загружены из файла: %1").arg(fileName));

		QString strCameraMatrix = Utils::settingsToString(this->calibUI->getCameraMatrix());
		QString strDistorsCoeff = Utils::settingsToString(this->calibUI->getDistCoeffs());
		qDebug() << "Opened camera matrix:" << strCameraMatrix;
		qDebug() << "Opened distors coeffs:" << strDistorsCoeff;
	}
}

//изменение параметров на вкладке Диагональный тип
/*void UI::on_spin_box_fx_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_cx_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_fy_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_cy_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k1_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k2_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k3_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_p1_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_p2_diag_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

//изменение параметров на вкладке Циркулярный тип
void UI::on_spin_box_fx_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_fy_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_cx_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_cy_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k1_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k2_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_k3_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_p1_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}

void UI::on_spin_box_p2_cir_valueChanged(double arg1)
{
	setMatrixes();
	this->calibUI->setMustInitUndistort(true);
}*/

void UI::setMatrixes()
{
	Mat cameraMatrix;
	Mat distortionCoefficients;
	//вкладка Диагональный тип
	if(ui->tabWidget->currentIndex() == 0)
	{
		qDebug() << "Tab diagonal enable";
		cameraMatrix = (Mat1d(3, 3) <<
						ui->spin_box_fx_diag->value(), 0,
						ui->spin_box_cx_diag->value(), 0,
						ui->spin_box_fy_diag->value(), ui->spin_box_cy_diag->value(),
						0, 0, 1);

		distortionCoefficients = (Mat1d(1, 5) <<
								  ui->spin_box_k1_diag->value(),
								  ui->spin_box_k2_diag->value(),
								  ui->spin_box_p1_diag->value(),
								  ui->spin_box_p2_diag->value(),
								  ui->spin_box_k3_diag->value());

		this->calibUI->setCameraMatrix(cameraMatrix);
		this->calibUI->setDistCoeffs(distortionCoefficients);
	}
	//вкладка Циркулярный тип
	if(ui->tabWidget->currentIndex() == 1)
	{
		qDebug() << "Tab circular enable";
		cameraMatrix = (Mat1d(3, 3) <<
						ui->spin_box_fx_cir->value(), 0,
						ui->spin_box_cx_cir->value(), 0,
						ui->spin_box_fy_cir->value(), ui->spin_box_cy_diag->value(),
						0, 0, 1);

		distortionCoefficients = (Mat1d(1, 5) <<
								  ui->spin_box_k1_cir->value(),
								  ui->spin_box_k2_cir->value(),
								  ui->spin_box_p1_cir->value(),
								  ui->spin_box_p2_cir->value(),
								  ui->spin_box_k3_cir->value());

		this->calibUI->setCameraMatrix(cameraMatrix);
		this->calibUI->setDistCoeffs(distortionCoefficients);

	}

	this->calibUI->setMustInitUndistort(true);

	//Mat cameraMatrix = (Mat1d(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
	//Mat distortionCoefficients = (Mat1d(1, 5) << k1, k2, p1, p2, k3);
}

void UI::updateSpinners()
{
	qDebug() << "Update spinners";
	qDebug() << "fx=" << this->calibUI->getCameraMatrix().at<double>(0, 0);
	qDebug() << "fy=" << this->calibUI->getCameraMatrix().at<double>(1, 1);
	qDebug() << "cx=" << this->calibUI->getCameraMatrix().at<double>(0, 2);
	qDebug() << "cy=" << this->calibUI->getCameraMatrix().at<double>(1, 2);
	qDebug() << "k1=" << this->calibUI->getDistCoeffs().at<double>(0, 0);
	qDebug() << "k2=" << this->calibUI->getDistCoeffs().at<double>(0, 1);
	qDebug() << "p1=" << this->calibUI->getDistCoeffs().at<double>(0, 2);
	qDebug() << "p2=" << this->calibUI->getDistCoeffs().at<double>(0, 3);
	qDebug() << "k3=" << this->calibUI->getDistCoeffs().at<double>(0, 4);

	ui->spin_box_fx_diag->setValue(this->calibUI->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_fy_diag->setValue(this->calibUI->getCameraMatrix().at<double>(1, 1));
	ui->spin_box_cx_diag->setValue(this->calibUI->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_cy_diag->setValue(this->calibUI->getCameraMatrix().at<double>(1, 2));
	ui->spin_box_k1_diag->setValue(this->calibUI->getDistCoeffs().at<double>(0, 0));
	ui->spin_box_k2_diag->setValue(this->calibUI->getDistCoeffs().at<double>(0, 1));
	ui->spin_box_k3_diag->setValue(this->calibUI->getDistCoeffs().at<double>(0, 4));
	ui->spin_box_p1_diag->setValue(this->calibUI->getDistCoeffs().at<double>(0, 2));
	ui->spin_box_p2_diag->setValue(this->calibUI->getDistCoeffs().at<double>(0, 3));

	ui->spin_box_fx_cir->setValue(this->calibUI->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_fy_cir->setValue(this->calibUI->getCameraMatrix().at<double>(1, 1));
	ui->spin_box_cx_cir->setValue(this->calibUI->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_cy_cir->setValue(this->calibUI->getCameraMatrix().at<double>(1, 2));
	ui->spin_box_k1_cir->setValue(this->calibUI->getDistCoeffs().at<double>(0, 0));
	ui->spin_box_k2_cir->setValue(this->calibUI->getDistCoeffs().at<double>(0, 1));
	ui->spin_box_k3_cir->setValue(this->calibUI->getDistCoeffs().at<double>(0, 4));
	ui->spin_box_p1_cir->setValue(this->calibUI->getDistCoeffs().at<double>(0, 2));
	ui->spin_box_p2_cir->setValue(this->calibUI->getDistCoeffs().at<double>(0, 3));

	this->calibUI->setMustInitUndistort(true);
}
