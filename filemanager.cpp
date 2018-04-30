#include "filemanager.h"

FileManager::FileManager()
{
}

//загрузка файла изображния
QImage FileManager::loadFile(const QString fileName)
{
    QImage image;
    QImageReader reader(fileName);
    if(reader.canRead()) {
        reader.setAutoTransform(true);
        image = reader.read();
    }
        return image;
}

//сохранение файла изображения
bool FileManager::saveFile(const QString &fileName, const QImage &image)
{
    QImageWriter writer(fileName);
    if (!writer.canWrite())
        return false;
    if (!writer.write(image))
        return false;
    return true;
}

//инициализация QFileDialog и установка фильтров
//const QString &mimeTypeFilter - "image/jpeg" (для изображений jpeg)
//const QString &suffix - "jpg" (формат jpg)
void FileManager::initialize(QFileDialog *dialog, QFileDialog::AcceptMode acceptMode, const QString &mimeTypeFilter, const QString &suffix)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog->setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
            ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
    mimeTypeFilters.sort();
    dialog->setMimeTypeFilters(mimeTypeFilters);
    dialog->selectMimeTypeFilter(mimeTypeFilter);
    if (acceptMode == QFileDialog::AcceptSave)
        dialog->setDefaultSuffix(suffix);

}
