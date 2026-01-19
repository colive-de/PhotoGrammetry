#include "Match.h"
#include <QtWidgets/QApplication>
#define QT_USE_MAIN


int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    Match window;
    window.show();
    return app.exec();
}