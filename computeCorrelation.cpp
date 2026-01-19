#include "head.h"
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/core/hal/intrin.hpp> // 引入 OpenCV 通用指令集
using namespace std;
using namespace cv;

/*
* 计算两个小窗口之间的相关性（皮尔逊相关系数）
* @param window1: 第一个图像窗口（单通道）
* @param window2: 第二个图像窗口（单通道）
* @return: 返回两个窗口之间的相关性值（范围 [-1, 1]）
*/
double computerelate(const Mat& window1, const Mat& window2) {


    CV_Assert(window1.size() == window2.size() && window1.type() == window2.type());
    CV_Assert(window1.channels() == 1);

    Mat f1, f2;
    window1.convertTo(f1, CV_32F);
    window2.convertTo(f2, CV_32F);

    int rows = f1.rows;
    int cols = f1.cols;

    double sum1 = 0.0, sum2 = 0.0;
    double sum11 = 0.0, sum22 = 0.0, sum12 = 0.0;

    for (int i = 0; i < rows; ++i) {
        const float* p1 = f1.ptr<float>(i);
        const float* p2 = f2.ptr<float>(i);
        for (int j = 0; j < cols; ++j) {
            float a = p1[j];
            float b = p2[j];
            sum1 += a;
            sum2 += b;
            sum11 += a * a;
            sum22 += b * b;
            sum12 += a * b;
        }
    }

    double N = static_cast<double>(rows * cols);
    double mean1 = sum1 / N;
    double mean2 = sum2 / N;

    double numerator = sum12 - (sum1 * sum2 / N);
    double denominator = sqrt((sum11 - sum1 * mean1) * (sum22 - sum2 * mean2));

    if (denominator < 1e-5) {
        return 0.0; // 避免除以接近零的标准差
    }

    return numerator / denominator;
}



//using namespace cv;
//
//double computerelate(const Mat& window1, const Mat& window2) {
//    CV_Assert(window1.size() == window2.size());
//    CV_Assert(window1.channels() == 1 && window2.channels() == 1);
//
//    int n_pixels = window1.rows * window1.cols;
//
//    // 使用 double 累加器防止大窗口下的溢出，但计算过程尽量用寄存器
//    double sum1 = 0.0, sum2 = 0.0;
//    double sum11 = 0.0, sum22 = 0.0, sum12 = 0.0;
//
//    // 判断内存是否连续，如果连续则压扁为一维数组处理，减少循环开销
//    bool is_continuous = window1.isContinuous() && window2.isContinuous();
//    int rows = is_continuous ? 1 : window1.rows;
//    int cols = is_continuous ? n_pixels : window1.cols;
//
//    // SIMD 步长 (float32 一次处理 4 个 float，即 v_float32::nlanes)
//    // 现代指令集如 AVX2 可能一次处理 8 个
//    int step = v_float32::nlanes;
//
//    for (int i = 0; i < rows; ++i) {
//        const uchar* ptr1_u8 = nullptr;
//        const uchar* ptr2_u8 = nullptr;
//        const float* ptr1_f = nullptr;
//        const float* ptr2_f = nullptr;
//
//        // 获取行指针 (根据类型)
//        if (window1.depth() == CV_8U) {
//            ptr1_u8 = window1.ptr<uchar>(i);
//            ptr2_u8 = window2.ptr<uchar>(i);
//        }
//        else {
//            ptr1_f = window1.ptr<float>(i);
//            ptr2_f = window2.ptr<float>(i);
//        }
//
//        // 初始化 SIMD 累加寄存器 (设置为 0)
//        v_float32 v_sum1 = vx_setzero_f32();
//        v_float32 v_sum2 = vx_setzero_f32();
//        v_float32 v_sum11 = vx_setzero_f32();
//        v_float32 v_sum22 = vx_setzero_f32();
//        v_float32 v_sum12 = vx_setzero_f32();
//
//        int j = 0;
//        // SIMD 主循环
//        for (; j <= cols - step; j += step) {
//            v_float32 val1, val2;
//
//            // 加载数据并转为 float 向量
//            if (window1.depth() == CV_8U) {
//                // 加载 uchar，扩展为 float
//                // 1. 加载 8位 -> 转 16位 -> 转 32位 float (Universal Intrinsics 稍微繁琐但极快)
//                // 这里为了代码简洁，我们做稍微简化的加载（如果是 AVX2 有更快的方法，这里用通用兼容写法）
//
//                // 临时方案：手动加载少量数据转换 (Opencv的 load_expand 比较复杂)
//                // 更直接的方式：加载 float 数组。
//                // 为保证极致性能，通常需要针对 uchar 专门写 intrinsics，
//                // 但这里我们为了兼容性，使用通用方式：
//                float buf1[v_float32::nlanes];
//                float buf2[v_float32::nlanes];
//                for (int k = 0; k < step; ++k) {
//                    buf1[k] = (float)ptr1_u8[j + k];
//                    buf2[k] = (float)ptr2_u8[j + k];
//                }
//                val1 = v_load(buf1);
//                val2 = v_load(buf2);
//            }
//            else {
//                val1 = v_load(ptr1_f + j);
//                val2 = v_load(ptr2_f + j);
//            }
//
//            // 核心计算：乘加运算 (FMA - Fused Multiply Add 如果硬件支持会自动调用)
//            v_sum1 = v_add(v_sum1, val1);             // sum x
//            v_sum2 = v_add(v_sum2, val2);             // sum y
//            v_sum11 = v_muladd(val1, val1, v_sum11);  // sum x*x
//            v_sum22 = v_muladd(val2, val2, v_sum22);  // sum y*y
//            v_sum12 = v_muladd(val1, val2, v_sum12);  // sum x*y
//        }
//
//        // 将 SIMD 寄存器中的 4/8 个分量求和归约为 1 个 double
//        sum1 += v_reduce_sum(v_sum1);
//        sum2 += v_reduce_sum(v_sum2);
//        sum11 += v_reduce_sum(v_sum11);
//        sum22 += v_reduce_sum(v_sum22);
//        sum12 += v_reduce_sum(v_sum12);
//
//        // 处理剩余的尾部像素 (Tail processing)
//        for (; j < cols; ++j) {
//            float a, b;
//            if (window1.depth() == CV_8U) {
//                a = (float)ptr1_u8[j];
//                b = (float)ptr2_u8[j];
//            }
//            else {
//                a = ptr1_f[j];
//                b = ptr2_f[j];
//            }
//            sum1 += a;
//            sum2 += b;
//            sum11 += a * a;
//            sum22 += b * b;
//            sum12 += a * b;
//        }
//    }
//
//    // 最后的数学计算 (保持不变，这部分只有一次运算，不是瓶颈)
//    double N = static_cast<double>(n_pixels);
//    // 使用优化后的公式减少除法次数：
//    // Numerator = N * sumXY - sumX * sumY
//    // Denominator = sqrt((N * sumXX - sumX^2) * (N * sumYY - sumY^2))
//    // 这样可以避免中间除以 N 带来的精度损失和开销，最后结果是一样的
//
//    double num = N * sum12 - sum1 * sum2;
//    double den_sq = (N * sum11 - sum1 * sum1) * (N * sum22 - sum2 * sum2);
//
//    // 只有分母平方大于0才开根号
//    if (den_sq < 1e-10) return 0.0;
//
//    return num / std::sqrt(den_sq);
//}