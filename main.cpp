#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include "utils/StereoImageProcessing.h"

using namespace cv;
using namespace std;


int main() {
    cv::VideoCapture inputVideo(0);
    float markerLength = 0.05;
    // You can read camera parameters from tutorial_camera_params.yml
    float intrinsicMat[3][3] = {{2.36308422e+03, 0.0, 9.46684665e+02},
                                {0.0, 2.28733771e+03, 2.87930582e+02},
                                {0, 0, 1}};
    float distMat[5] = {0.83953412, -10.9281040,  -0.02852879,   0.03361264,  26.40532812};
    cv::Mat cameraMatrix = cv::Mat(3,3, CV_32FC1, &intrinsicMat);
    cv::Mat distMatrix = cv::Mat(1,5, CV_32FC1, &distMat);
    // Set coordinate system
    cv::Mat objPoints(4, 1, CV_32FC3);
    objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength/2.f, -markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength/2.f, -markerLength/2.f, 0);
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    aruco::ArucoDetector detector(dictionary, detectorParams);

    cv::Mat imageUndistorted;
    while (true) {
        cv::Mat image, imageCopy;
        inputVideo.read(image);
        image.copyTo(imageCopy);
        cv::undistort(image, imageUndistorted, cameraMatrix, distMatrix);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        detector.detectMarkers(imageCopy, corners, ids);
        // If at least one marker detected
        if (ids.size() > 0) {
            cv::aruco::drawDetectedMarkers(imageCopy, corners, ids);
            int nMarkers = corners.size();
            std::vector<cv::Vec3d> rvecs(nMarkers), tvecs(nMarkers);
            // Calculate pose for each marker
            for (int i = 0; i < nMarkers; i++) {
                solvePnP(objPoints, corners.at(i), cameraMatrix, distMatrix, rvecs.at(i), tvecs.at(i));
            }
            // Draw axis for each marker
            for(unsigned int i = 0; i < ids.size(); i++) {
                cv::drawFrameAxes(imageCopy, cameraMatrix, distMatrix, rvecs[i], tvecs[i], 0.1);
            }
        }
        // Show resulting image and close window
        cv::imshow("out", imageUndistorted);
        char key = (char) cv::waitKey(1);
        if (key == 27)
            break;
    }


}

