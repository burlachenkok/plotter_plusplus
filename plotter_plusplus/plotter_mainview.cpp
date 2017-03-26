#include "plotter_mainview.h"
#include "plotter_cfg.h"
#include "plotter_graph.h"
#include "plotter_values.h"
#include "plotter_console.h"
#include "plotter_image.h"
#include "plotter_scatter3d.h"

#include "aboutboxdialog.h"

#include "ui_plotter_mainview.h"

#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>

#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>
#include <QFileInfo>
#include <QMessageBox>

PlotterMainView::PlotterMainView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlotterMainView)
{
    ui->setupUi(this);

    auto cfgTab = new Plotter_Cfg(this);
    ui->loConfigureTab->addWidget(cfgTab);

    auto graphTab = new plotter_graph(this);
    ui->loGraphTab->addWidget(graphTab);

    auto valueTab = new plotter_values(this);
    ui->loValuesTab->addWidget(valueTab);

    auto consoleTab = new plotter_console(this);
    ui->loTextConsole->addWidget(consoleTab);

    auto imageTab = new plotter_image(this);
    ui->loImageTab->addWidget(imageTab);

    auto scatterTab = new plotter_scatter3d(this);
    ui->loScatterLayout->addWidget(scatterTab);

    QObject::connect(valueTab, &plotter_values::signalRedrawGraphs, graphTab, &plotter_graph::slotRedrawGraphs);
    QObject::connect(cfgTab, &Plotter_Cfg::updatePlotsFromNetwork, graphTab, &plotter_graph::slotRedrawGraphs);
    QObject::connect(cfgTab, &Plotter_Cfg::updatePlotsFromNetwork, scatterTab, &plotter_scatter3d::preSlotRedrawGraphs);

    QObject::connect(cfgTab, &Plotter_Cfg::updateLogFromNetwork, consoleTab, &plotter_console::slotLog);
    QObject::connect(cfgTab, &Plotter_Cfg::updateImageFromNetwork, imageTab, &plotter_image::slotHandleBitmapInSpecifiedFormat);

    QObject::connect(this, &PlotterMainView::signalPlotsUpdate, valueTab, &plotter_values::slotRedrawGraphs);
    QObject::connect(this, &PlotterMainView::signalPlotsUpdate, graphTab, &plotter_graph::slotRedrawGraphs);
    QObject::connect(this, &PlotterMainView::signalConsoleLogLoad, consoleTab, &plotter_console::slotLoadLogFromContext);
    QObject::connect(cfgTab, &Plotter_Cfg::signalConsoleLogLoad, consoleTab, &plotter_console::slotLoadLogFromContext);
}

PlotterMainView::~PlotterMainView()
{
    delete ui;
}

bool PlotterMainView::saveToFile(QString fileName)
{
    GraphsContext& ctx = graphContext();

    QJsonArray graphics;
    QJsonObject jsonObj;
    for (size_t i = 0; i< ctx.graphics.size(); ++i)
    {
        QJsonObject graph;
        graph["name"] = ctx.graphics[i].name.c_str();
        graph["color_red"]   = ctx.graphics[i].rgbColor[0];
        graph["color_green"] = ctx.graphics[i].rgbColor[1];
        graph["color_blue"]  = ctx.graphics[i].rgbColor[2];
        graph["visible"]     = ctx.graphics[i].visible;
        graph["plot_dot_line"] = ctx.graphics[i].plotDotLine;
        graph["width_in_pixels"] = ctx.graphics[i].widthInPixels;

        QJsonArray points;
        for (size_t j = 0; j < ctx.graphics[i].size(); ++j)
        {
            QJsonArray point;
            point.append(ctx.graphics[i].xCoord[j]);
            point.append(ctx.graphics[i].yCoord[j]);
            points.append(point);
        }
        graph["points"] = points;
        graphics.append(graph);
    }
    jsonObj["numberOfGraphics"]  = int(ctx.graphics.size());
    jsonObj["graphics"] = graphics;
    jsonObj["consoleLog"] = ctx.consoleLog.c_str();

    QString content(QJsonDocument(jsonObj).toJson());
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox msg(QMessageBox::Critical, "Error", tr("Cannot write to file %1:\n%2.").arg(fileName).arg(file.errorString()));
        return false;
    }

    if (file.write(qPrintable(content), content.length()) != content.length())
    {
        if (file.remove()) {
            QMessageBox msg(QMessageBox::Critical, "Error", tr("Can not remove file: %1").arg(fileName));
            return false;
        } else {
            QMessageBox msg(QMessageBox::Critical, "Error", tr("Can not write to file: %1").arg(fileName));
            return false;
        }
    }
    file.close();
    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File %1 saved").arg(fileName), 2000);
    return true;
}

