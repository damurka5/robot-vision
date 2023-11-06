#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>
#include "utils/StereoImageProcessing.h"

using namespace cv;
using namespace std;

Mat imgGray, imgBlurL, imgBlurR, imgCannyL, imgCannyR, imgDilL, imgDilR, imgErode;
Mat imgEdged, maskL, maskR;

int canny_threshold1 = 200;


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

    canny_threshold1 = 200; // empirically

    imgDilL = StereoImageProcessing::applyDilation(maskL, imgDilL, canny_threshold1, 3*canny_threshold1, 7,5);
    imgDilR = StereoImageProcessing::applyDilation(maskR, imgDilR, canny_threshold1, 3*canny_threshold1, 7,5);
    Point pLeft = StereoImageProcessing::getContourOfObj(imgDilL, imgL);
    Point pRight = StereoImageProcessing::getContourOfObj(imgDilR, imgR);

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
    Mat different_ChannelsL[3];
    Mat different_ChannelsR[3];
    Ptr<StereoBM> sbm = StereoBM::create( 16*8, 35);
//    cvtColor(imgL, imgGrayL, COLOR_BGR2GRAY);
//    cvtColor(imgR, imgGrayR, COLOR_BGR2GRAY);

    split(imgL, different_ChannelsL);
    split(imgR, different_ChannelsR);

    sbm->compute(different_ChannelsL[1], different_ChannelsR[1], disparityMap);
    imshow("Disparity", disparityMap);

    waitKey(0);
}

