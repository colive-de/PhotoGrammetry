#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtGui/QScreen>
#include <QtGui/QGuiApplication>
#include "ui_Match.h"
#include <opencv2/opencv.hpp>
using namespace cv;

class Match : public QMainWindow
{
    Q_OBJECT

public:
    Match(QWidget *parent = nullptr);
    ~Match();

private slots:
    void on_selectLeftImg_clicked();   // 选择左影像
    void on_selectRightImg_clicked();  // 选择右影像
    void on_run_clicked();          // 运行匹配

    // 导出相关槽函数
    void on_ExportPoints_stateChanged(int state);
    void on_txtStyle_clicked();
    void on_csvStyle_clicked();
    void on_SelectOutPath_clicked();



private:
    Ui::MatchClass* ui;
    QString leftImgPath;
    QString rightImgPath;
    QString exportPath;


    // 导出功能辅助函数
    QString generateDefaultExportName();
    QString getCurrentExtension();
    QString getFileFilter();
    QString shortenPathForDisplay(const QString& fullPath);
    void updateFormatFromExtension(const QString& filePath);
    void updateExportPathExtension(const QString& extension);
    void exportControlPoints(const std::vector<std::pair<Point2f, Point2f>>& points);

};