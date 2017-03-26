#ifndef PLOTTER_VALUES_H
#define PLOTTER_VALUES_H

#include <QWidget>

namespace Ui {
    class plotter_values;
}

class plotter_values : public QWidget
{
    Q_OBJECT

public:
    explicit plotter_values(QWidget *parent = 0);
    ~plotter_values();

public slots:
    void slotRedrawGraphs(bool changePoints, bool changePlots);

signals:
    void signalRedrawGraphs(bool changePoints, bool changePlots);

public slots:
    void slotAppendNewGraph();
    void slotRemoveCurrentGraph();

    void slotAppendPointForCurrentGraph();
    void slotRemovePointForCurrentGraph();
    void slotRemoveAllPointForCurrentGraph();

private slots:
    void on_tblGraphs_cellDoubleClicked(int row, int column);

    void on_tblGraphs_cellChanged(int row, int column);

    void on_tblPoints_cellChanged(int row, int column);

    void on_tblGraphs_itemSelectionChanged();

    void on_btnRefreshView_clicked();

    void on_btnIverseGraphVisible_clicked();

    void on_btnDeleteAllGraphs_clicked();

private:
    Ui::plotter_values *ui;
    bool ignoreGraphsCellChanged;
    bool ignorePointsCellChanged;

};

#endif // PLOTTER_VALUES_H
