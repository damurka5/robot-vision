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
    VideoCapture cap(1);


    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    while(true){
        Mat inputImage;
        cap.read(inputImage);
        detector.detectMarkers(inputImage, markerCorners, markerIds, rejectedCandidates);
        cv::Mat outputImage = inputImage.clone();
        cv::aruco::drawDetectedMarkers(outputImage, markerCorners, markerIds);
        imshow("QR", outputImage);
        waitKey(1);
    }
}