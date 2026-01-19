#include "head.h"
#include "utils.h"

vector<MMatch> ransacFundamentalFilter(const vector<MMatch>& matches, double ransacReprojThreshold = 3.0, double confidence = 0.99)
{
    if (matches.size() < 8) return {};

    vector<Point2f> pts1, pts2;
    pts1.reserve(matches.size());
    pts2.reserve(matches.size());

    for (const auto& m : matches) {
        pts1.push_back(m.srcPt);
        pts2.push_back(m.dstPt);
    }

    // 掩码
    vector<uchar> inlierMask;

    // 计算基础矩阵 F
    Mat F = findFundamentalMat(pts1, pts2, RANSAC, ransacReprojThreshold, confidence, inlierMask);

    if (F.empty()) {
        return {};
    }

    // 只保留 inlier
    vector<MMatch> inliers;
    inliers.reserve(matches.size());

    for (size_t i = 0; i < matches.size(); ++i) {
        if (inlierMask[i]) {
            inliers.push_back(matches[i]);
        }
    }

    return inliers;
}


vector<MMatch> gridFilterMatches(const vector<MMatch>& matches, int imgWidth, int imgHeight, int gridRows = 20, int gridCols = 20, int perCellMax = 2) {
    // 1. 建立网格：每格一个 vector<MMatch>
    vector<vector<vector<MMatch>>> grid(
        gridRows,
        vector<vector<MMatch>>(gridCols)
    );

    float cellW = static_cast<float>(imgWidth) / gridCols;
    float cellH = static_cast<float>(imgHeight) / gridRows;

    // 2. 将匹配点根据 srcPt 落入对应网格
    for (const auto& m : matches) {
        int cx = min((int)(m.srcPt.x / cellW), gridCols - 1);
        int cy = min((int)(m.srcPt.y / cellH), gridRows - 1);
        grid[cy][cx].push_back(m);
    }

    vector<MMatch> result;
    result.reserve(matches.size());

    // 3. 每格最多保留 perCellMax 个
    for (int r = 0; r < gridRows; r++) {
        for (int c = 0; c < gridCols; c++) {
            auto& cell = grid[r][c];
            if (cell.empty()) continue;

            int take = min((int)cell.size(), perCellMax);
            for (int i = 0; i < take; i++) {
                result.push_back(cell[i]);
            }
        }
    }

    return result;
}


// 双线性插值下采样
Mat downsampleBilinear(const Mat& img) {
    CV_Assert(!img.empty());
    CV_Assert(img.channels() == 1);

    // 使用更精确的尺寸计算
    int newRows = (img.rows + 1) / 2;
    int newCols = (img.cols + 1) / 2;
    Mat result(newRows, newCols, CV_8UC1);

    for (int i = 0; i < newRows; i++) {
        uchar* dst_ptr = result.ptr<uchar>(i);
        for (int j = 0; j < newCols; j++) {
            double srcY = i * 2.0;
            double srcX = j * 2.0;

            // 边界安全计算
            int x0 = static_cast<int>(srcX);
            int y0 = static_cast<int>(srcY);
            int x1 = min(x0 + 1, img.cols - 1);
            int y1 = min(y0 + 1, img.rows - 1);

            // 确保不越界
            x0 = max(0, x0);
            y0 = max(0, y0);

            double dx = srcX - x0;
            double dy = srcY - y0;

            // 双线性插值
            double top = (1.0 - dx) * img.at<uchar>(y0, x0) + dx * img.at<uchar>(y0, x1);
            double bottom = (1.0 - dx) * img.at<uchar>(y1, x0) + dx * img.at<uchar>(y1, x1);
            double value = (1.0 - dy) * top + dy * bottom;

            dst_ptr[j] = static_cast<uchar>(std::clamp(value, 0.0, 255.0));
        }
    }

    return result;
}


// 判断点是否在图像范围内
bool isValidPoint(const Mat& img, Point2f pt, int windowSize) {
    int r = windowSize / 2;
    return (pt.x >= r && pt.x < img.cols - r &&
        pt.y >= r && pt.y < img.rows - r);
}


