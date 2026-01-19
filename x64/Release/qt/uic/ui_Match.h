/********************************************************************************
** Form generated from reading UI file 'Match.ui'
**
** Created by: Qt User Interface Compiler version 6.10.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MATCH_H
#define UI_MATCH_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MatchClass
{
public:
    QWidget *centralWidget;
    QGroupBox *ImagePath;
    QLineEdit *lineEditImgL;
    QPushButton *selectLeftImg;
    QLineEdit *lineEditImgR;
    QPushButton *selectRightImg;
    QGroupBox *relateParam;
    QComboBox *PyramidLevel;
    QLabel *labelPyramid;
    QComboBox *relateWinSize;
    QLabel *labelrelateWinSize;
    QDoubleSpinBox *relateThreshold;
    QLabel *labelthreshold;
    QLabel *labelthreshold_2;
    QSpinBox *upmaxHarris;
    QGroupBox *LSMParam;
    QLabel *LSMlabelrelateWinSize;
    QComboBox *LSMrelateWinSize;
    QSpinBox *maxIterations;
    QLabel *LSMmaxIterations;
    QDoubleSpinBox *LSMrelateThreshold;
    QLabel *LSMlabelthreshold;
    QLabel *LSMepsilon;
    QDoubleSpinBox *epsilon;
    QPushButton *run;
    QLabel *labelResult;
    QGroupBox *PointExport;
    QCheckBox *ExportPoints;
    QLabel *labelExportStyle;
    QPushButton *SelectOutPath;
    QLineEdit *ExportPath;
    QRadioButton *txtStyle;
    QRadioButton *csvStyle;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MatchClass)
    {
        if (MatchClass->objectName().isEmpty())
            MatchClass->setObjectName("MatchClass");
        MatchClass->resize(1422, 767);
        centralWidget = new QWidget(MatchClass);
        centralWidget->setObjectName("centralWidget");
        ImagePath = new QGroupBox(centralWidget);
        ImagePath->setObjectName("ImagePath");
        ImagePath->setGeometry(QRect(10, 0, 571, 81));
        lineEditImgL = new QLineEdit(ImagePath);
        lineEditImgL->setObjectName("lineEditImgL");
        lineEditImgL->setGeometry(QRect(10, 20, 461, 23));
        selectLeftImg = new QPushButton(ImagePath);
        selectLeftImg->setObjectName("selectLeftImg");
        selectLeftImg->setGeometry(QRect(480, 20, 91, 24));
        lineEditImgR = new QLineEdit(ImagePath);
        lineEditImgR->setObjectName("lineEditImgR");
        lineEditImgR->setGeometry(QRect(10, 50, 461, 23));
        selectRightImg = new QPushButton(ImagePath);
        selectRightImg->setObjectName("selectRightImg");
        selectRightImg->setGeometry(QRect(480, 50, 91, 24));
        relateParam = new QGroupBox(centralWidget);
        relateParam->setObjectName("relateParam");
        relateParam->setGeometry(QRect(10, 90, 261, 161));
        PyramidLevel = new QComboBox(relateParam);
        PyramidLevel->addItem(QString());
        PyramidLevel->addItem(QString());
        PyramidLevel->addItem(QString());
        PyramidLevel->addItem(QString());
        PyramidLevel->setObjectName("PyramidLevel");
        PyramidLevel->setGeometry(QRect(10, 30, 101, 21));
        labelPyramid = new QLabel(relateParam);
        labelPyramid->setObjectName("labelPyramid");
        labelPyramid->setGeometry(QRect(120, 30, 91, 16));
        relateWinSize = new QComboBox(relateParam);
        relateWinSize->addItem(QString());
        relateWinSize->addItem(QString());
        relateWinSize->addItem(QString());
        relateWinSize->addItem(QString());
        relateWinSize->addItem(QString());
        relateWinSize->setObjectName("relateWinSize");
        relateWinSize->setGeometry(QRect(10, 60, 101, 21));
        labelrelateWinSize = new QLabel(relateParam);
        labelrelateWinSize->setObjectName("labelrelateWinSize");
        labelrelateWinSize->setGeometry(QRect(120, 60, 91, 16));
        relateThreshold = new QDoubleSpinBox(relateParam);
        relateThreshold->setObjectName("relateThreshold");
        relateThreshold->setGeometry(QRect(10, 90, 101, 21));
        relateThreshold->setMaximum(1.000000000000000);
        relateThreshold->setSingleStep(0.010000000000000);
        relateThreshold->setValue(0.850000000000000);
        labelthreshold = new QLabel(relateParam);
        labelthreshold->setObjectName("labelthreshold");
        labelthreshold->setGeometry(QRect(120, 90, 121, 16));
        labelthreshold_2 = new QLabel(relateParam);
        labelthreshold_2->setObjectName("labelthreshold_2");
        labelthreshold_2->setGeometry(QRect(120, 120, 121, 16));
        upmaxHarris = new QSpinBox(relateParam);
        upmaxHarris->setObjectName("upmaxHarris");
        upmaxHarris->setGeometry(QRect(10, 120, 101, 22));
        upmaxHarris->setMinimum(3000);
        upmaxHarris->setMaximum(8000);
        upmaxHarris->setValue(3000);
        LSMParam = new QGroupBox(centralWidget);
        LSMParam->setObjectName("LSMParam");
        LSMParam->setGeometry(QRect(320, 90, 261, 161));
        LSMlabelrelateWinSize = new QLabel(LSMParam);
        LSMlabelrelateWinSize->setObjectName("LSMlabelrelateWinSize");
        LSMlabelrelateWinSize->setGeometry(QRect(120, 30, 91, 16));
        LSMrelateWinSize = new QComboBox(LSMParam);
        LSMrelateWinSize->addItem(QString());
        LSMrelateWinSize->addItem(QString());
        LSMrelateWinSize->addItem(QString());
        LSMrelateWinSize->addItem(QString());
        LSMrelateWinSize->setObjectName("LSMrelateWinSize");
        LSMrelateWinSize->setGeometry(QRect(10, 30, 101, 21));
        maxIterations = new QSpinBox(LSMParam);
        maxIterations->setObjectName("maxIterations");
        maxIterations->setGeometry(QRect(10, 60, 101, 22));
        maxIterations->setValue(25);
        LSMmaxIterations = new QLabel(LSMParam);
        LSMmaxIterations->setObjectName("LSMmaxIterations");
        LSMmaxIterations->setGeometry(QRect(120, 60, 121, 16));
        LSMrelateThreshold = new QDoubleSpinBox(LSMParam);
        LSMrelateThreshold->setObjectName("LSMrelateThreshold");
        LSMrelateThreshold->setGeometry(QRect(10, 90, 101, 21));
        LSMrelateThreshold->setMaximum(1.000000000000000);
        LSMrelateThreshold->setSingleStep(0.010000000000000);
        LSMrelateThreshold->setValue(0.850000000000000);
        LSMlabelthreshold = new QLabel(LSMParam);
        LSMlabelthreshold->setObjectName("LSMlabelthreshold");
        LSMlabelthreshold->setGeometry(QRect(120, 90, 121, 16));
        LSMepsilon = new QLabel(LSMParam);
        LSMepsilon->setObjectName("LSMepsilon");
        LSMepsilon->setGeometry(QRect(120, 120, 121, 16));
        epsilon = new QDoubleSpinBox(LSMParam);
        epsilon->setObjectName("epsilon");
        epsilon->setGeometry(QRect(10, 120, 101, 22));
        epsilon->setMinimum(0.000000000000000);
        epsilon->setMaximum(1.000000000000000);
        epsilon->setSingleStep(0.001000000000000);
        epsilon->setValue(0.000000000000000);
        run = new QPushButton(centralWidget);
        run->setObjectName("run");
        run->setGeometry(QRect(10, 410, 571, 24));
        labelResult = new QLabel(centralWidget);
        labelResult->setObjectName("labelResult");
        labelResult->setGeometry(QRect(600, 20, 801, 411));
        labelResult->setAlignment(Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignHCenter);
        PointExport = new QGroupBox(centralWidget);
        PointExport->setObjectName("PointExport");
        PointExport->setGeometry(QRect(10, 260, 571, 131));
        ExportPoints = new QCheckBox(PointExport);
        ExportPoints->setObjectName("ExportPoints");
        ExportPoints->setGeometry(QRect(10, 30, 541, 21));
        labelExportStyle = new QLabel(PointExport);
        labelExportStyle->setObjectName("labelExportStyle");
        labelExportStyle->setGeometry(QRect(10, 60, 81, 16));
        SelectOutPath = new QPushButton(PointExport);
        SelectOutPath->setObjectName("SelectOutPath");
        SelectOutPath->setGeometry(QRect(480, 90, 91, 24));
        ExportPath = new QLineEdit(PointExport);
        ExportPath->setObjectName("ExportPath");
        ExportPath->setGeometry(QRect(10, 90, 461, 23));
        txtStyle = new QRadioButton(PointExport);
        txtStyle->setObjectName("txtStyle");
        txtStyle->setGeometry(QRect(90, 60, 96, 21));
        csvStyle = new QRadioButton(PointExport);
        csvStyle->setObjectName("csvStyle");
        csvStyle->setGeometry(QRect(190, 60, 96, 21));
        ExportPoints->raise();
        labelExportStyle->raise();
        SelectOutPath->raise();
        ExportPath->raise();
        csvStyle->raise();
        txtStyle->raise();
        MatchClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MatchClass);
        menuBar->setObjectName("menuBar");
        menuBar->setGeometry(QRect(0, 0, 1422, 21));
        MatchClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MatchClass);
        mainToolBar->setObjectName("mainToolBar");
        MatchClass->addToolBar(Qt::ToolBarArea::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MatchClass);
        statusBar->setObjectName("statusBar");
        MatchClass->setStatusBar(statusBar);

        retranslateUi(MatchClass);

        QMetaObject::connectSlotsByName(MatchClass);
    } // setupUi

    void retranslateUi(QMainWindow *MatchClass)
    {
        MatchClass->setWindowTitle(QCoreApplication::translate("MatchClass", "Match", nullptr));
        ImagePath->setTitle(QCoreApplication::translate("MatchClass", "\345\275\261\345\203\217\346\226\207\344\273\266\351\200\211\346\213\251", nullptr));
        lineEditImgL->setText(QCoreApplication::translate("MatchClass", "select the left image(\344\270\215\350\246\201\344\270\255\346\226\207\350\267\257\345\276\204)", nullptr));
        selectLeftImg->setText(QCoreApplication::translate("MatchClass", "SelectLeft", nullptr));
        lineEditImgR->setText(QCoreApplication::translate("MatchClass", "select the right image(\344\270\215\350\246\201\344\270\255\346\226\207\350\267\257\345\276\204)", nullptr));
        selectRightImg->setText(QCoreApplication::translate("MatchClass", "SelectRight", nullptr));
        relateParam->setTitle(QCoreApplication::translate("MatchClass", "\347\233\270\345\205\263\347\263\273\346\225\260\345\214\271\351\205\215\345\217\202\346\225\260", nullptr));
        PyramidLevel->setItemText(0, QCoreApplication::translate("MatchClass", "4", nullptr));
        PyramidLevel->setItemText(1, QCoreApplication::translate("MatchClass", "3", nullptr));
        PyramidLevel->setItemText(2, QCoreApplication::translate("MatchClass", "2", nullptr));
        PyramidLevel->setItemText(3, QCoreApplication::translate("MatchClass", "1", nullptr));

        labelPyramid->setText(QCoreApplication::translate("MatchClass", "\351\207\221\345\255\227\345\241\224\345\261\202\346\225\260", nullptr));
        relateWinSize->setItemText(0, QCoreApplication::translate("MatchClass", "11", nullptr));
        relateWinSize->setItemText(1, QCoreApplication::translate("MatchClass", "9", nullptr));
        relateWinSize->setItemText(2, QCoreApplication::translate("MatchClass", "13", nullptr));
        relateWinSize->setItemText(3, QCoreApplication::translate("MatchClass", "15", nullptr));
        relateWinSize->setItemText(4, QCoreApplication::translate("MatchClass", "17", nullptr));

        relateWinSize->setCurrentText(QCoreApplication::translate("MatchClass", "11", nullptr));
        labelrelateWinSize->setText(QCoreApplication::translate("MatchClass", "\347\233\270\345\205\263\347\252\227\345\217\243\345\244\247\345\260\217", nullptr));
        labelthreshold->setText(QCoreApplication::translate("MatchClass", "\347\233\270\345\205\263\347\263\273\346\225\260\351\230\210\345\200\274(\350\257\267\350\276\223\345\205\245)", nullptr));
        labelthreshold_2->setText(QCoreApplication::translate("MatchClass", "Harris\346\234\200\345\244\232\346\217\220\345\217\226\347\202\271\346\225\260", nullptr));
        LSMParam->setTitle(QCoreApplication::translate("MatchClass", "\346\234\200\345\260\217\344\272\214\344\271\230\345\214\271\351\205\215\345\217\202\346\225\260", nullptr));
        LSMlabelrelateWinSize->setText(QCoreApplication::translate("MatchClass", "\347\233\270\345\205\263\347\252\227\345\217\243\345\244\247\345\260\217", nullptr));
        LSMrelateWinSize->setItemText(0, QCoreApplication::translate("MatchClass", "11", nullptr));
        LSMrelateWinSize->setItemText(1, QCoreApplication::translate("MatchClass", "13", nullptr));
        LSMrelateWinSize->setItemText(2, QCoreApplication::translate("MatchClass", "15", nullptr));
        LSMrelateWinSize->setItemText(3, QCoreApplication::translate("MatchClass", "17", nullptr));

        LSMmaxIterations->setText(QCoreApplication::translate("MatchClass", "\346\234\200\345\244\247\350\277\255\344\273\243\350\275\256\346\254\241", nullptr));
        LSMlabelthreshold->setText(QCoreApplication::translate("MatchClass", "\347\233\270\345\205\263\347\263\273\346\225\260\351\230\210\345\200\274(\350\257\267\350\276\223\345\205\245)", nullptr));
        LSMepsilon->setText(QCoreApplication::translate("MatchClass", "\346\224\266\346\225\233\351\230\210\345\200\274", nullptr));
        run->setText(QCoreApplication::translate("MatchClass", "run", nullptr));
        labelResult->setText(QCoreApplication::translate("MatchClass", "\345\214\271\351\205\215\347\273\223\346\236\234", nullptr));
        PointExport->setTitle(QCoreApplication::translate("MatchClass", "\345\220\214\345\220\215\347\202\271\345\257\274\345\207\272\350\256\276\347\275\256", nullptr));
        ExportPoints->setText(QCoreApplication::translate("MatchClass", "\346\230\257\345\220\246\350\276\223\345\207\272\345\220\214\345\220\215\347\202\271", nullptr));
        labelExportStyle->setText(QCoreApplication::translate("MatchClass", "\351\200\211\346\213\251\345\257\274\345\207\272\346\240\274\345\274\217", nullptr));
        SelectOutPath->setText(QCoreApplication::translate("MatchClass", "SelectOutPath", nullptr));
        txtStyle->setText(QCoreApplication::translate("MatchClass", "TXT", nullptr));
        csvStyle->setText(QCoreApplication::translate("MatchClass", "CSV", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MatchClass: public Ui_MatchClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MATCH_H
