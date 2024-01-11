#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/aruco.hpp>

using namespace cv;
using namespace std;

// exploration of https://docs.opencv.org/4.x/d5/dae/tutorial_aruco_detection.html

int main() {
    float markerLength = 28;
    float intrinsicMat[3][3] = {{886.45390975, 0.0, 346.18649671},
                                {0.0, 885.64408409, 669.6104976},
                                {0, 0, 1}};
    float distMat[5] = {-0.05252045,  0.05994806,  0.00199345, -0.02194643, -0.06142772};
    cv::Mat cameraMatrix = cv::Mat(3,3, CV_32FC1, &intrinsicMat);
    cv::Mat distMatrix = cv::Mat(1,5, CV_32FC1, &distMat);

    // Set coordinate system
    cv::Mat objPoints(4, 1, CV_32FC3);
    objPoints.ptr<cv::Vec3f>(0)[0] = cv::Vec3f(-markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[1] = cv::Vec3f(markerLength/2.f, markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[2] = cv::Vec3f(markerLength/2.f, -markerLength/2.f, 0);
    objPoints.ptr<cv::Vec3f>(0)[3] = cv::Vec3f(-markerLength/2.f, -markerLength/2.f, 0);

    string path = "/Users/damirnurtdinov/Desktop/My Courses/Диплом/robot_vision/resources/markers_2.jpg";
    cv::Mat inputImage = imread(path);
    cv::Mat outputImage = inputImage.clone();

    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
    // cout<<markerCorners.size()<<"\n";
    int nMarkers = markerCorners.size();
    std::vector<cv::Vec3d> rvecs(nMarkers), tvecs(nMarkers);
    cv::Mat R;
    // Calculate pose for each marker
    for (int i = 0; i < nMarkers; i++) {
        solvePnP(objPoints, markerCorners.at(i), cameraMatrix, distMatrix, rvecs.at(i), tvecs.at(i));
        std::cout<<"rvec: ";
        cv::Rodrigues(rvecs.at(i), R);
        std::cout<<R<<"\n";

        std::cout<<"tvec: ";
        std::cout<<tvecs.at(i)<<"\n";
    }

    inputImage.copyTo(outputImage);
    for (int i = 0; i < rvecs.size(); ++i) {
        auto rvec = rvecs[i];
        auto tvec = tvecs[i];
        cv::drawFrameAxes(outputImage, cameraMatrix, distMatrix, rvec, tvec, 0.1);
    }
    imshow("QR", outputImage);
    waitKey(0);
}