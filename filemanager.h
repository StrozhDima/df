#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include<QString>
#include<QFileDialog>
#include<QImageReader>
#include<QMessageBox>
#include<QImageWriter>
#include<QStringList>
#include<QStandardPaths>

class FileManager
{
public:
    FileManager();
    static QImage loadImageFile(const QString fileName);
    static bool saveImageFile(const QString &fileName, const QImage &image);
    static void initialize(QFileDialog *dialog, QFileDialog::AcceptMode acceptMode, const QString &mimeTypeFilter, const QString &suffix);
    static QString openFolder(QWidget *parent);
    static QString loadSettingsFile();
private:

};

#endif // FILEMANAGER_H