vector<Mat> buildGaussianPyramidManual(const Mat& img, int numLevels, int kernelSize, double sigma )
{
    vector<Mat> pyramid;
    pyramid.reserve(numLevels);
    pyramid.push_back(img.clone());

    for (int i = 1; i < numLevels; ++i) {
        Mat prev = pyramid.back();

        // 图像太小就停止
        if (prev.rows <= 8 || prev.cols <= 8)
            break;

        // ① 生成高斯核
        Mat kernel = gaussian_filter(kernelSize, sigma);

        // ② 高斯模糊
        Mat blurred;
        filter2D(prev, blurred, -1, kernel, Point(-1, -1), 0, BORDER_DEFAULT);

        // ③ 下采样
        Mat smaller = downsampleBilinear(blurred);

        pyramid.push_back(smaller);
    }

    return pyramid;
}


// 双向验证匹配
void matchWithVerification(
    const Mat& src, const Mat& dst,
    const vector<Point2f>& srcPts, const vector<Point2f>& dstPts,
    vector<MMatch>& matches, int windowSize, double threshold)
{
    matches.clear();
    int r = windowSize / 2;

    int Nsrc = srcPts.size();
    int Ndst = dstPts.size();

    cout << "Matching: " << Nsrc << " source points vs " << Ndst << " target points" << endl;

    // -----------------------------------------------------------
    // 1. 预先缓存所有窗口（避免重复裁剪 ROI，巨大加速）
    // -----------------------------------------------------------
    vector<Mat> srcWins(Nsrc);
    vector<Mat> dstWins(Ndst);

    for (int i = 0; i < Nsrc; i++) {
        if (isValidPoint(src, srcPts[i], windowSize)) {
            Rect rect(srcPts[i].x - r, srcPts[i].y - r, windowSize, windowSize);
            srcWins[i] = src(rect).clone();  // clone 保证生命周期
        }
    }
    for (int j = 0; j < Ndst; j++) {
        if (isValidPoint(dst, dstPts[j], windowSize)) {
            Rect rect(dstPts[j].x - r, dstPts[j].y - r, windowSize, windowSize);
            dstWins[j] = dst(rect).clone();
        }
    }

    // -----------------------------------------------------------
    // 2. 正向匹配（完全按你的逻辑）
    // -----------------------------------------------------------
    vector<MMatch> forwardMatches;

    for (int i = 0; i < Nsrc; i++) {
        if (srcWins[i].empty()) continue;

        const Mat& win1 = srcWins[i];

        double bestScore = -1.0;
        Point2f bestDstPt;
        bool found = false;

        for (int j = 0; j < Ndst; j++) {
            if (dstWins[j].empty()) continue;

            double score = computerelate(win1, dstWins[j]);

            if (score > bestScore && score >= threshold) {
                bestScore = score;
                bestDstPt = dstPts[j];
                found = true;
            }
        }

        if (found) {
            MMatch m;
            m.srcPt = srcPts[i];
            m.dstPt = bestDstPt;
            m.dist = bestScore;
            forwardMatches.push_back(m);
        }
    }

    cout << "Forward matches: " << forwardMatches.size() << endl;

    // -----------------------------------------------------------
    // 3. 双向验证（同样用缓存窗口）
    // -----------------------------------------------------------
    for (const auto& fm : forwardMatches) {
        // 找到 dstPt 对应的下标 j
        int j = -1;
        for (int t = 0; t < Ndst; t++) {
            if (dstPts[t] == fm.dstPt) { j = t; break; }
        }
        if (j == -1 || dstWins[j].empty()) continue;

        const Mat& win2 = dstWins[j];

        double bestRevScore = -1.0;
        Point2f bestSrcPt;

        for (int i = 0; i < Nsrc; i++) {
            if (srcWins[i].empty()) continue;

            double score = computerelate(win2, srcWins[i]);

            if (score > bestRevScore) {
                bestRevScore = score;
                bestSrcPt = srcPts[i];
            }
        }

        if (bestRevScore >= threshold && norm(fm.srcPt - bestSrcPt) < 2.0)
            matches.push_back(fm);
    }

    cout << "After bidirectional verification: " << matches.size() << endl;
}