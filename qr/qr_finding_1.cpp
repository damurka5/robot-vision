#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>

using namespace cv;
using namespace std;

int main() {
    string path1 = "../resources/qr1.jpg";

    Mat qr_img = imread(path1);
    Mat qr_detected;

    auto detector = QRCodeDetector();

    Mat points;
    bool result = detector.detect(qr_img, points);
    cout << "detection_result: " << result << endl;

    String link  = detector.detectAndDecode(qr_img, points, qr_detected);
    cout<<link;
    imshow("QR", qr_detected);

    waitKey(0);
}