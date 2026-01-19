#include "Match.h"
#include "ui_Match.h" 
#include <iostream>
#include "head.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <chrono>
#include <opencv2/opencv.hpp>



using namespace std;
using namespace cv;


Match::Match(QWidget* parent): QMainWindow(parent), ui(new Ui::MatchClass){
    ui->setupUi(this);

    // ========== 设置epsilon的精度 ==========
    ui->epsilon->setDecimals(6);  // 

    // 初始化导出相关控件 - 默认隐藏，直到用户勾选导出
    ui->txtStyle->setChecked(true); // 默认选择TXT格式
    ui->csvStyle->setChecked(false);

    // 初始状态下禁用导出相关控件
    ui->labelExportStyle->setEnabled(false);
    ui->txtStyle->setEnabled(false);
    ui->csvStyle->setEnabled(false);
    ui->SelectOutPath->setEnabled(false);
    ui->ExportPath->setEnabled(false);

    // 设置路径文本框的提示文字
    ui->ExportPath->setPlaceholderText("Check to select export path...");
}

Match::~Match()
{
    delete ui;
}



// --- 左影像选择 ---
void Match::on_selectLeftImg_clicked()
{
    leftImgPath = QFileDialog::getOpenFileName(this, "Select Left Image", "", "Images (*.png *.jpg *.bmp)");
    if (!leftImgPath.isEmpty()) {
        ui->lineEditImgL->setText(leftImgPath);
    }
}

// --- 右影像选择 ---
void Match::on_selectRightImg_clicked()
{
    rightImgPath = QFileDialog::getOpenFileName(this, "Select Right Image", "", "Images (*.png *.jpg *.bmp)");
    if (!rightImgPath.isEmpty()) {
        ui->lineEditImgR->setText(rightImgPath);
    }
}


// --- 导出同名点相关功能 ---

// 导出复选框状态改变
void Match::on_ExportPoints_stateChanged(int state)
{
    bool enabled = (state == Qt::Checked);

    // 启用/禁用相关控件
    ui->labelExportStyle->setEnabled(enabled);
    ui->txtStyle->setEnabled(enabled);
    ui->csvStyle->setEnabled(enabled);
    ui->SelectOutPath->setEnabled(enabled);
    ui->ExportPath->setEnabled(enabled);

    if (enabled) {
        // 自动弹出路径选择对话框
        QString defaultName = generateDefaultExportName();

        QString fileName = QFileDialog::getSaveFileName(this,"Save Control Points File",defaultName,getFileFilter());

        if (!fileName.isEmpty()) {
            exportPath = fileName;

            // 在文本框中显示路径（缩短长路径）
            QString displayPath = shortenPathForDisplay(fileName);
            ui->ExportPath->setText(displayPath);
            ui->ExportPath->setToolTip(fileName); // 悬停显示完整路径

            // 根据文件后缀更新格式选择
            updateFormatFromExtension(fileName);
        }
        else {
            // 用户取消选择，取消勾选
            ui->ExportPoints->setChecked(false);
            // 重新禁用相关控件
            ui->labelExportStyle->setEnabled(false);
            ui->txtStyle->setEnabled(false);
            ui->csvStyle->setEnabled(false);
            ui->SelectOutPath->setEnabled(false);
            ui->ExportPath->setEnabled(false);
        }
    }
    else {
        // 取消勾选时清空路径
        exportPath.clear();
        ui->ExportPath->clear();
        ui->ExportPath->setToolTip("");
    }
}


// 格式选择：TXT
void Match::on_txtStyle_clicked()
{
    if (ui->txtStyle->isChecked()) {
        // 如果已经有路径，更新文件后缀
        if (!exportPath.isEmpty()) {
            updateExportPathExtension("txt");
        }
    }
}

// 格式选择：CSV
void Match::on_csvStyle_clicked()
{
    if (ui->csvStyle->isChecked()) {
        // 如果已经有路径，更新文件后缀
        if (!exportPath.isEmpty()) {
            updateExportPathExtension("csv");
        }
    }
}

