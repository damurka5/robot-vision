//
// Created by Damir Nurtdinov on 10/21/23.
//

#include "StereoImageProcessing.h"

#define OBJ_AREA 13200

using namespace cv;
using namespace std;

// comparison function object
bool StereoImageProcessing::compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

cv::Point StereoImageProcessing::getContourOfObj(cv::Mat imgDil, cv::Mat img) {
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    drawContours(img, contours, -1, Scalar(255,0, 255), 2);
    vector<vector<Point>> largeContours;
    for (int j =0; j<contours.size(); j++){
        if(contourArea(contours[j]) > 1000){
            largeContours.push_back(contours[j]);
        }
    }
    sort(largeContours.begin(), largeContours.end(), StereoImageProcessing::compareContourAreas);
    vector<Point> myObj;

    for (int j =0; j<largeContours.size(); j++){
        if(contourArea(largeContours[j]) > OBJ_AREA-500 && contourArea(largeContours[j]) < OBJ_AREA+500){
            myObj = largeContours[j];
        }
    }

    vector<vector<Point>> conPoly(1);
    Rect boundRect;
    string objectType = "Object detected";

    float peri = arcLength(myObj, true);
    approxPolyDP(myObj, conPoly[0], 0.02 * peri, true);
    cout << objectType << endl;

    // object detection
    boundRect = boundingRect(conPoly[0]);


    drawContours(img, conPoly, -1, Scalar(255, 0, 255), 2);

    rectangle(img, boundRect.tl(), boundRect.br(), Scalar(0, 255, 0), 5);
//    putText(img, objectType, {boundRect.tl().x, boundRect.tl().y - 5}, FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 255), 2);

    Point objCenter(boundRect.tl().x + (boundRect.br().x - boundRect.tl().x) / 2,
                    boundRect.tl().y + (boundRect.br().y - boundRect.tl().y) / 2);
    circle(img, objCenter, 3, (0, 0, 0), 3);

    return Point(boundRect.tl().x + (boundRect.br().x-boundRect.tl().x)/2,
                 boundRect.tl().y + (boundRect.br().y-boundRect.tl().y)/2);
}

cv::Mat StereoImageProcessing::applyDilation(cv::Mat src, cv::Mat res, int cannyThreshold1, int cannyThreshold2, int blurKernel, int dilationKernel) {
    Mat blured, canny;
    GaussianBlur(src, blured, Size(blurKernel, blurKernel), 0, 0);
    Canny(blured, canny, cannyThreshold1, cannyThreshold2);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(dilationKernel, dilationKernel));
    dilate(canny, res, kernel);
    return res;
}

