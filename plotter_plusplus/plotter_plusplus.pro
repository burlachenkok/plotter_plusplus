#-------------------------------------------------
#
# Project created by QtCreator 2016-12-20T16:20:08
#
#-------------------------------------------------

# Qmake documentation:   http://doc.qt.io/qt-5/qmake-language.html
#                        http://doc.qt.io/qt-5/qmake-project-files.html

QT       += core gui network charts datavisualization

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = plotter_plusplus
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
    plotter_cfg.cpp \
    plotter_mainview.cpp \
    plotter_graph.cpp \
    plotter_values.cpp \
    aboutboxdialog.cpp \
    plotter_console.cpp \
    plotter_image.cpp \
    plotter_scatter3d.cpp

HEADERS  += \
    plotter_cfg.h \
    plotter_mainview.h \
    plotter_graph.h \
    graphs_context.h \
    plotter_values.h \
    aboutboxdialog.h \
    plotter_console.h \
    plotter_image.h \
    plotter_scatter3d.h

FORMS    += \
    plotter_cfg.ui \
    plotter_mainview.ui \
    plotter_graph.ui \
    plotter_values.ui \
    aboutboxdialog.ui \
    plotter_console.ui \
    plotter_image.ui \
    plotter_scatter3d.ui

RESOURCES += \
    plotter_resource.qrc
