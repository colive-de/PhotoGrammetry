/*
*这一部分用来实现完整的相关系数匹配算法
* 其中涉及到的函数有：
* double computerelate(const Mat& window1, const Mat& window2) >>> 他的实现在computeCorrelation.cpp
* vector<Point2f> Harris(const Mat &img, int blockSize, int ksize, double k, double threshold) >>> 他的实现在Harris.cpp
* Mat downsampleBilinear(const Mat& img) >>> 双线性插值进行下采样 >>> 他的实现在本cpp文件
* bool isVaildPoint(const Mat& img, Point2f pt, int windowSize) >>> 判断点是否在图像范围内 >>> 他的实现在本cpp文件
* vector<MMatch> correlationMatchingPyramid(const Mat& srcImg, const Mat& dstImg, int numLevels, int windowSize, double threshold)
*/


#include "head.h"
#include "utils.h"
#include <cmath>
#include <opencv2/opencv.hpp>
#include <algorithm>

using namespace std;
using namespace cv;


// 显示并保存匹配点（保持原图尺寸，放大加号与圆点）
void showMatches(const Mat& srcImg, const Mat& dstImg, const vector<MMatch>& matches) {

    Mat leftVis, rightVis;
    cvtColor(srcImg, leftVis, COLOR_GRAY2BGR);
    cvtColor(dstImg, rightVis, COLOR_GRAY2BGR);

    // 标记尺寸放大
    int crossSize = 12;      // 加号尺寸
    int crossThickness = 3;  // 加号线宽
    int circleRadius = 6;    // 圆点半径
    int circleThickness = -1;// 填充

    for (const auto& m : matches) {

        // --- 左影像：黄色加号 ---
        Scalar yellow(0, 255, 255);

        line(leftVis,
            Point(m.srcPt.x - crossSize, m.srcPt.y),
            Point(m.srcPt.x + crossSize, m.srcPt.y),
            yellow, crossThickness);

        line(leftVis,
            Point(m.srcPt.x, m.srcPt.y - crossSize),
            Point(m.srcPt.x, m.srcPt.y + crossSize),
            yellow, crossThickness);

        // --- 右影像：绿色圆点 ---
        Scalar green(0, 255, 0);
        circle(rightVis, m.dstPt, circleRadius, green, circleThickness);
    }

    // --- 按原图分辨率直接保存 ---
    imwrite("matched_left.png", leftVis);
    imwrite("matched_right.png", rightVis);

    cout << "保存完成：matched_left.png, matched_right.png" << endl;
}


vector<MMatch> correlationMatchingPyramidMultiScale(const Mat& srcImg, const Mat& dstImg, int numLevels, int windowSize, double threshold, int upmax) {
    cout << "=== Starting Independent Multi-Level Matching ===" << endl;
    cout << "Image sizes: " << srcImg.size() << " -> " << dstImg.size() << endl;

  

    vector<Mat> srcPyramid = buildGaussianPyramidManual(srcImg, numLevels, 5, 1.0);
    vector<Mat> dstPyramid = buildGaussianPyramidManual(dstImg, numLevels, 5, 1.0);



    int actualLevels = min(srcPyramid.size(), dstPyramid.size());
    cout << "Actual pyramid levels: " << actualLevels << endl;

    vector<MMatch> allMatches;


    // 对每一层独立进行特征提取和匹配
    for (int level = 0; level < actualLevels; ++level) {
        cout << "Processing Level " << level << " - Size: " << srcPyramid[level].size() << endl;

        Mat srcLevel = srcPyramid[level];
        Mat dstLevel = dstPyramid[level];

        // 为每层调整参数
        int levelMaxPoints = upmax / (level + 1);
        double levelThreshold = threshold * (1.0 - 0.1 * level); // 底层使用更低阈值


        // 提取特征点
        vector<Point2f> srcPts = Harris(srcLevel, 3, 5, 0.04, 0.005, levelMaxPoints);
        vector<Point2f> dstPts = Harris(dstLevel, 3, 5, 0.04, 0.005, levelMaxPoints);

       
       

        cout << "Level " << level << " features: " << srcPts.size() << " / " << dstPts.size() << endl;
      
        if (srcPts.empty() || dstPts.empty()) {
            continue;
        }

        // 在当前层进行双向验证匹配
        
        vector<MMatch> levelMatches;
        matchWithVerification(srcLevel, dstLevel, srcPts, dstPts, levelMatches, windowSize, levelThreshold);


        cout << "Level " << level << " matches: " << levelMatches.size() << endl;

        if (levelMatches.empty()) {
            continue;
        }

        // 将匹配点坐标缩放到原始图像尺度
        float scale = pow(2.0f, level);
        for (auto& match : levelMatches) {
            match.srcPt *= scale;
            match.dstPt *= scale;
        }

        // 直接添加到总匹配集
        allMatches.insert(allMatches.end(), levelMatches.begin(), levelMatches.end());
    }



    cout << "Total matches from all levels: " << allMatches.size() << endl;



    if (allMatches.empty()) {
        cout << "No matches found in any level!" << endl;
        return vector<MMatch>();
    }

    // 进行一次简单的去重 - 基于源点位置
    vector<MMatch> uniqueMatches;
    double duplicateThreshold = 8.0; // 适当放宽距离阈值

    for (const auto& match : allMatches) {
        bool isDuplicate = false;

        for (const auto& existing : uniqueMatches) {
            if (norm(match.srcPt - existing.srcPt) < duplicateThreshold) {
                isDuplicate = true;
                break;
            }
        }

        if (!isDuplicate) {
            uniqueMatches.push_back(match);
        }
    }

    cout << "After deduplication: " << uniqueMatches.size() << " matches" << endl;

    int W = srcImg.cols;
    int H = srcImg.rows;

    vector<MMatch> uniformMatches = gridFilterMatches(uniqueMatches, W, H, 20, 20, 2);

    vector<MMatch> finalMatches = ransacFundamentalFilter(uniqueMatches, 2.0, 0.995);

    showMatches(srcImg, dstImg, finalMatches);


    cout << "After gridRANSAC: " << finalMatches.size() << " matches" << endl;

    return finalMatches;
}


