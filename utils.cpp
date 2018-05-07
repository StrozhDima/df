#include "utils.h"

QString Utils::MatToString(const Mat &matrix)
{
    ostringstream oss;
    oss << matrix << endl;
    QString matContent(oss.str().c_str());
    return matContent;
}

//преобразовать QImage в Mat
void Utils::qImageToMat(const QImage& image, OutputArray out) {

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
void Utils::matToQImage(InputArray image, QImage& out)
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

void Utils::rotateImage(QImage &image, int angle)
{
    QPoint center = image.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    image = image.transformed(matrix);
}
