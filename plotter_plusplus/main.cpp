#include "plotter_mainview.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    QCoreApplication::addLibraryPath(QCoreApplication::applicationDirPath());
    QCoreApplication::addLibraryPath(QDir::currentPath());

    PlotterMainView w;
    QObject::connect(&a, &QApplication::lastWindowClosed, &a, &QApplication::quit);

//    QRect screenGeometry = QApplication::desktop()->availableGeometry();
//    w.resize(screenGeometry.width(), screenGeometry.height());
//    w.showMaximized();
    w.show();
    return a.exec();
}
