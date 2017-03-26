#include "plotter_scatter3d.h"
#include "ui_plotter_scatter3d.h"
#include "graphs_context.h"
#include <QColorDialog>
#include <algorithm>

plotter_scatter3d::plotter_scatter3d(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter_scatter3d),
    pointsColor(Qt::red),
    timerId(0),
    changePoints(false),
    changePlots(false)
{
    ui->setupUi(this);

    graph = new Q3DScatter();
    graph->activeTheme()->setType(Q3DTheme::ThemeArmyBlue);
    graph->setShadowQuality(QAbstract3DGraph::ShadowQualityNone);

    QWidget *container = QWidget::createWindowContainer(graph);
    ui->loForPointClouds->addWidget(container);

    graph->axisX()->setTitle(ui->edtXaxeName->text());
    graph->axisY()->setTitle(ui->edtYaxeName->text());
    graph->axisZ()->setTitle(ui->edtZaxeName->text());
    graph->axisX()->setTitleVisible(true);
    graph->axisY()->setTitleVisible(true);
    graph->axisZ()->setTitleVisible(true);

    if (0)
    {
        double x[] = {+0.5f, +0.5f};
        double y[] = {-0.3f, -1.5f};
        double z[] = {+0.1f, -0.3f};
        addSeries(x, y, z, sizeof(x)/sizeof(x[0]), 255, 0, 0);
    }

    QImage img(ui->btnPointColor->size(), QImage::Format_RGB32);
    img.fill(pointsColor);
    ui->btnPointColor->setIcon(QIcon(QPixmap::fromImage(img)));

    QTableWidget* views[] = {ui->tblXview, ui->tblYview, ui->tblZview};
    for (size_t v = 0; v < sizeof(views)/sizeof(views[0]); ++v)
    {
        views[v]->resizeRowsToContents();
        views[v]->resizeColumnsToContents();
    }
}

void plotter_scatter3d::addSeries(double* x, double* y, double* z, size_t numPoints,
                                    unsigned char redColor, unsigned char greenColor, unsigned char blueColor)
{
    QScatter3DSeries *series = new QScatter3DSeries;
    QScatterDataArray data;
    for (size_t i = 0; i < numPoints; ++i)
    {
        QVector3D appendPoint(0.0, 0.0, 0.0);
        if (x != nullptr) appendPoint.setX(x[i]);
        if (y != nullptr) appendPoint.setY(y[i]);
        if (z != nullptr) appendPoint.setZ(z[i]);

        data << appendPoint;
    }

    series->dataProxy()->addItems(data);
    series->setItemSize(ui->spPointSize->value());
    series->setBaseColor(QColor(redColor, greenColor, blueColor));

    {
        QAbstract3DSeries::Mesh style = QAbstract3DSeries::MeshPoint;
        switch(ui->cbxPrimitives->currentIndex())
        {
        case 0:
            style = QAbstract3DSeries::MeshSphere;
            break;
        case 1:
            style = QAbstract3DSeries::MeshCube;
            break;
        case 2:
            style = QAbstract3DSeries::MeshMinimal;
            break;
        case 3:
            style = QAbstract3DSeries::MeshPoint;
            break;
        }
        series->setMesh(style);
    }

    graph->addSeries(series);
}

void plotter_scatter3d::removeSeries()
{
    auto list = graph->seriesList();
    int numSeries = list.size();
    for (int i = 0; i < numSeries; ++i)
        graph->removeSeries(list[i]);
}

void plotter_scatter3d::preSlotRedrawGraphs(bool theChangePoints, bool theChangePlots)
{
    changePoints |= theChangePoints;
    changePlots  |= theChangePlots;

    if (timerId == 0)
    {
        int millisecondsDelay = 1000;
        if (ui->spbChangeUpdateRate->value() > 0)
            millisecondsDelay = 1000 / ui->spbChangeUpdateRate->value();
        timerId = startTimer(millisecondsDelay);
    }
}

void plotter_scatter3d::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timerId)
    {
        QWidget::timerEvent(event);
        return;
    }

    if (ui->cbxUpdateViewForPointCloud->isChecked())
    {
        slotRedrawGraphs(changePoints, changePlots);
    }

    killTimer(timerId);
    timerId = 0;
    changePoints = false;
    changePlots = false;
}

