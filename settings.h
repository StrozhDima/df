#ifndef SETTINGS_H
#define SETTINGS_H
#include <opencv2/core.hpp>
#include <QDebug>

using namespace cv;

class Settings
{
public:
    Settings();
    static bool saveSettings(const Mat &cameraMatrix, const Mat &distorsCoeffs, const String filePath);
    static bool loadSettings(const String filePath, Mat &cameraMatrix, Mat &distorsCoeffs);
};

#endif // SETTINGS_H
