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
		const QString message = tr("Opened \"%1\", %2x%3, Depth: %4").arg(QDir::toNativeSeparators(this->fileName)).arg(this->image.width()).arg(this->image.height()).arg(this->image.depth());
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
		const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(this->fileName));
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
	qImageToMat(this->image, matImage);

	Mat newMatImage = this->calibUI->remapImage(matImage);

	matToQImage(newMatImage, this->image);
	setImageToLabel(this->image);
}

//преобразовать QImage в Mat
void UI::qImageToMat(const QImage& image, OutputArray out) {

	switch(image.format()) {
		case QImage::Format_Invalid:
		{
			Mat empty;
			empty.copyTo(out);
			break;
		}
		case QImage::Format_RGB32:
		{
			Mat view(image.height(),image.width(),CV_8UC4,(void *)image.constBits(),image.bytesPerLine());
			view.copyTo(out);
			break;
		}
		case QImage::Format_RGB888:
		{
			Mat view(image.height(),image.width(),CV_8UC3,(void *)image.constBits(),image.bytesPerLine());
			cvtColor(view, out, COLOR_RGB2BGR);
			break;
		}
		default:
		{
			QImage conv = image.convertToFormat(QImage::Format_ARGB32);
			Mat view(conv.height(),conv.width(),CV_8UC4,(void *)conv.constBits(),conv.bytesPerLine());
			view.copyTo(out);
			break;
		}
	}
}

//преобразовать Mat в QImage
//QImage::Format_ARGB32
void UI::matToQImage(InputArray image, QImage& out)
{
	switch(image.type())
	{
		case CV_8UC4:
		{
			Mat view(image.getMat());
			QImage view2(view.data, view.cols, view.rows, view.step[0], QImage::Format_ARGB32);
			out = view2.copy();
			break;
		}
		case CV_8UC3:
		{
			Mat mat;
			cvtColor(image, mat, COLOR_BGR2BGRA); //COLOR_BGR2RGB doesn't behave so use RGBA
			QImage view(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_ARGB32);
			out = view.copy();
			break;
		}
		case CV_8UC1:
		{
			Mat mat;
			cvtColor(image, mat, COLOR_GRAY2BGRA);
			QImage view(mat.data, mat.cols, mat.rows, mat.step[0], QImage::Format_ARGB32);
			out = view.copy();
			break;
		}
		default:
		{
			throw invalid_argument("Image format not supported");
			break;
		}
	}
}
