#include "plotter_graph.h"
#include "ui_plotter_graph.h"
#include "graphs_context.h"

#include <QtCharts/QLineSeries>
#include <QtCharts/QSplineSeries>
#include <QDebug>
#include <QGraphicsLayout>
#include <QtGui/QMouseEvent>

namespace
{
    class MyChartView : public QChartView
    {
    public:
        MyChartView(QChart *chart, plotter_graph *theMainWindow = 0)
        : QChartView(chart, theMainWindow)
        , mainWindow(theMainWindow)
        , lastPostionX(-1)
        , lastPostionY(-1)
        , supportPositioning(true)
        {
            setRubberBand(QChartView::RectangleRubberBand);
        }

        bool viewportEvent(QEvent *event)
        {
            return QChartView::viewportEvent(event);
        }

        void mousePressEvent(QMouseEvent *event)
        {
            QChartView::mousePressEvent(event);
        }

        virtual void paintEvent(QPaintEvent *event)
        {
           QChartView::paintEvent(event);
           if (lastPostionX != -1 && lastPostionY != -1)
           {
               QPainter painter(viewport());
               painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
               painter.drawLine(QPointF(0, lastPostionY), QPointF(size().width(), lastPostionY));
               painter.drawLine(QPointF(lastPostionX, 0), QPointF(lastPostionX, size().height()));
               painter.end();
           }
        }

        virtual void leaveEvent(QEvent *
                                //event
                                )
        {
            lastPostionX = lastPostionY = -1;
            viewport()->update();
        }

        virtual void mouseMoveEvent(QMouseEvent *event)
        {
            QChartView::mouseMoveEvent(event);
            if (!event)
                return;

            if (!supportPositioning)
                return;

            lastPostionX = event->pos().x();
            lastPostionY = event->pos().y();

            QPointF pointInChart = chart()->mapToValue(QPointF(lastPostionX, lastPostionY));
            mainWindow->ui->edtCurPoint->setText(QString("x=%1 y=%2").arg(pointInChart.x()).arg(pointInChart.y()));
            mainWindow->ui->edtGraphicName->clear();
            QChartView::mouseMoveEvent(event);

            viewport()->update();
        }

        void mouseReleaseEvent(QMouseEvent *event)
        {
            QChartView::mouseReleaseEvent(event);
        }

        void keyPressEvent(QKeyEvent *event)
        {
            QGraphicsView::keyPressEvent(event);
        }

        void setSupportPositioning(bool on)
        {
            supportPositioning = on;
            lastPostionX = lastPostionY = -1;
            viewport()->update();
        }

    private:
        plotter_graph* mainWindow;
        int lastPostionX;
        int lastPostionY;
        bool supportPositioning;
    };
}
plotter_graph::plotter_graph(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter_graph),
    chartZooms(0),
    timerId(0),
    changePoints(false),
    changePlots(false)
{
    ui->setupUi(this);

    chart = new QChart();

    chart->createDefaultAxes();
    chart->setTitle("");
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView = new MyChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    ui->loForPlots->addWidget(chartView);

    chart->layout()->setContentsMargins(0,0,0,0);
    chart->setBackgroundRoundness(0);

    chartView->installEventFilter(this);
}

bool plotter_graph::eventFilter(QObject *object, QEvent *event)
{
    if (object == chartView)
    {
        if (event->type() == QEvent::KeyPress)
        {
            double dr = ui->spbStepForScrollPlot->value();

            switch ( ((QKeyEvent*)event)->key())
            {
            case Qt::Key_W:
                chart->scroll(0, dr);
                break;
            case Qt::Key_S:
                chart->scroll(0, -dr);
                break;
            case Qt::Key_A:
                chart->scroll(-dr, 0);
                break;
            case Qt::Key_D:
                chart->scroll(dr, 0);
                break;
            case Qt::Key_Plus:
                {
                    chart->zoomIn();
                    chartZooms++;
                    break;
                }
            case Qt::Key_Minus:
                {
                    chart->zoomOut();
                    chartZooms--;
                    break;
                }
            }
        }
        else if (event->type() == QEvent::Wheel)
        {
            QWheelEvent* wevent = (QWheelEvent*)event;
            if (wevent->delta() > 0)
            {
                chart->zoomIn();
                chartZooms++;
            }
            else if (wevent->delta() < 0)
            {
                chart->zoomOut();
                chartZooms--;
            }
        }
#if 0
        else if (event->type() == QEvent::MouseButtonPress)
        {
            qDebug() << "press";
            QMouseEvent* mevent = (QMouseEvent*)event;
            if (mevent->button() == Qt::LeftButton)
                chartViewLasLeftClicked = mevent->pos();
        }
#endif
    }

    return QWidget::eventFilter(object, event);
}

plotter_graph::~plotter_graph()
{
    delete ui;
    chart = nullptr;
    chartView = nullptr;
}
void plotter_graph::slotRedrawGraphs(bool theChangePoints, bool theChangePlots)
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

