#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

#define OBJ_AREA 13200

Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;
Mat imgEdged, mask;

int canny_threshold1 = 5, dilKernel1 = 1, min = 0;
int canny_threshold2 = 100, blurSize = 3, max = 255;

// comparison function object
bool compareContourAreas ( std::vector<cv::Point> contour1, std::vector<cv::Point> contour2 ) {
    double i = fabs( contourArea(cv::Mat(contour1)) );
    double j = fabs( contourArea(cv::Mat(contour2)) );
    return ( i < j );
}

Point getContourOfObj(Mat imgDil, Mat img){
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
    sort(largeContours.begin(), largeContours.end(), compareContourAreas);
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


    // object detection
    boundRect = boundingRect(conPoly[0]);

    cout << objectType << endl;
    drawContours(img, conPoly, -1, Scalar(255, 0, 255), 2);

    rectangle(img, boundRect.tl(), boundRect.br(), Scalar(0, 255, 0), 5);
    putText(img, objectType, {boundRect.tl().x, boundRect.tl().y - 5}, FONT_HERSHEY_PLAIN, 3, Scalar(0, 0, 255), 2);

    Point objCenter(boundRect.tl().x + (boundRect.br().x - boundRect.tl().x) / 2,
                    boundRect.tl().y + (boundRect.br().y - boundRect.tl().y) / 2);
    circle(img, objCenter, 5, (0, 0, 0), 2);

    return Point(boundRect.tl().x + (boundRect.br().x-boundRect.tl().x)/2,
                 boundRect.tl().y + (boundRect.br().y-boundRect.tl().y)/2);
}


int main() {
    string path1 = "../resources/left_pic4.jpg";
    string path2 = "../resources/middle_pic4.jpg";
    Mat imgL = imread(path1);
    Mat imgR = imread(path2);
    int img_height = 960;  // px
    int img_weight = 1280;  // px

    float f = 26;  // focal length in mm (camera of iPhone 13)
    float b_x = 100; // baseline in mm

    vector<Mat> channels;
    Mat hsv;
    cvtColor(imgL, hsv, COLOR_BGR2HSV);

    Scalar lower(16, 0, 0);
    Scalar upper(175, 255, 255);
    inRange(hsv, lower, upper, mask);

    GaussianBlur(mask, imgBlur, Size(5, 5), 0, 0);

    canny_threshold1 = 200; // empirically
    Canny(imgBlur, imgCanny, canny_threshold1, 3 * canny_threshold1);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(imgCanny, imgDil, kernel);

    Point p = getContourOfObj(imgDil, imgL); // i=4 for left_pic4, i = 6 for middle_pic4

    imshow("Image Left", imgL);
    cout << "x: " << p.x << " y: " << p.y << "\n";

    waitKey(0);

}

