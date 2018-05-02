#include "settings.h"

Settings::Settings()
{

}

bool Settings::saveSettings(const Mat &cameraMatrix, const Mat &distorsCoeffs, const String filePath)
{
    FileStorage fs(filePath, FileStorage::WRITE);
    if(fs.isOpened())
    {
    fs << "camera_matrix" << cameraMatrix;
    fs << "distors_coeffs" << distorsCoeffs;
    fs.release();
    return true;
    }
    else return false;
}

bool Settings::loadSettings(const String filePath, Mat &cameraMatrix, Mat &distorsCoeffs)
{
    FileStorage fs(filePath, FileStorage::READ);
    if(fs.isOpened())
    {
    fs["camera_matrix"] >> cameraMatrix;
    fs["distors_coeffs"] >> distorsCoeffs;
    fs.release();
    return true;
    }
    else return false;
}