void plotter_graph::doSlotRedrawGraphs()
{
    if (changePoints == false && changePlots == false)
        return;

    if (!chart)
        return;

    chart->removeAllSeries();

    GraphsContext& ctx = graphContext();
    size_t numberOfGraphs = ctx.graphics.size();

    for (size_t i = 0; i < numberOfGraphs; ++i)
    {
        GraphsContext::Plot2D& plot = ctx.graphics[i];
        if (!plot.visible)
            continue;

        QXYSeries *series = ui->cbxPlotGraphsAsSpline->isChecked() ? new QSplineSeries : new QLineSeries;
        series->setName(plot.name.c_str());
        QColor color(plot.rgbColor[0], plot.rgbColor[1], plot.rgbColor[2]);
        if (plot.plotDotLine)
            series->setPen(QPen(color, plot.widthInPixels, Qt::DotLine));
        else
            series->setPen(QPen(color, plot.widthInPixels, Qt::SolidLine));

        size_t numOfPoints = plot.size();
        for (size_t p = 0; p < numOfPoints; ++p)
            series->append(plot.xCoord[p], plot.yCoord[p]);

        QString graphicName = ctx.graphics[i].name.c_str();
        QObject::connect(series, &QXYSeries::hovered, [=](const QPointF &point, bool /*state*/)
                                                      {
                                                         ui->edtCurPoint->setText(QString("[x,y]=%1,%2").arg(point.x()).arg(point.y()));
                                                         ui->edtGraphicName->setText(QString("%1").arg(graphicName));
                                                      }
                        );

        chart->addSeries(series);
    }

    chart->createDefaultAxes();

    for (int i = 0; i < chartZooms; ++i)
        chart->zoomIn();
    for (int i = 0; i < -chartZooms; ++i)
        chart->zoomOut();

    changePoints = false;
    changePlots = false;
}

void plotter_graph::on_cbxPlotGraphsAsSpline_stateChanged(int /*arg1*/)
{
    slotRedrawGraphs(true, true);
}

void plotter_graph::on_btnZoomIn_clicked()
{
    chart->zoomIn();
    chartZooms++;
}

void plotter_graph::on_btnZoomOut_clicked()
{
    chart->zoomOut();
    chartZooms--;
}

void plotter_graph::on_btnZoomReset_clicked()
{
    chart->zoomReset();
    chartZooms = 0;
}

void plotter_graph::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timerId)
    {
        QWidget::timerEvent(event);
        return;
    }
    doSlotRedrawGraphs();
    killTimer(timerId);
    timerId = 0;
}

void plotter_graph::on_btnGraphicUp_clicked()
{
    double dr = ui->spbStepForScrollPlot->value();
    chart->scroll(0, dr);
}

void plotter_graph::on_btnGraphicDown_clicked()
{
    double dr = ui->spbStepForScrollPlot->value();
    chart->scroll(0, -dr);
}

void plotter_graph::on_btnGraphicRight_clicked()
{
    double dr = ui->spbStepForScrollPlot->value();
    chart->scroll(dr, 0.0);
}

void plotter_graph::on_btnGraphicLeft_clicked()
{
    double dr = ui->spbStepForScrollPlot->value();
    chart->scroll(-dr, 0.0);
}

void plotter_graph::on_cbxChartTheme_activated(int index)
{
    switch (index)
    {
    case 0:
        chart->setTheme(QChart::ChartThemeLight);
        break;
    case 1:
        chart->setTheme(QChart::ChartThemeBlueCerulean);
        break;
    case 2:
        chart->setTheme(QChart::ChartThemeDark);
        break;
    case 3:
        chart->setTheme(QChart::ChartThemeBrownSand);
        break;
    case 4:
        chart->setTheme(QChart::ChartThemeBlueNcs);
        break;
    case 5:
        chart->setTheme(QChart::ChartThemeHighContrast);
        break;
    case 6:
        chart->setTheme(QChart::ChartThemeBlueIcy);
        break;
    case 7:
        chart->setTheme(QChart::ChartThemeQt);
        break;
    }

    GraphsContext& ctx = graphContext();
    for (int  i = 0; i < chart->series().size(); ++i)
    {
        QXYSeries* s = (QXYSeries*)(chart->series().at(i));
        int index = ctx.findPlotByName(s->name().toStdString());
        assert(index >= 0);

        QColor color = s->pen().color();
        ctx.graphics[index].rgbColor[0] = color.red();
        ctx.graphics[index].rgbColor[1] = color.green();
        ctx.graphics[index].rgbColor[2] = color.blue();
    }
}

void plotter_graph::on_cbxSupportPositioning_stateChanged(int /*arg1*/)
{
    ((MyChartView*) chartView)->setSupportPositioning(ui->cbxSupportPositioning->isChecked());
}