void plotter_scatter3d::slotRedrawGraphs(bool theChangePoints, bool theChangePlots)
{
    if (theChangePlots)
    {
        GraphsContext& ctx = graphContext();

        QTableWidget* views[] = {ui->tblXview, ui->tblYview, ui->tblZview};

        for (size_t v =0; v < sizeof(views)/sizeof(views[0]); ++v)
        {
            views[v]->clearContents();
            views[v]->setRowCount(0);
            for (size_t index = 0; index < ctx.graphics.size(); ++index)
            {

                QTableWidgetItem* graphName = new QTableWidgetItem;
                graphName->setText(ctx.graphics[index].name.c_str());
                graphName->setBackgroundColor(QColor(ctx.graphics[index].rgbColor[0],
                                                     ctx.graphics[index].rgbColor[1],
                                                     ctx.graphics[index].rgbColor[2]));
                graphName->setFlags(graphName->flags() & ~ Qt::ItemIsEditable & ~Qt::ItemIsSelectable);

                QTableWidgetItem* xItem = new QTableWidgetItem;
                xItem->setText("X");
                xItem->setFlags(xItem->flags() & ~ Qt::ItemIsEditable);

                QTableWidgetItem* yItem = new QTableWidgetItem;
                yItem->setText("Y");
                yItem->setFlags(yItem->flags() & ~ Qt::ItemIsEditable);

                int r = views[v]->rowCount();
                views[v]->insertRow(r);
                views[v]->setItem(r, 0, graphName);
                views[v]->setItem(r, 1, xItem);
                views[v]->setItem(r, 2, yItem);
            }
            views[v]->resizeRowsToContents();
            views[v]->resizeColumnsToContents();
        }
    }

    if (theChangePoints)
    {
        double *x = nullptr;
        size_t xNumPoints = 0;
        if (!ui->tblXview->selectedItems().empty())
        {
            int graphIndex = ui->tblXview->currentRow();
            bool useX = ui->tblXview->currentColumn() - 1 == 0 ? true : false;
            xNumPoints = graphContext().graphics[graphIndex].size();
            if (xNumPoints > 0)
            {
                if (useX)
                    x = &graphContext().graphics[graphIndex].xCoord[0];
                else
                    x = &graphContext().graphics[graphIndex].yCoord[0];
            }
        }

        double *y = nullptr;
        size_t yNumPoints = 0;
        if (!ui->tblYview->selectedItems().empty())
        {
            int graphIndex = ui->tblYview->currentRow();
            bool useX = ui->tblYview->currentColumn() - 1 == 0 ? true : false;
            yNumPoints = graphContext().graphics[graphIndex].size();

            if (yNumPoints > 0)
            {
                if (useX)
                    y = &graphContext().graphics[graphIndex].xCoord[0];
                else
                    y = &graphContext().graphics[graphIndex].yCoord[0];
            }
        }

        double *z = nullptr;
        size_t zNumPoints = 0;
        if (!ui->tblZview->selectedItems().empty())
        {
            int graphIndex = ui->tblZview->currentRow();
            bool useX = ui->tblZview->currentColumn() - 1 == 0 ? true : false;
            zNumPoints = graphContext().graphics[graphIndex].size();

            if (zNumPoints > 0)
            {
                if (useX)
                    z = &graphContext().graphics[graphIndex].xCoord[0];
                else
                    z = &graphContext().graphics[graphIndex].yCoord[0];
            }
        }

        removeSeries();

        {
            size_t minPoints = 0;
            bool passAppend = false;

            if (x) minPoints = xNumPoints;
            else if (y) minPoints = yNumPoints;
            else if (z) minPoints = zNumPoints;
            else passAppend = true;

            if (!passAppend)
            {
                if (x && minPoints > xNumPoints)
                    minPoints = xNumPoints;
                if (y && minPoints > yNumPoints)
                    minPoints = yNumPoints;
                if (z && minPoints > zNumPoints)
                    minPoints = zNumPoints;
                addSeries(x, y, z, minPoints, pointsColor.red(), pointsColor.green(), pointsColor.blue());
            }
        }
    }
}

plotter_scatter3d::~plotter_scatter3d()
{
    removeSeries();
    delete ui;
}

