#pragma once
#include <utility> 
#include <vector>
#include <cstring>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat downsampleBilinear(const Mat& img);

bool isValidPoint(const Mat& img, Point2f pt, int windowSize);