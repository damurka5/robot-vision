//
// Created by Damir Nurtdinov on 10/21/23.
//
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <opencv2/calib3d.hpp>

#ifndef ROBOT_VISION_STEREOIMAGEPROCESSING_H
#define ROBOT_VISION_STEREOIMAGEPROCESSING_H


class StereoImageProcessing {
public:
    static bool compareContourAreas(std::vector<cv::Point> contour1, std::vector<cv::Point> contour2);
    static cv::Point getContourOfObj(cv::Mat imgDil, cv::Mat img);
};


#endif //ROBOT_VISION_STEREOIMAGEPROCESSING_H
