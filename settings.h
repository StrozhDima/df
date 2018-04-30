#ifndef SETTINGS_H
#define SETTINGS_H
#include<QLabel>

class Settings
{
public:
    Settings();
    void setImageToLabel(QLabel &label, const QImage &image);
};

#endif // SETTINGS_H