void plotter_scatter3d::on_cbxShadowQuality_activated(int quality)
{
    QAbstract3DGraph::ShadowQuality sq = QAbstract3DGraph::ShadowQuality(quality);
    graph->setShadowQuality(sq);
}

void plotter_scatter3d::on_cbxShowGrid_clicked()
{
    graph->activeTheme()->setGridEnabled(ui->cbxShowGrid->isChecked());
}

void plotter_scatter3d::on_cbxShowBackground_clicked()
{
    graph->activeTheme()->setLabelBackgroundEnabled(ui->cbxShowBackground->isChecked());
    graph->activeTheme()->setBackgroundEnabled(ui->cbxShowBackground->isChecked());
}

void plotter_scatter3d::on_btnCameraRest_clicked()
{
    Q3DCamera::CameraPreset preset = Q3DCamera::CameraPresetFront;
    switch(ui->cbxCameraPreset->currentIndex())
    {
    case 0:
        preset =Q3DCamera::CameraPresetFront;
        break;
    case 1:
        preset =Q3DCamera::CameraPresetLeft;
        break;
    case 2:
        preset =Q3DCamera::CameraPresetRight;
        break;
    case 3:
        preset =Q3DCamera::CameraPresetBehind;
        break;
    }
    graph->scene()->activeCamera()->setCameraPreset(preset);
    graph->scene()->activeCamera()->setZoomLevel(100.0f);
}

void plotter_scatter3d::on_edtXaxeName_textChanged(const QString &/*arg1*/)
{
    graph->axisX()->setTitle(ui->edtXaxeName->text());
}

void plotter_scatter3d::on_edtYaxeName_textChanged(const QString &/*arg1*/)
{
    graph->axisY()->setTitle(ui->edtYaxeName->text());
}

void plotter_scatter3d::on_edtZaxeName_textChanged(const QString &/*arg1*/)
{
    graph->axisZ()->setTitle(ui->edtZaxeName->text());
}

void plotter_scatter3d::on_zoomIn_clicked()
{
    const float percentageStep = 5.0f;
    graph->scene()->activeCamera()->setZoomLevel(graph->scene()->activeCamera()->zoomLevel() + percentageStep);
}

void plotter_scatter3d::on_zoomOut_clicked()
{
    const float percentageStep = 5.0f;
    graph->scene()->activeCamera()->setZoomLevel(graph->scene()->activeCamera()->zoomLevel() - percentageStep);
}

void plotter_scatter3d::on_btnRefreshViews_clicked()
{
    slotRedrawGraphs(true, true);
}

void plotter_scatter3d::on_tblXview_cellClicked(int /*row*/, int /*column*/)
{
    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_tblYview_cellClicked(int /*row*/, int /*column*/)
{
    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_tblZview_cellClicked(int /*row*/, int /*column*/)
{
    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_spPointSize_valueChanged(double /*arg1*/)
{
    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_btnPointColor_clicked()
{
    QColor color = QColorDialog::getColor(pointsColor,
                                          this,
                                          tr("Select color for point cloud"));

    pointsColor = color;
    QImage img(ui->btnPointColor->size(), QImage::Format_RGB32);
    img.fill(pointsColor);
    ui->btnPointColor->setIcon(QIcon(QPixmap::fromImage(img)));

    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_cbxPrimitives_activated(int index)
{
    QAbstract3DSeries::Mesh style = QAbstract3DSeries::MeshSphere;
    switch(index)
    {
    case 0:
        style = QAbstract3DSeries::MeshSphere;
        break;
    case 1:
        style = QAbstract3DSeries::MeshCube;
        break;
    case 2:
        style = QAbstract3DSeries::MeshMinimal;
        break;
    case 3:
        style = QAbstract3DSeries::MeshPoint;
        break;
    }

    for (int i = 0; i < graph->seriesList().size(); ++i)
        graph->seriesList().at(i)->setMesh(style);
}

void plotter_scatter3d::on_btnUpdateView_clicked()
{
    slotRedrawGraphs(true, false);
}

void plotter_scatter3d::on_cbxUpdateViewForPointCloud_clicked()
{
    ui->spbChangeUpdateRate->setEnabled(ui->cbxUpdateViewForPointCloud->isChecked());
}
