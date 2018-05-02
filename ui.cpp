#include "ui.h"

UI::UI(QWidget *parent) : QMainWindow(parent),
	ui(new Ui::UI),
	labelImage(new QLabel(this)),
	fileDialog(new QFileDialog),
	scaleFactor(1.0),
	calibUI(new CalibrationUI(this))
{
	ui->setupUi(this);
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
	}
}

//нажата кнопка меню "Сохранить как..."
void UI::on_save_file_as_triggered()
{
	FileManager::initialize(fileDialog, QFileDialog::AcceptSave, "image/jpeg", "jpg");
	if(this->fileDialog->exec() == QDialog::Accepted) {
		this->fileName = this->fileDialog->selectedFiles().first();
		FileManager::saveImageFile(this->fileName, this->image);
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
	this->labelImage->setPixmap(QPixmap::fromImage(this->image));
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
	qDebug() << "Format image: " << this->image.format();
	Mat matImage;
	Utils::qImageToMat(this->image, matImage);

	Mat newMatImage = this->calibUI->remapImage(matImage);

	Utils::matToQImage(newMatImage, this->image);
	setImageToLabel(this->image);
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
	}
	else
		QMessageBox::critical(this, "Ошибка!", QString("Настройки не были загружены из файла: %1").arg(fileName));

	QString strCameraMatrix = Utils::settingsToString(this->calibUI->getCameraMatrix());
	QString strDistorsCoeff = Utils::settingsToString(this->calibUI->getDistCoeffs());
	qDebug() << "Opened camera matrix:" << strCameraMatrix;
	qDebug() << "Opened distors coeffs:" << strDistorsCoeff;
	}
}

