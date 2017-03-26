#ifndef PLOTTER_GRAPH_H
#define PLOTTER_GRAPH_H

#include <QtCharts/QChartView>
#include <QWidget>

QT_CHARTS_USE_NAMESPACE

namespace Ui {
    class plotter_graph;
}

class plotter_graph : public QWidget
{
    Q_OBJECT

public:
    explicit plotter_graph(QWidget *parent = 0);
    ~plotter_graph();

public slots:
    void slotRedrawGraphs(bool theChangePoints, bool theChangePlots);

private slots:
    void on_cbxPlotGraphsAsSpline_stateChanged(int);

    void on_btnZoomIn_clicked();

    void on_btnZoomOut_clicked();

    void on_btnZoomReset_clicked();

    void on_btnGraphicUp_clicked();

    void on_btnGraphicDown_clicked();

    void on_btnGraphicRight_clicked();

    void on_btnGraphicLeft_clicked();


    void on_cbxChartTheme_activated(int index);

    void on_cbxSupportPositioning_stateChanged(int arg1);

protected:
    void timerEvent(QTimerEvent* event) override;

    void doSlotRedrawGraphs();

    bool eventFilter(QObject* object, QEvent* event);

public:
    Ui::plotter_graph *ui;

private:
    QChartView* chartView;
    QChart* chart;
#if 0
    QPoint chartViewLasLeftClicked;
#endif

    int chartZooms;
    int timerId;
    bool changePoints;
    bool changePlots;
};

#endif // PLOTTER_GRAPH_H
