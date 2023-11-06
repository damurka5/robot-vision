#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;

void drawContour(Mat src, Mat points) {

    Point intPoints[4];
    for (int i = 0; i < 4; i++){
        intPoints[i] = Point((int) points.at<float>(i*2), (int) points.at<float>(i*2 + 1));
        cout<<intPoints[i];
    }

    int thickness = 3; 
    auto colour = Scalar(0, 0, 255);

    line(src, intPoints[0],  intPoints[1], colour, thickness);
    line(src, intPoints[1],  intPoints[2], colour, thickness);
    line(src, intPoints[2],  intPoints[3], colour, thickness);
    line(src, intPoints[3],  intPoints[0], colour, thickness);

}

int main() {
    VideoCapture cap(0);
    while (true){
        
        // string path1 = "../resources/qr1.jpg";

        // Mat qr_img = imread(path1);
        Mat qr_img;
        cap.read(qr_img);
        Mat qr_detected;

        auto detector = QRCodeDetector();

        Mat points;
        bool result = detector.detect(qr_img, points);
        cout << "detection_result: " << result << endl;

        String link  = detector.detectAndDecode(qr_img, points, qr_detected);
        // for(int i=0;i<8;++i)
        // {
        //     cout<<(int) points.at<float>(i)<<"\n";
        // }
        if (result>0) drawContour(qr_img, points);
        imshow("QR", qr_img);

        waitKey(1);
    }
}