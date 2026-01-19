#pragma once
#include <utility> 
#include <vector>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <unordered_set>

using namespace cv;
using namespace std;

struct MMatch {
    Point2f srcPt;
    Point2f dstPt;
    double dist;

    // ÷ÿ‘ÿ == ‘ÀÀ„∑˚
    bool operator==(const MMatch& other) const {
        return (srcPt == other.srcPt && dstPt == other.dstPt && dist == other.dist);
    }
};

vector<Point2f> Harris(const Mat& img, int blockSize, int ksize, double k, double threshold, int maxCorners);

double computerelate(const Mat& window1, const Mat& window2);

vector<MMatch> correlationMatchingPyramid(const Mat& srcImg, const Mat& dstImg, int numLevels, int windowSize, double threshold, int upmax);

vector<MMatch> correlationMatchingPyramidMultiScale(const Mat& srcImg, const Mat& dstImg, int numLevels, int windowSize, double threshold, int upmax);

vector<pair<Point2f, Point2f>> LSM(const Mat& img1, const Mat& img2, vector<MMatch>& initialPairs, int windowSize, int maxIterations, double epsilon, double LSMthres);

Mat gaussian_filter(int size, double sigma);

vector<MMatch> ransacFundamentalFilter(const vector<MMatch>& matches, double ransacReprojThreshold, double confidence);

vector<MMatch> gridFilterMatches(const vector<MMatch>& matches, int imgWidth, int imgHeight, int gridRows, int gridCols, int perCellMax);

vector<Mat> buildGaussianPyramidManual(const Mat& img, int numLevels, int kernelSize, double sigma);

void matchWithVerification(const Mat& src, const Mat& dst, const vector<Point2f>& srcPts, const vector<Point2f>& dstPts, vector<MMatch>& matches, int windowSize, double threshold);