bool PlotterMainView::loadFromFile(QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox msg(QMessageBox::Critical, "Error", tr("Can not read file ") + fileName);
        msg.exec();
        return false;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = file.readAll();
    file.close();

    if (content.isEmpty()) {
        QMessageBox msg(QMessageBox::Critical, "Error", tr("File %1 is empty").arg(fileName));
        return false;
    }

    QJsonParseError parseError = QJsonParseError();
    QJsonDocument doc = QJsonDocument::fromJson(content.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        QMessageBox msg(QMessageBox::Critical, "Error",
                        tr("During parsing: %1 was an error %2").arg(fileName).arg(parseError.errorString())
                        );
        return false;
    }

    QJsonObject jsonObj = doc.object();
    GraphsContext loadContext = GraphsContext();
    QJsonArray graphics = jsonObj["graphics"].toArray();
    loadContext.graphics.resize(graphics.size());
    for (int i = 0; i < graphics.size(); ++i)
    {
        QJsonObject graph = graphics[i].toObject();
        loadContext.graphics[i].name =  graph["name"].toString().toStdString();
        loadContext.graphics[i].rgbColor[0] = graph["color_red"].toInt();
        loadContext.graphics[i].rgbColor[1] = graph["color_green"].toInt();
        loadContext.graphics[i].rgbColor[2] = graph["color_blue"].toInt();
        loadContext.graphics[i].visible = graph["visible"].toBool();
        loadContext.graphics[i].plotDotLine = graph["plot_dot_line"].toBool();
        loadContext.graphics[i].widthInPixels = graph["width_in_pixels"].toDouble();

        QJsonArray points = graph["points"].toArray();
        for (int j = 0; j < points.size(); ++j)
        {
            QJsonArray p = points[j].toArray();
            loadContext.graphics[i].add(p[0].toDouble(), p[1].toDouble());
        }
    }
    loadContext.consoleLog = jsonObj["consoleLog"].toString().toStdString();

    // Update graphics
    graphContext() = loadContext;
    setCurrentFile(fileName);
    emit signalPlotsUpdate(true, true);
    emit signalConsoleLogLoad();

    statusBar()->showMessage(tr("File %1 with %2 graphs have been loaded ").arg(fileName).arg(loadContext.graphics.size()), 2000);

    return true;
}

bool PlotterMainView::setCurrentFile(QString fileName)
{
    currentFilename = fileName;

    if (currentFilename.isEmpty())
    {
        setWindowTitle("plotter++");
    }
    else
    {
        QString basename = QFileInfo(currentFilename).fileName();
        setWindowTitle(basename + " - plotter++");
    }

    bool currentFilenameIsNotEmpty = !currentFilename.isEmpty();
    ui->action_Save->setEnabled(currentFilenameIsNotEmpty);

    return true;
}

void PlotterMainView::on_action_Exit_triggered()
{
    QApplication::quit();
}

void PlotterMainView::on_action_About_triggered()
{
    AboutBoxDialog about;
    about.show();
    about.exec();
}

void PlotterMainView::on_action_Load_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open plain text file with debug data in Json"),
                                                    ".",
                                                    tr("Plain text file (*.txt);;Plain text file (*.json)")
                                                    );
    if (!fileName.isEmpty())
    {
        if (!loadFromFile(fileName))
        {
            QMessageBox msg(QMessageBox::Critical, "Error", tr("Can not load graph from ") + fileName);
            msg.exec();
        }
    }
}

void PlotterMainView::on_action_Save_triggered()
{
    if (currentFilename.isEmpty())
        return;

    saveToFile(currentFilename);
}

void PlotterMainView::on_action_Save_As_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save plain text file with debug data in Json format"),
                                                    ".",
                                                    tr("Plain text file (*.txt);;Plain text file (*.json)"));
    if (fileName.size() == 0)
        return;
    saveToFile(fileName);
}
