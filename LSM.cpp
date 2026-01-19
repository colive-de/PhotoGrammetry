/*
*这个函数进行最小二乘匹配优化在相关系数匹配时的粗匹配点。
*函数名>>>vector<pair<Point2f, Point2f>> LSM(const Mat& img1, const Mat& img2, const vector<MMatch>& initialPairs, int windowSize, int maxIterations, double epsilon);
* @param img1: 源图像
* @param img2: 目标图像
* @param initialPairs: 初始匹配点对
* @param windowSize: 用于计算梯度和误差的窗口大小
* @param maxIterations: 最大迭代次数
* @param epsilon: 收敛阈值
*/

#include "head.h"
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

static inline double bilinearInterpolate(const Mat& img, double x, double y)
{
    int ix = floor(x);
    int iy = floor(y);
    double dx = x - ix;
    double dy = y - iy;

    if (ix < 0 || iy < 0 || ix + 1 >= img.cols || iy + 1 >= img.rows)
        return 0.0;

    double v00 = img.at<uchar>(iy, ix);
    double v10 = img.at<uchar>(iy, ix + 1);
    double v01 = img.at<uchar>(iy + 1, ix);
    double v11 = img.at<uchar>(iy + 1, ix + 1);

    return (1 - dx) * (1 - dy) * v00 + dx * (1 - dy) * v10 + (1 - dx) * dy * v01 + dx * dy * v11;
}

