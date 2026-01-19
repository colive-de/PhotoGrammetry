#include "head.h"
#include <cmath>


Mat conv2d(const Mat& image, const Mat& kernel) {
    CV_Assert(image.channels() == 1 && kernel.channels() == 1);
    
    Mat result;
    filter2D(image, result, CV_32F, kernel, Point(-1,-1), 0, BORDER_REFLECT);
    return result;
}


Mat gaussian_filter(int size, double sigma) {
    int half = size / 2;
    Mat kernel(size, size, CV_32F);
    double sum = 0.0;

    for (int i = -half; i <= half; ++i) {
        for (int j = -half; j <= half; ++j) {
            double value = exp(-(i * i + j * j) / (2 * sigma * sigma));
            kernel.at<float>(i + half, j + half) = (float)value;
            sum += value;
        }
    }
    kernel /= sum;
    return kernel;
}



/**
 * @param img 输入灰度或彩色图像
 * @param blockSize 窗口大小（用于求取 M 矩阵时的加权窗口，如 3 或 5）
 * @param ksize Sobel算子大小（用于计算梯度）
 * @param k Harris 响应参数（典型取 0.04 ~ 0.06）
 * @param threshold 阈值（筛选角点）
 * @return 返回检测到的角点 (x=列, y=行)
 */

vector<Point2f> Harris(const Mat& img, int blockSize, int ksize, double k, double threshold, int maxCorners) {
    Mat gray;
    if (img.channels() == 3) {
        cvtColor(img, gray, COLOR_BGR2GRAY);
    }
    else {
        gray = img.clone();
    }
    gray.convertTo(gray, CV_32F);

    // 1. Sobel参数
    Mat Ix, Iy;
    int sobelSize = (ksize % 2 == 0) ? ksize + 1 : ksize; // 确保奇数
    sobelSize = max(3, min(7, sobelSize)); // 限制在合理范围
    Sobel(gray, Ix, CV_32F, 1, 0, sobelSize);
    Sobel(gray, Iy, CV_32F, 0, 1, sobelSize);

    // 2. 改进的高斯滤波参数
    double sigma = max(1.0, blockSize / 6.0); // 动态调整sigma
    Mat gauss = gaussian_filter(blockSize, sigma);

    // 3. 计算Harris矩阵分量
    Mat Ix2, Iy2, Ixy;
    multiply(Ix, Ix, Ix2);
    multiply(Iy, Iy, Iy2);
    multiply(Ix, Iy, Ixy);

    // 应用高斯滤波
    Mat Sx2, Sy2, Sxy;
    filter2D(Ix2, Sx2, CV_32F, gauss);
    filter2D(Iy2, Sy2, CV_32F, gauss);
    filter2D(Ixy, Sxy, CV_32F, gauss);

    // 4. 计算Harris响应
    Mat R = Mat::zeros(img.size(), CV_32F);
    for (int i = 0; i < img.rows; ++i) {
        float* pSx2 = Sx2.ptr<float>(i);
        float* pSy2 = Sy2.ptr<float>(i);
        float* pSxy = Sxy.ptr<float>(i);
        float* pR = R.ptr<float>(i);
        for (int j = 0; j < img.cols; ++j) {
            float a = pSx2[j];
            float b = pSxy[j];
            float c = pSy2[j];
            float det = a * c - b * b;
            float trace = a + c;

            // 添加小的epsilon避免除零
            if (trace > 1e-6) {
                pR[j] = det / (trace + 1e-6) - k; // 使用归一化版本
            }
            else {
                pR[j] = 0;
            }
        }
    }

    // 5. 改进的非极大值抑制
    int nmsSize = max(3, blockSize / 2);
    Mat kernel = getStructuringElement(MORPH_RECT, Size(nmsSize, nmsSize));
    Mat R_dilated;
    dilate(R, R_dilated, kernel);

    Mat R_nms = Mat::zeros(R.size(), CV_32F);
    R.copyTo(R_nms, R == R_dilated);

    // 6. 改进的阈值设置
    double Rmin, Rmax;
    minMaxLoc(R_nms, &Rmin, &Rmax);

    // 使用绝对阈值和相对阈值的组合
    double absThreshold = 0.01; // 绝对阈值
    double relThreshold = threshold; // 相对阈值
    double Th = max(absThreshold, relThreshold * Rmax);

    // 7. 收集角点
    struct Corner {
        Point2f pt;
        float response;
    };

    vector<Corner> corners;
    for (int i = 0; i < R_nms.rows; ++i) {
        const float* row = R_nms.ptr<float>(i);
        for (int j = 0; j < R_nms.cols; ++j) {
            if (row[j] > Th) {
                corners.push_back({ Point2f((float)j, (float)i), row[j] });
            }
        }
    }

    cout << "Harris found " << corners.size() << " corners before filtering" << endl;

    // 8. 按响应值排序
    sort(corners.begin(), corners.end(), [](const Corner& a, const Corner& b) {
        return a.response > b.response;
        });

    // 9. 限制数量并确保空间分布
    vector<Point2f> result;
    if (corners.size() > (size_t)maxCorners) {
        corners.resize(maxCorners);
    }

    // 进一步的空间分布过滤
    double minDistance = 10.0; // 最小像素距离
    for (size_t i = 0; i < corners.size(); ++i) {
        bool tooClose = false;
        for (size_t j = 0; j < result.size(); ++j) {
            if (norm(corners[i].pt - result[j]) < minDistance) {
                tooClose = true;
                break;
            }
        }
        if (!tooClose) {
            result.push_back(corners[i].pt);
        }
    }

    cout << "Harris final corners: " << result.size() << endl;
    return result;
}