// 手动选择路径按钮
void Match::on_SelectOutPath_clicked()
{
    QString defaultName = generateDefaultExportName();

    QString fileName = QFileDialog::getSaveFileName(this,"Select Output Path",defaultName,getFileFilter());

    if (!fileName.isEmpty()) {
        exportPath = fileName;

        // 显示路径
        QString displayPath = shortenPathForDisplay(fileName);
        ui->ExportPath->setText(displayPath);
        ui->ExportPath->setToolTip(fileName);

        // 更新格式选择
        updateFormatFromExtension(fileName);
    }
}

// 生成默认导出文件名
QString Match::generateDefaultExportName()
{
    QFileInfo leftInfo(leftImgPath);
    QFileInfo rightInfo(rightImgPath);

    QString baseName = "control_points";

    // 如果左右图像都有路径，用它们的基名组合
    if (!leftImgPath.isEmpty() && !rightImgPath.isEmpty()) {
        baseName = leftInfo.baseName() + "_" + rightInfo.baseName() + "_points";
    }
    // 如果只有左图像有路径
    else if (!leftImgPath.isEmpty()) {
        baseName = leftInfo.baseName() + "_points";
    }

    // 添加时间戳避免重复
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString extension = getCurrentExtension();

    // 获取当前工作目录或图片所在目录
    QString baseDir = QDir::currentPath();
    if (!leftImgPath.isEmpty()) {
        baseDir = leftInfo.absolutePath();
    }

    return QString("%1/%2_%3.%4").arg(baseDir).arg(baseName).arg(timestamp).arg(extension);
}

// 获取当前选择的文件格式
QString Match::getCurrentExtension()
{
    if (ui->csvStyle->isChecked()) {
        return "csv";
    }
    else {
        return "txt"; // 默认TXT
    }
}

// 获取文件过滤器
QString Match::getFileFilter()
{
    if (ui->csvStyle->isChecked()) {
        return "CSV Files (*.csv)";
    }
    else {
        return "Text Files (*.txt)";
    }
}

// 缩短路径显示
QString Match::shortenPathForDisplay(const QString& fullPath)
{
    if (fullPath.length() <= 50) return fullPath;

    QFileInfo info(fullPath);
    QString fileName = info.fileName();
    QString path = info.absolutePath();

    // 如果文件名本身就很长
    if (fileName.length() > 35) {
        return "..." + fileName.right(32);
    }

    // 缩短路径部分
    int remaining = 50 - fileName.length() - 4; // 4个字符用于".../"
    if (remaining < 5) return "..." + fileName;

    return "..." + path.right(remaining) + "/" + fileName;
}

// 根据文件后缀更新格式选择
void Match::updateFormatFromExtension(const QString& filePath)
{
    QFileInfo info(filePath);
    QString ext = info.suffix().toLower();

    if (ext == "csv") {
        ui->csvStyle->setChecked(true);
        ui->txtStyle->setChecked(false);
    }
    else {
        ui->txtStyle->setChecked(true);
        ui->csvStyle->setChecked(false);
    }
}

// 更新导出路径的文件后缀
void Match::updateExportPathExtension(const QString& extension)
{
    if (exportPath.isEmpty()) return;

    QFileInfo info(exportPath);
    QString newPath = info.absolutePath() + "/" + info.completeBaseName() + "." + extension;

    exportPath = newPath;

    QString displayPath = shortenPathForDisplay(newPath);
    ui->ExportPath->setText(displayPath);
    ui->ExportPath->setToolTip(newPath);
}