vector<pair<Point2f, Point2f>> LSM(const Mat& img1, const Mat& img2, vector<MMatch>& initialPairs, int windowSize, int maxIterations, double epsilon, double LSMthres) {
    CV_Assert(img1.channels() == 1 && img2.channels() == 1);
    CV_Assert(windowSize % 2 == 1);

    int r = windowSize / 2;
    vector<pair<Point2f, Point2f>> resPairs;

    for (const auto& m : initialPairs) {
        double x1 = m.srcPt.x;
        double y1 = m.srcPt.y;
        double x2 = m.dstPt.x;
        double y2 = m.dstPt.y;

        // 边界检查
        if (x1 - r < 0 || y1 - r < 0 || x2 - r < 0 || y2 - r < 0 ||
            x1 + r >= img1.cols - 1 || y1 + r >= img1.rows - 1 ||
            x2 + r >= img2.cols - 1 || y2 + r >= img2.rows - 1)
            continue;

        // 提取源图像窗口
        Mat win1 = img1(Rect(cvRound(x1 - r), cvRound(y1 - r), windowSize, windowSize)).clone();
        win1.convertTo(win1, CV_32F);

        // ========== 在迭代前计算梯度平方权重 ==========
        double xNumerator = 0.0, xDenominator = 0.0;
        double yNumerator = 0.0, yDenominator = 0.0;

        for (int i = -r; i <= r; i++) {
            for (int j = -r; j <= r; j++) {
                double col_src = x1 + j;
                double row_src = y1 + i;

                // 计算左影像梯度（固定不变）
                double gxsrc = (bilinearInterpolate(img1, col_src + 1, row_src) - bilinearInterpolate(img1, col_src - 1, row_src)) * 0.5;
                double gysrc = (bilinearInterpolate(img1, col_src, row_src + 1) - bilinearInterpolate(img1, col_src, row_src - 1)) * 0.5;

                // 使用梯度平方作为权重
                xNumerator += j * gxsrc * gxsrc;  // j是列偏移，对应x方向
                xDenominator += gxsrc * gxsrc;
                yNumerator += i * gysrc * gysrc;  // i是行偏移，对应y方向
                yDenominator += gysrc * gysrc;
            }
        }

        // 计算加权平均的坐标偏移（相对于窗口中心）
        double xx1 = (xDenominator > 1e-8) ? xNumerator / xDenominator : 0;
        double yy1 = (yDenominator > 1e-8) ? yNumerator / yDenominator : 0;

        // 初始化变换参数
        double a0 = 0, a1 = 1.0, a2 = 0.0;
        double b0 = 0, b1 = 0.0, b2 = 1.0;
        double h0 = 0.0, h1 = 1.0;

        double currentCorrelationIdx = 0.0, bestCorrelationIdx = -1.0;
        double prevCorrelationIdx = -1.0;
        Point2d bestPt(x2, y2);
        bool converged = false;
        int noImprovementCount = 0;

        // 迭代优化
        for (int iter = 0; iter < maxIterations && !converged; iter++) {
            vector<vector<double>> A;
            vector<double> L;
            Mat currentWin2 = Mat::zeros(windowSize, windowSize, CV_32F);

            for (int i = -r; i <= r; i++) {
                for (int j = -r; j <= r; j++) {
                    double col_src = x1 + j;
                    double row_src = y1 + i;

                    // 计算变换后的坐标（使用原始窗口坐标）
                    double col_dst = a0 + a1 * j + a2 * i;
                    double row_dst = b0 + b1 * j + b2 * i;

                    // 转换为绝对坐标
                    col_dst += x2;
                    row_dst += y2;

                    if (col_dst < 1 || row_dst < 1 || col_dst >= img2.cols - 1 || row_dst >= img2.rows - 1)
                        continue;

                    // 计算目标图像值和梯度
                    double g2 = bilinearInterpolate(img2, col_dst, row_dst);
                    double reg2 = h0 + h1 * g2;
                    currentWin2.at<float>(i + r, j + r) = static_cast<float>(reg2);

                    double g1 = win1.at<float>(i + r, j + r);

                    // 计算梯度
                    double gx = (bilinearInterpolate(img2, col_dst + 1, row_dst) - bilinearInterpolate(img2, col_dst - 1, row_dst)) * 0.5;
                    double gy = (bilinearInterpolate(img2, col_dst, row_dst + 1) - bilinearInterpolate(img2, col_dst, row_dst - 1)) * 0.5;

                    // 构建误差方程系数
                    vector<double> rowA = {
                          1.0,            // dh0
                          g2,             // dh1
                          gx * h1,        // da0
                          j * gx * h1,    // da1
                          i * gx * h1,    // da2
                          gy * h1,        // db0
                          j * gy * h1,    // db1
                          i * gy * h1     // db2
                    };

                    A.push_back(rowA);
                    L.push_back(g1 - reg2);
                }
            }

            if (A.size() < 8) continue;

            // 计算当前相关系数
            currentCorrelationIdx = computerelate(win1, currentWin2);

            // 改进的收敛判断
            if (iter > 0) {
                double correlationChange = currentCorrelationIdx - prevCorrelationIdx;

                if (correlationChange < 0) {
                    noImprovementCount++;
                }
                else {
                    noImprovementCount = 0;
                }

                if (noImprovementCount >= 3 || abs(correlationChange) < 0.001) {
                    converged = true;
                }
            }
            prevCorrelationIdx = currentCorrelationIdx;

            // 构建和求解法方程
            Mat AtA = Mat::zeros(8, 8, CV_64F);
            Mat AtL = Mat::zeros(8, 1, CV_64F);

            for (size_t idx = 0; idx < A.size(); idx++) {
                for (int row_idx = 0; row_idx < 8; row_idx++) {
                    for (int col_idx = 0; col_idx < 8; col_idx++) {
                        AtA.at<double>(row_idx, col_idx) += A[idx][row_idx] * A[idx][col_idx];
                    }
                    AtL.at<double>(row_idx, 0) += A[idx][row_idx] * L[idx];
                }
            }

            Mat dx;
            if (!solve(AtA, AtL, dx, DECOMP_SVD)) break;

            // 保存旧参数
            double a0_old = a0;
            double a1_old = a1;
            double a2_old = a2;
            double b0_old = b0;
            double b1_old = b1;
            double b2_old = b2;
            double h0_old = h0;
            double h1_old = h1;

            // 使用改进的参数更新方式
            a0 = a0_old + dx.at<double>(2, 0) + a0_old * dx.at<double>(3, 0) + b0_old * dx.at<double>(4, 0);
            a1 = a1_old + a1_old * dx.at<double>(3, 0) + b1_old * dx.at<double>(4, 0);
            a2 = a2_old + a2_old * dx.at<double>(3, 0) + b2_old * dx.at<double>(4, 0);
            b0 = b0_old + dx.at<double>(5, 0) + a0_old * dx.at<double>(6, 0) + b0_old * dx.at<double>(7, 0);
            b1 = b1_old + a1_old * dx.at<double>(6, 0) + b1_old * dx.at<double>(7, 0);
            b2 = b2_old + a2_old * dx.at<double>(6, 0) + b2_old * dx.at<double>(7, 0);
            h0 = h0_old + dx.at<double>(0, 0) + h0_old * dx.at<double>(1, 0);
            h1 = h1_old + h1_old * dx.at<double>(1, 0);

            // ========== 使用固定的加权中心更新最佳结果 ==========
            if (currentCorrelationIdx > bestCorrelationIdx) {
                bestCorrelationIdx = currentCorrelationIdx;
                // 使用变换参数将左影像的加权中心映射到右影像
                bestPt.x = x2 + a0 + xx1 * a1 + yy1 * a2;
                bestPt.y = y2 + b0 + xx1 * b1 + yy1 * b2;
            }

            // 收敛判断
            double paramChange = norm(dx);
            if (paramChange < epsilon || currentCorrelationIdx > 0.99) {
                converged = true;
            }
        }

        // 只保存相关系数足够好的结果
        if (bestCorrelationIdx > LSMthres) {
            resPairs.emplace_back(Point2f(x1 + xx1, y1 + yy1), Point2f(bestPt.x, bestPt.y));
        }
    }

    return resPairs;
}

