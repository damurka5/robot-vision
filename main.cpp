#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;

Mat imgGray, imgBlur, imgCanny, imgDil, imgErode;
Mat imgEdged, mask;

int canny_threshold1 = 5, dilKernel1 = 1, min = 0;
int canny_threshold2 = 100, blurSize = 3, max = 255;

void getContours(Mat imgDil, Mat img){
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    findContours(imgDil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    drawContours(img, contours, -1, Scalar(255,0, 255), 2);

    for (int i = 0; i < contours.size(); i++)
    {
        int area = contourArea(contours[i]);
        cout<<area<<endl;

        vector<vector<Point>> conPoly(contours.size());
        vector<Rect> boundRect(contours.size());
        string objectType;
        if (area > 1000)
        {
            float peri = arcLength(contours[i], true);
            approxPolyDP(contours[i], conPoly[i], 0.02*peri, true);

            cout<<conPoly[i].size()<<endl;

//            // object detection
            boundRect[i] = boundingRect(conPoly[i]);
//            rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0),5);
            int objCor = (int)conPoly[i].size();

            if(objCor == 3) {objectType = "Tri";}
            if(objCor == 4) {
                float aspRatio = (float)boundRect[i].width/(float)boundRect[i].width;
                cout<< aspRatio << endl;
                if (aspRatio > 0.95 && aspRatio < 1.05){objectType = "Square";}
                else {objectType = "Rect";}
            }
            if(objCor > 4) {objectType = "Object detected";}

            drawContours(img, conPoly, i, Scalar(255,0, 255), 2);
            rectangle(img, boundRect[i].tl(), boundRect[i].br(), Scalar(0,255,0),5);
            putText(img, objectType, {boundRect[i].x, boundRect[i].y - 5}, FONT_HERSHEY_PLAIN, 1, Scalar(0,0, 255), 1);
        }
    }
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

    namedWindow("Trackbars", (640,200));
    createTrackbar("Dilation Kernel", "Trackbars", &dilKernel1, 15);


    while(true) {
        Mat imgL = imread(path1);

        vector<Mat> channels;
        Mat hsv;
        cvtColor(imgL, hsv, COLOR_BGR2HSV);
        // Preprocessing

        Scalar lower(16, 0, 0);
        Scalar upper(175, 255, 255);
        inRange(hsv, lower, upper, mask);

        GaussianBlur(mask, imgBlur, Size(5,5), 0, 0);
//        medianBlur(imgL, imgBlur, 3);
        canny_threshold1 = 200; // empirically
        Canny(imgBlur, imgCanny, canny_threshold1, 3*canny_threshold1);
        Mat kernel = getStructuringElement(MORPH_RECT, Size(dilKernel1, dilKernel1));
        dilate(imgCanny, imgDil, kernel);

        getContours(imgDil, imgL);

        imshow("Image Left", imgL);
        imshow("Image blur", imgDil);

        waitKey(200);
    }
}

