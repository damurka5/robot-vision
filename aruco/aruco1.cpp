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
    // VideoCapture cap(0);


    // std::vector<int> markerIds;
    // std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    // cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    // cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    
    // string path = "/Users/damirnurtdinov/Desktop/My Courses/Диплом/robot_vision/resources/markers_2.jpg";
    // cv::Mat inputImage = imread(path);
    // cv::Mat outputImage = inputImage.clone();
    // detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
    // cout<<markerCorners.size()<<"\n";
    // cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    // imshow("QR", outputImage);
    // waitKey(0);
    // while(true){
    //     Mat inputImage;
    //     cap.read(inputImage);
    //     detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
    //     cv::Mat outputImage = inputImage.clone();
    //     cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
    //     imshow("QR", outputImage);
    //     waitKey(1);
    // }

    // cv::Mat markerImage;
    // cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    // cv::aruco::generateImageMarker(dictionary, 3, 200, markerImage, 1);
    // cv::imwrite("marker3.png", markerImage);



    cv::VideoCapture inputVideo(1);
    float markerLength = 0.05;
    // You can read camera parameters from tutorial_camera_params.yml
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
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    aruco::ArucoDetector detector(dictionary, detectorParams);
    while (true) {
        cv::Mat image, imageCopy;
        inputVideo.read(image);
        image.copyTo(imageCopy);
        std::vector<int> ids;
        std::vector<std::vector<cv::Point2f>> corners;
        detector.detectMarkers(image, corners, ids);
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
        cv::imshow("out", imageCopy);
        char key = (char) cv::waitKey(1);
        if (key == 27)
            break;
    }
}