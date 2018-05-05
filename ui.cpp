#include "ui.h"

UI::UI(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::UI),
	labelImage(new QLabel(this)),
	fileDialog(new QFileDialog),
	scaleFactor(1.0),
	core(new Core),
	calibUI(new CalibrationUI(this->core, this))
{
	ui->setupUi(this);
	this->calibUI->setParent(this);
	ui->scrollArea->setWidget(this->labelImage);
	this->labelImage->setAlignment(Qt::AlignCenter);
	this->labelImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

//нажата кнопка меню "Открыть"
void UI::on_open_file_triggered()
{
	FileManager::initialize(fileDialog, QFileDialog::AcceptOpen, "image/jpeg", "jpg");
	if(this->fileDialog->exec() == QDialog::Accepted) {
		this->fileName = this->fileDialog->selectedFiles().first();
		this->image = FileManager::loadImageFile(this->fileName);
		const QString message = tr("Открыт \"%1\", %2x%3, Глубина: %4").arg(QDir::toNativeSeparators(this->fileName)).arg(this->image.width()).arg(this->image.height()).arg(this->image.depth());
		statusBar()->showMessage(message);
		setImageToLabel(this->image);
		updateActions();
		updateManualSpinners(false);
		core->setMustInitUndistort(true);
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
	updateManualSpinners(true);
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
	qDebug() << "Нажата кнопка Применить";
	//вкладка Автоматическая коррекция
	if(ui->tabWidget->currentIndex() == 0)
	{
	setMatrixes();
	applySettings();
	}

	applyManualSetting();
}

//нажата кнопка "Сохранить настройки"
void UI::on_save_settings_triggered()
{
	setMatrixes();
	QString fileName =  QFileDialog::getSaveFileName(0, "Сохранение параметров камеры", QDir::currentPath(), "Matrix (*.yml);;Matrix (*.xml);;All files (*.*)", new QString("Matrix (*.yml)"));
	qDebug() << "Save matrix to file:" << fileName;
	if(fileName.length() > 1)
	{
		if(Settings::saveSettings(this->core->getCameraMatrix(), this->core->getDistCoeffs(),fileName.toStdString().c_str()))
			QMessageBox::information(this, "Успех", QString("Настройки успешно сохранены в файл: %1").arg(fileName));
		else
			QMessageBox::critical(this, "Ошибка!", QString("Настройки не были сохранены в файл: %1").arg(fileName));

		QString cameraMatrix = Utils::MatToString(this->core->getCameraMatrix());
		QString distorsCoeff = Utils::MatToString(this->core->getDistCoeffs());
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
			this->core->setCameraMatrix(cameraMatrix);
			this->core->setDistCoeffs(distorsCoeff);
			setSpinners();
			core->setMustInitUndistort(true);
		}
		else
			QMessageBox::critical(this, "Ошибка!", QString("Настройки не были загружены из файла: %1").arg(fileName));

		QString strCameraMatrix = Utils::MatToString(this->core->getCameraMatrix());
		QString strDistorsCoeff = Utils::MatToString(this->core->getDistCoeffs());
		qDebug() << "Opened camera matrix:" << strCameraMatrix;
		qDebug() << "Opened distors coeffs:" << strDistorsCoeff;
	}
}

void UI::on_slider_strength_sliderMoved(int position)
{
	qDebug() << "Сработал slider_strength: " << position;
	applyManualSetting();
}

void UI::on_slider_zoom_sliderMoved(int position)
{
	qDebug() << "Сработал slider_zoom: " << position;
	applyManualSetting();
}

//применить параметры
void UI::applySettings()
{
	qDebug() << "Сработал метод applySettings()";
	core->undistortCameraCalibration(this->image, this->processedImage);
	setImageToLabel(this->processedImage);
}

void UI::applyManualSetting()
{
	//вкладка Ручная коррекция
	if(ui->tabWidget->currentIndex() == 1)
	{
		qDebug() << "вкладка Ручная коррекция enable";
		core->manualCorrection(float(ui->slider_strength->value()/1000.0f), float(ui->slider_zoom->value()/1000.0f), this->image, this->processedImage);
		QImage cropImage;
		core->cropImage(this->processedImage, cropImage, ui->spin_box_height->value(), ui->spin_box_width->value());
		this->processedImage = cropImage;
		setImageToLabel(this->processedImage);
	}
}

//Mat cameraMatrix = (Mat1d(3, 3) << fx, 0, cx, 0, fy, cy, 0, 0, 1);
//Mat distortionCoefficients = (Mat1d(1, 5) << k1, k2, p1, p2, k3);
void UI::setMatrixes()
{
	qDebug() << "Сработал метод setMatrixes()";
	//вкладка Автоматическая коррекция
	if(ui->tabWidget->currentIndex() == 0)
	{
		qDebug() << "вкладка Автоматическая коррекция enable";
		Mat cameraMatrix = (Mat1d(3, 3) <<
							ui->spin_box_fx->value(), 0,
							ui->spin_box_cx->value(), 0,
							ui->spin_box_fy->value(), ui->spin_box_cy->value(),
							0, 0, 1);

		Mat distortionCoefficients = (Mat1d(1, 14) <<
									  ui->spin_box_k1->value(),
									  ui->spin_box_k2->value(),
									  ui->spin_box_p1->value(),
									  ui->spin_box_p2->value(),
									  ui->spin_box_k3->value(),
									  ui->spin_box_k4->value(),
									  ui->spin_box_k5->value(),
									  ui->spin_box_k6->value(),
									  0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

		if(this->core->setCameraMatrix(cameraMatrix))
			core->setMustInitUndistort(true);
		if(this->core->setDistCoeffs(distortionCoefficients))
			core->setMustInitUndistort(true);
		QString strCameraMatrix = Utils::MatToString(this->core->getCameraMatrix());
		QString strDistorsCoeff = Utils::MatToString(this->core->getDistCoeffs());
		qDebug() << "Set camera matrix:" << strCameraMatrix;
		qDebug() << "Set distors coeffs:" << strDistorsCoeff;
	}
}

void UI::setSpinners()
{
	qDebug() << "Update spinners";
	qDebug() << "fx=" << this->core->getCameraMatrix().at<double>(0, 0);
	qDebug() << "fy=" << this->core->getCameraMatrix().at<double>(1, 1);
	qDebug() << "cx=" << this->core->getCameraMatrix().at<double>(0, 2);
	qDebug() << "cy=" << this->core->getCameraMatrix().at<double>(1, 2);
	qDebug() << "k1=" << this->core->getDistCoeffs().at<double>(0, 0);
	qDebug() << "k2=" << this->core->getDistCoeffs().at<double>(0, 1);
	qDebug() << "p1=" << this->core->getDistCoeffs().at<double>(0, 2);
	qDebug() << "p2=" << this->core->getDistCoeffs().at<double>(0, 3);
	qDebug() << "k3=" << this->core->getDistCoeffs().at<double>(0, 4);
	qDebug() << "k4=" << this->core->getDistCoeffs().at<double>(0, 5);
	qDebug() << "k5=" << this->core->getDistCoeffs().at<double>(0, 6);
	qDebug() << "k6=" << this->core->getDistCoeffs().at<double>(0, 7);

	ui->spin_box_fx->setValue(this->core->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_fy->setValue(this->core->getCameraMatrix().at<double>(1, 1));
	ui->spin_box_cx->setValue(this->core->getCameraMatrix().at<double>(0, 2));
	ui->spin_box_cy->setValue(this->core->getCameraMatrix().at<double>(1, 2));
	ui->spin_box_k1->setValue(this->core->getDistCoeffs().at<double>(0, 0));
	ui->spin_box_k2->setValue(this->core->getDistCoeffs().at<double>(0, 1));
	ui->spin_box_p1->setValue(this->core->getDistCoeffs().at<double>(0, 2));
	ui->spin_box_p2->setValue(this->core->getDistCoeffs().at<double>(0, 3));
	ui->spin_box_k3->setValue(this->core->getDistCoeffs().at<double>(0, 4));
	ui->spin_box_k4->setValue(this->core->getDistCoeffs().at<double>(0, 5));
	ui->spin_box_k5->setValue(this->core->getDistCoeffs().at<double>(0, 6));
	ui->spin_box_k6->setValue(this->core->getDistCoeffs().at<double>(0, 7));
	core->setMustInitUndistort(true);

}

void UI::updateManualSpinners(bool zero)
{
	if(zero)
	{
		ui->spin_box_width->setValue(0);
		ui->spin_box_height->setValue(0);
	}
	else
	{
		ui->spin_box_width->setValue(this->image.width());
		ui->spin_box_height->setValue(this->image.height());
	}
}
