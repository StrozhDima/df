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
    static QImage loadFile(const QString fileName);
    static bool saveFile(const QString &fileName, const QImage &image);
    static void initialize(QFileDialog *dialog, QFileDialog::AcceptMode acceptMode, const QString &mimeTypeFilter, const QString &suffix);
private:

};

#endif // FILEMANAGER_H
