#include "settings.h"

Settings::Settings()
{

}

bool Settings::saveSettings(const Mat &cameraMatrix, const Mat &distorsCoeffs, const String filePath)
{
    try
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
    catch(cv::Exception& e)
    {
        qDebug() << "Error to save settings" << e.what();
        return false;
    }
}

bool Settings::loadSettings(const String filePath, Mat &cameraMatrix, Mat &distorsCoeffs)
{
    try
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
    catch(cv::Exception& e)
    {
        qDebug() << "Error to open settings" << e.what();
        return false;
    }
}
