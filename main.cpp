#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;

#define OBJ_AREA 13200

Mat imgGray, imgBlurL, imgBlurR, imgCannyL, imgCannyR, imgDilL, imgDilR, imgErode;
Mat imgEdged, maskL, maskR;

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
    Mat hsvL, hsvR;
    cvtColor(imgL, hsvL, COLOR_BGR2HSV);
    cvtColor(imgR, hsvR, COLOR_BGR2HSV);

    Scalar lower(16, 0, 0);
    Scalar upper(175, 255, 255); // 175, 255, 255
    inRange(hsvL, lower, upper, maskL);
    inRange(hsvR, lower, upper, maskR);

    GaussianBlur(maskL, imgBlurL, Size(7, 7), 0, 0);
    GaussianBlur(maskR, imgBlurR, Size(7, 7), 0, 0);

    canny_threshold1 = 200; // empirically
    Canny(imgBlurL, imgCannyL, canny_threshold1, 3 * canny_threshold1);
    Mat kernelL = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(imgCannyL, imgDilL, kernelL);

    Canny(imgBlurR, imgCannyR, canny_threshold1, 3 * canny_threshold1);
    Mat kernelR = getStructuringElement(MORPH_RECT, Size(5, 5));
    dilate(imgCannyR, imgDilR, kernelR);

    Point pLeft = getContourOfObj(imgDilL, imgL);
    Point pRight = getContourOfObj(imgDilR, imgR);

    imshow("Image Left", imgL);
    imshow("Image Right", imgR);
    cout << "Left x: " << pLeft.x << " Left y: " << pLeft.y << "\n";
    cout << "Right x: " << pRight.x << " Right y: " << pRight.y << "\n";

    float alpha = 0.0590909; // coefficient to get disparity
    float disparity = pLeft.x - pRight.x;

    float depth = (float) f*b_x /( (float) alpha*disparity);
    cout<<"Depth is: "<<depth;

    // disparity map calculations
    imgL = imread(path1);
    imgR = imread(path2);
    Mat disparityMap, imgGrayL, imgGrayR;
    Ptr<StereoBM> sbm = StereoBM::create( 16*8, 35);
    cvtColor(imgL, imgGrayL, COLOR_BGR2GRAY);
    cvtColor(imgR, imgGrayR, COLOR_BGR2GRAY);

    sbm->compute(imgGrayL, imgGrayR, disparityMap);
    imshow("Disparity", disparityMap);


    waitKey(0);

}

