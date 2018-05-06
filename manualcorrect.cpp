#include "manualcorrect.h"

ManualCorrect::ManualCorrect()
{
}

QImage ManualCorrect::undistorsing(const float strength, const float zoom, const QImage &inputImage, bool antialias)
{
    QImage outputImage(inputImage.width(), inputImage.height(), inputImage.format());

    const uint width = inputImage.width();
    const uint height = inputImage.height();

    qDebug() << "Image width: " << width << " | image height: " << height;

    const size_t halfWidth = width / 2;
    const size_t halfHeight = height / 2;

    QRgb black = qRgba(0, 0, 0, 0);

    QVector<QRgb> originalPixels;

    for (uint j = 0; j < height; ++j)
        for (uint i = 0; i < width; ++i)
            originalPixels.push_back(inputImage.pixel(i, j));

    const float correctionRadius = sqrt(width * width + height * height) / strength;
    float theta = 0.0;

    for (uint y = 0; y < height; ++y)
    {
        //const uint offset = j * width;

        for (uint x = 0; x < width; ++x)
        {
            const int dx = x - halfWidth;   // смещение от центра изображения по X
            const int dy = y - halfHeight;  // смещение от центра изображения по Y

            const float distance = sqrt(dx * dx + dy * dy); // расстояние от центра
            const float r = distance / correctionRadius;

            if(r == 0)
                theta = 1;
            else
                theta = atan(r) / r;

            const float sourceX = halfWidth + theta * dx * zoom;
            const float sourceY = halfHeight + theta * dy * zoom;

            const uint sY = uint(sourceY);
            const uint sX = uint(sourceX);

            // выполнять с использованием интерполяции
            // граница обработки пикселей
            if (sX < width - 1 && sY < height - 1 && sX > 0 && sY > 0)
            {
                if(antialias)
                {

                    const float sXoverflow = sourceX - sX - 0.5f;
                    const float sYoverflow = sourceY - sY - 0.5f;

                    float weight[] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

                    if (sXoverflow < 0.5f && sYoverflow < 0.5f)
                    {
                        const float dx = 0.5f - sXoverflow;
                        const float dy = 0.5f - sYoverflow;
                        const float cdx = 1.0f - dx;
                        const float cdy = 1.0f - dy;

                        weight[0] = dx * dy;
                        weight[1] = dy * cdx;
                        weight[3] = dx * cdy;
                        weight[4] = cdx * cdy;
                    }
                    else if (sYoverflow < 0.5f && sXoverflow >= 0.5f)
                    {
                        const float dx = sXoverflow - 0.5f;
                        const float dy = 0.5f - sYoverflow;
                        const float cdx = 1.0f - dx;
                        const float cdy = 1.0f - dy;

                        weight[1] = dy * cdx;
                        weight[2] = dx * dy;
                        weight[4] = cdx * cdy;
                        weight[5] = dx * cdy;
                    }
                    else if (sYoverflow > 0.5f && sXoverflow < 0.5f)
                    {
                        const float dx = 0.5f - sXoverflow;
                        const float dy = sYoverflow - 0.5f;
                        const float cdx = 1.0f - dx;
                        const float cdy = 1.0f - dy;

                        weight[3] = dx * cdy;
                        weight[4] = cdx * cdy;
                        weight[6] = dx * dy;
                        weight[7] = cdx * dy;
                    }
                    else
                    {
                        const float dx = sXoverflow - 0.5f;
                        const float dy = sYoverflow - 0.5f;
                        const float cdx = 1.0f - dx;
                        const float cdy = 1.0f - dy;

                        weight[4] = cdx * cdy;
                        weight[5] = dx * cdy;
                        weight[7] = cdx * dy;
                        weight[8] = dx * dy;
                    }
                    //текущий цвет черный
                    QColor currentPixel = black;
                    //интерполяция 9 уровня
                    for (uint wi = 0; wi < 9; ++wi)
                    {
                        const float w = weight[wi];
                        if (w > 0)
                        {
                            const int j_offset = wi / 3 - 1;
                            const int i_offset = wi % 3 - 1;
                            //находим положение итогового пиксела
                            const uint sourceidx = width * (sY + j_offset) + sX + i_offset;
                            //добавляем интерлолированные цвета
                            QColor sourcePixel = originalPixels[sourceidx];
                            currentPixel.setRed(currentPixel.red() + w * sourcePixel.red());
                            currentPixel.setGreen(currentPixel.green() + w * sourcePixel.green());
                            currentPixel.setBlue(currentPixel.blue() + w * sourcePixel.blue());
                        }
                    }
                    //добавляем к изображению итоговые пиксели
                    outputImage.setPixelColor(x, y, currentPixel);
                }
                else
                {
                    //добавляем к изображению итоговые пиксели
                    outputImage.setPixelColor(x, y, inputImage.pixelColor(sX, sY));
                }

            }
            else
            {   //вместо пустых добавляем черный цвет
                outputImage.setPixelColor(x, y, black);
            }
        }
    }
    return outputImage;
}

//обрезаем до заданной ширины и высоты, вокруг его центральной точки
QImage ManualCorrect::cropBlackBorder(const QImage &inputImage, int height, int width)
{
    Mat image;
    Utils::qImageToMat(inputImage, image);
    Size imageSize = Size(inputImage.height(), inputImage.width());
    Point imageCenter = Point(int(imageSize.height * 0.5), int(imageSize.width * 0.5));

    if (width > inputImage.width() - 1)
        width = inputImage.width() - 1;

    if (height > inputImage.height() - 1)
        height = inputImage.height() - 1;

    int x1 = int(imageCenter.x - width * 0.5);
    int x2 = int(imageCenter.x + width * 0.5);
    int y1 = int(imageCenter.y - height * 0.5);
    int y2 = int(imageCenter.y + height * 0.5);

    qDebug() << "Crop: x1:" << x1 << "y1:" << y1 << " ; x2:" << x2 << "y2:" << y2;

    QImage outputImage;
    Utils::matToQImage(image(Rect(Point(x1, y1), Point(x2, y2))), outputImage);
    qDebug() << "Output image: " << outputImage.width() << "x" << outputImage.height();
    return outputImage;
}