// 导出同名点函数
void Match::exportControlPoints(const std::vector<std::pair<Point2f, Point2f>>& points)
{
    if (exportPath.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select output file path first");
        return;
    }

    QFile file(exportPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Cannot create output file");
        return;
    }

    QTextStream out(&file);
    out.setRealNumberPrecision(8);  // 设置精度

    QFileInfo fileInfo(exportPath);
    QString suffix = fileInfo.suffix().toLower();

    if (suffix == "csv") {
        // CSV格式
        out << "Left_X,Left_Y,Right_X,Right_Y\n";
        for (const auto& pointPair : points) {
            out << QString::number(pointPair.first.x, 'f', 6) << ","
                << QString::number(pointPair.first.y, 'f', 6) << ","
                << QString::number(pointPair.second.x, 'f', 6) << ","
                << QString::number(pointPair.second.y, 'f', 6) << "\n";
        }
    }
    else {
        // 默认TXT格式
        out << "# Control Points File\n";
        out << "# Left Image: " << QFileInfo(leftImgPath).fileName() << "\n";
        out << "# Right Image: " << QFileInfo(rightImgPath).fileName() << "\n";
        out << "# Format: Left_X Left_Y Right_X Right_Y\n";
        out << "# Total Points: " << points.size() << "\n";
        out << "# Generated Time: " << QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") << "\n\n";

        for (const auto& pointPair : points) {
            out << QString::number(pointPair.first.x, 'f', 6) << " "
                << QString::number(pointPair.first.y, 'f', 6) << " "
                << QString::number(pointPair.second.x, 'f', 6) << " "
                << QString::number(pointPair.second.y, 'f', 6) << "\n";
        }
    }

    file.close();

    QMessageBox::information(this, "Export Success",QString("Control points exported to:\n%1\nTotal %2 points").arg(exportPath).arg(points.size()));
}





