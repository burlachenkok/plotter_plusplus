#ifndef PLOTTER_SCATTER3D_H
#define PLOTTER_SCATTER3D_H

#include <QWidget>
#include <QTimerEvent>
#include <QtDataVisualization>

using namespace QtDataVisualization;

namespace Ui {
    class plotter_scatter3d;
}

class plotter_scatter3d : public QWidget
{
    Q_OBJECT

public:
    explicit plotter_scatter3d(QWidget *parent = 0);
    ~plotter_scatter3d();

    void addSeries(double* x, double* y, double* z, size_t numPoints,
                   unsigned char redColor, unsigned char greenColor, unsigned char blueColor);

    void removeSeries();

public slots:
    void preSlotRedrawGraphs(bool theChangePoints, bool theChangePlots);
    void slotRedrawGraphs(bool theChangePoints, bool theChangePlots);

private slots:
    void on_cbxShadowQuality_activated(int quality);

    void on_cbxShowGrid_clicked();

    void on_cbxShowBackground_clicked();

    void on_btnCameraRest_clicked();

    void on_edtXaxeName_textChanged(const QString &arg1);

    void on_edtYaxeName_textChanged(const QString &arg1);

    void on_edtZaxeName_textChanged(const QString &arg1);

    void on_zoomIn_clicked();

    void on_zoomOut_clicked();

    void on_btnRefreshViews_clicked();

    void on_tblXview_cellClicked(int row, int column);

    void on_tblYview_cellClicked(int row, int column);

    void on_tblZview_cellClicked(int row, int column);

    void on_spPointSize_valueChanged(double arg1);

    void on_btnPointColor_clicked();

    void on_cbxPrimitives_activated(int index);

    void on_btnUpdateView_clicked();

    void on_cbxUpdateViewForPointCloud_clicked();

protected:

    void timerEvent(QTimerEvent* event) override;

private:
    Ui::plotter_scatter3d *ui;
    Q3DScatter* graph;
    QColor pointsColor;

    int timerId;
    bool changePoints;
    bool changePlots;

};

#endif // PLOTTER_SCATTER3D_H
