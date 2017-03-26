#ifndef PLOTTER_MAINVIEW_H
#define PLOTTER_MAINVIEW_H

#include <QMainWindow>
#include "graphs_context.h"

namespace Ui {
    class PlotterMainView;
}

class PlotterMainView : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlotterMainView(QWidget *parent = 0);
    ~PlotterMainView();

    bool loadFromFile(QString fileName);
    bool saveToFile(QString fileName);

    bool setCurrentFile(QString fileName);

private slots:
    void on_action_Exit_triggered();

    void on_action_About_triggered();

    void on_action_Load_triggered();

    void on_action_Save_triggered();

    void on_action_Save_As_triggered();

signals:
   void signalPlotsUpdate(bool newPoints, bool newPlots);
   void signalConsoleLogLoad();

private:
    Ui::PlotterMainView *ui;
    QString currentFilename;
};

#endif // PLOTTER_MAINVIEW_H
