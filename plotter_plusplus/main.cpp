#include "plotter_mainview.h"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    PlotterMainView w;
    QObject::connect(&a, &QApplication::lastWindowClosed, &a, &QApplication::quit);

    QRect screenGeometry = QApplication::desktop()->availableGeometry();
    w.resize(screenGeometry.width(), screenGeometry.height());
    w.showMaximized();

    return a.exec();
}