// --- 运行匹配 ---
void Match::on_run_clicked()
{

    auto startTime = std::chrono::high_resolution_clock::now();  // 计时开始

    if (leftImgPath.isEmpty() || rightImgPath.isEmpty()) {
        QMessageBox::warning(this, "Instruction", "Please first select the left and right images");
        return;
    }

    // 读取彩色图像
    Mat colorImgL = imread(leftImgPath.toStdString(), IMREAD_COLOR);
    Mat colorImgR = imread(rightImgPath.toStdString(), IMREAD_COLOR);

    if (colorImgL.empty() || colorImgR.empty()) {
        QMessageBox::critical(this, "Error", "Failed to read the image, Please not include Chinese!!!!");
		cout << "请注意路径是否有中文字符！！！" << endl;
        return;
    }

    // 转换为灰度图用于算法处理
    Mat imgL, imgR;
    if (colorImgL.channels() == 3)
        cvtColor(colorImgL, imgL, COLOR_BGR2GRAY);
    else
        imgL = colorImgL.clone();

    if (colorImgR.channels() == 3)
        cvtColor(colorImgR, imgR, COLOR_BGR2GRAY);
    else
        imgR = colorImgR.clone();

  

    // ---- 读取界面参数 ----
    int numLevels = ui->PyramidLevel->currentText().toInt();
    int relateWin = ui->relateWinSize->currentText().toInt();
    double relateThres = ui->relateThreshold->value();
    int upmax = ui->upmaxHarris->value();

    int LSMwin = ui->LSMrelateWinSize->currentText().toInt();
    int maxIter = ui->maxIterations->value();
    double LSMthres = ui->LSMrelateThreshold->value();
    double eps = ui->epsilon->value();

  

    vector<MMatch> coarseMatches = correlationMatchingPyramidMultiScale(imgL, imgR, numLevels, relateWin, relateThres, upmax);


    // 处理 UI 事件，防止卡死
    QApplication::processEvents();

    if (coarseMatches.empty()) {
        QMessageBox::warning(this, "Warning", "No initial matches found");
        return;
    }

 

    vector<pair<Point2f, Point2f>> refined = LSM(imgL, imgR, coarseMatches, LSMwin, maxIter, eps, LSMthres);

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono::duration<double>(endTime - startTime).count();

	cout << "==================================时间输出==================================" << endl << endl << endl;
	cout << "Total matching time: " << elapsedTime << " seconds." << endl;
    cout << endl << endl << endl<< "==================================输出完毕=================================="<<endl ;


    cout << "==================================点数输出==================================" << endl << endl << endl;
	cout << "Total refined matches: " << refined.size() << endl;
	cout << endl << endl << endl << "==================================输出完毕==================================" << endl;

    if (refined.empty()) {
        QMessageBox::warning(this, "Warning", "No refined matches found");
        return;
    }

    // ========== 同名点导出功能 ==========
    if (ui->ExportPoints->isChecked() && !exportPath.isEmpty()) {
        exportControlPoints(refined);
    }


    // 确保两张彩色图像高度相同
    int maxHeight = max(colorImgL.rows, colorImgR.rows);
    if (colorImgL.rows < maxHeight) {
        copyMakeBorder(colorImgL, colorImgL, 0, maxHeight - colorImgL.rows, 0, 0, BORDER_CONSTANT, Scalar(0, 0, 0));
    }
    if (colorImgR.rows < maxHeight) {
        copyMakeBorder(colorImgR, colorImgR, 0, maxHeight - colorImgR.rows, 0, 0, BORDER_CONSTANT, Scalar(0, 0, 0));
    }

    // 现在可以安全地进行水平拼接
    Mat show;
    hconcat(colorImgL, colorImgR, show);

    // 绘制匹配结果
    for (const auto& p : refined) {
        Point2f pt1 = p.first;
        Point2f pt2 = p.second;
        pt2.x += colorImgL.cols;  // 右图坐标偏移
        circle(show, pt1, 3, Scalar(0, 255, 0), 2);      // 绿色圆点
        circle(show, pt2, 3, Scalar(0, 255, 0), 2);      // 绿色圆点
        line(show, pt1, pt2, Scalar(0, 255, 0), 3);      // 绿色连线
    }

    // 方法1：先保存图像到文件
    QString tempResultPath = "temp_match_result.jpg";
    bool saveSuccess = imwrite(tempResultPath.toStdString(), show);

    if (saveSuccess) {
        QMessageBox::information(this, "Debug",QString("Result image saved to: %1\nImage size: %2x%3").arg(tempResultPath).arg(show.cols).arg(show.rows));
    }
    else {
        QMessageBox::warning(this, "Debug", "Failed to save result image");
    }

    // 方法2：使用OpenCV窗口直接显示（确保能看到结果）
    namedWindow("Match Result - OpenCV Window", WINDOW_NORMAL);
    resizeWindow("Match Result - OpenCV Window", 1200, 600);
    imshow("Match Result - OpenCV Window", show);
    waitKey(100);  // 短暂等待，确保窗口显示

    // 方法3：在Qt Label中显示
    try {
        // 确保图像数据有效
        if (show.empty()) {
            QMessageBox::critical(this, "Error", "Result display image is empty");
            return;
        }

        // 转换为RGB格式用于Qt显示
        Mat displayImage;
        cvtColor(show, displayImage, COLOR_BGR2RGB);

        // 创建QImage（使用深拷贝避免数据失效）
        QImage qimg(displayImage.data, displayImage.cols, displayImage.rows,
            displayImage.step, QImage::Format_RGB888);

        // 创建深拷贝，确保图像数据独立
        QImage qimg_copy = qimg.copy();

        if (qimg_copy.isNull()) {
            QMessageBox::critical(this, "Error", "Cannot create display image!");
            return;
        }

        // 获取label的实际可用尺寸
        QSize labelSize = ui->labelResult->size();

        // 缩放图像以适应label，同时保持宽高比
        QPixmap pixmap = QPixmap::fromImage(qimg_copy);
        QPixmap scaledPixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);

        // 显示图像
        ui->labelResult->setPixmap(scaledPixmap);
        ui->labelResult->setAlignment(Qt::AlignCenter);

        // 强制刷新
        ui->labelResult->repaint();
        QApplication::processEvents();

       

    }
    catch (const exception& e) {QMessageBox::critical(this, "Exception",QString("Exception when displaying image: %1").arg(e.what()));}
    catch (...) {QMessageBox::critical(this, "Exception", "Unknown exception when displaying image");}
}