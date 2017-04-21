#include "plotter_values.h"
#include "ui_plotter_values.h"
#include "graphs_context.h"

#include <QColorDialog>
#include <QDebug>
#include <QMessageBox>

#include <vector>
#include <algorithm>
#include <assert.h>

plotter_values::plotter_values(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter_values) ,
    ignoreGraphsCellChanged(false),
    ignorePointsCellChanged(false)
{
    ui->setupUi(this);
}

plotter_values::~plotter_values()
{
    delete ui;
}

void plotter_values::slotRedrawGraphs(bool changePoints, bool changePlots)
{
    if (changePlots)
    {
        GraphsContext& ctx = graphContext();

        bool prevState = ignoreGraphsCellChanged;
        ignoreGraphsCellChanged = true;

        ui->tblGraphs->clearContents();
        ui->tblGraphs->setRowCount(0);
        for (size_t index = 0; index < ctx.graphics.size(); ++index)
        {
            QTableWidgetItem* graphName = new QTableWidgetItem;
            graphName->setText(ctx.graphics[index].name.c_str());

            QTableWidgetItem* graphColor = new QTableWidgetItem;
            graphColor->setBackgroundColor(QColor(ctx.graphics[index].rgbColor[0],
                                                  ctx.graphics[index].rgbColor[1],
                                                  ctx.graphics[index].rgbColor[2]));
            graphColor->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);

            QTableWidgetItem* graphWidth = new QTableWidgetItem;
            graphWidth->setText(QString::number(ctx.graphics[index].widthInPixels));

            QTableWidgetItem* showWithDots = new QTableWidgetItem;
            if (ctx.graphics[index].plotDotLine)
                showWithDots->setCheckState(Qt::Checked);
            else
                showWithDots->setCheckState(Qt::Unchecked);

            QTableWidgetItem* visible = new QTableWidgetItem;
            if (ctx.graphics[index].visible)
                visible->setCheckState(Qt::Checked);
            else
                visible->setCheckState(Qt::Unchecked);

            int r = ui->tblGraphs->rowCount();

            ui->tblGraphs->insertRow(r);
            ui->tblGraphs->setItem(r, 0, graphName);
            ui->tblGraphs->setItem(r, 1, graphColor);
            ui->tblGraphs->setItem(r, 2, graphWidth);
            ui->tblGraphs->setItem(r, 3, showWithDots);
            ui->tblGraphs->setItem(r, 4, visible);
        }

        ignoreGraphsCellChanged = prevState;
    }

    if (changePoints)
    {
        on_tblGraphs_itemSelectionChanged();
    }
}

void plotter_values::slotAppendNewGraph()
{
    int r = ui->tblGraphs->currentRow();
    if (r < 0 || ui->tblGraphs->selectedItems().isEmpty())
        r = ui->tblGraphs->rowCount();

    GraphsContext& ctx = graphContext();
    ctx.graphics.insert(ctx.graphics.begin() + r, GraphsContext::Plot2D());
    ui->tblGraphs->insertRow(r);

    QTableWidgetItem* graphName    = new QTableWidgetItem;
    QTableWidgetItem* graphColor   = new QTableWidgetItem;
    QTableWidgetItem* graphWidth   = new QTableWidgetItem;
    QTableWidgetItem* showWithDots = new QTableWidgetItem;
    QTableWidgetItem* visible      = new QTableWidgetItem;

    graphName->setText(QString("new graph %1").arg(r));
    graphColor->setBackgroundColor(QColor(qrand()%256, qrand()%256, qrand()%256));
    graphColor->setFlags(Qt::NoItemFlags | Qt::ItemIsEnabled);
    graphWidth->setText("1.0");
    showWithDots->setCheckState(Qt::Unchecked);
    visible->setCheckState(Qt::Checked);

    ui->tblGraphs->setItem(r, 0, graphName);
    ui->tblGraphs->setItem(r, 1, graphColor);
    ui->tblGraphs->setItem(r, 2, graphWidth);
    ui->tblGraphs->setItem(r, 3, showWithDots);
    ui->tblGraphs->setItem(r, 4, visible);

    assert(ui->tblGraphs->rowCount() == (int)(graphContext().graphics.size()));
}

void plotter_values::slotRemoveCurrentGraph()
{
    QList<QTableWidgetItem*> items = ui->tblGraphs->selectedItems();
    std::vector<int> rowToRemove;
    for (int i = 0; i < items.size(); ++i)
        rowToRemove.push_back(items[i]->row());

    std::sort(rowToRemove.begin(), rowToRemove.end());
    auto last = std::unique(rowToRemove.begin(), rowToRemove.end());
    rowToRemove.erase(last, rowToRemove.end());

    for (int i = static_cast<int>(rowToRemove.size()) - 1; i >=0; --i)
    {
        ui->tblGraphs->removeRow(rowToRemove[i]);
        graphContext().graphics.erase(graphContext().graphics.begin() + i);
    }

    assert(ui->tblGraphs->rowCount() == (int)(graphContext().graphics.size()));
    emit signalRedrawGraphs(false, true);
}

void plotter_values::slotAppendPointForCurrentGraph()
{
    int graphCurrent = ui->tblGraphs->currentRow();
    if (ui->tblGraphs->selectedItems().isEmpty())
    {
        QMessageBox msg(QMessageBox::Information, "Information", tr("Please select or append graph first"));
        msg.exec();
        return;
    }
    // qDebug() << "ui->tblPoints->currentRow() -- (" << ui->tblPoints->currentRow();

    int rowInPointTable = ui->tblPoints->currentRow();
    if (rowInPointTable < 0 || ui->tblPoints->selectedItems().isEmpty())
        rowInPointTable = ui->tblPoints->rowCount();

    GraphsContext& ctx = graphContext();

    ctx.graphics[graphCurrent].xCoord.insert(
                graphContext().graphics[graphCurrent].xCoord.begin() +rowInPointTable, 0.0);

    ctx.graphics[graphCurrent].yCoord.insert(
                graphContext().graphics[graphCurrent].yCoord.begin() +rowInPointTable, 0.0);

    ui->tblPoints->insertRow(rowInPointTable);
    ui->tblPoints->setItem(rowInPointTable, 0, new QTableWidgetItem());
    ui->tblPoints->setItem(rowInPointTable, 1, new QTableWidgetItem());

    QTableWidgetItem* xItem = ui->tblPoints->item(rowInPointTable, 0);
    QTableWidgetItem* yItem = ui->tblPoints->item(rowInPointTable, 1);
    xItem->setText(QString("0.0"));
    yItem->setText(QString("0.0"));
}

void plotter_values::slotRemovePointForCurrentGraph()
{
    if (ui->tblGraphs->selectedItems().isEmpty())
    {
        QMessageBox msg(QMessageBox::Information, "Information", tr("Please select or append graph first"));
        msg.exec();
        return;
    }

}

void plotter_values::slotRemoveAllPointForCurrentGraph()
{
    if (ui->tblGraphs->selectedItems().isEmpty())
    {
        QMessageBox msg(QMessageBox::Information, "Information", tr("Please select or append graph first"));
        msg.exec();
        return;
    }

    GraphsContext::Plot2D& plot = graphContext().graphics[ui->tblGraphs->currentRow()];
    ui->tblPoints->clearContents();
    ui->tblPoints->setRowCount(0);
    ui->tblPoints->clearSelection();
    plot.xCoord.clear();
    plot.yCoord.clear();

    emit signalRedrawGraphs(true, false);
}

void plotter_values::on_tblGraphs_cellDoubleClicked(int row, int column)
{
    if (column == 1)
    {
        QColor color = QColorDialog::getColor(ui->tblGraphs->item(row, column)->backgroundColor(),
                                              this,
                                              tr("Select color for plot"));
        ui->tblGraphs->item(row, column)->setBackgroundColor(color);

        GraphsContext& ctx = graphContext();

        ctx.graphics[row].rgbColor[0] = color.red();
        ctx.graphics[row].rgbColor[1] = color.green();
        ctx.graphics[row].rgbColor[2] = color.blue();
        emit signalRedrawGraphs(false, true);
    }
}

void plotter_values::on_tblGraphs_cellChanged(int row, int column)
{
    // qDebug() << "on_tblGraphs_cellChanged -- (" << row << "," << column << ")";
    if (ignoreGraphsCellChanged)
        return;

    GraphsContext& ctx = graphContext();

    if (column == 0)
    {
        QTableWidgetItem* graphName = ui->tblGraphs->item(row, 0);
        ctx.graphics[row].name = graphName->text().toStdString();
        ui->lblPerGraph->setText(QString("Points per graph: %1").arg(
                                   ctx.graphics[row].name.c_str()
                                                                   ));

    }
    else if (column == 1)
    {
        QTableWidgetItem* graphColor = ui->tblGraphs->item(row, 1);
        ctx.graphics[row].rgbColor[0] = graphColor->backgroundColor().red();
        ctx.graphics[row].rgbColor[1] = graphColor->backgroundColor().green();
        ctx.graphics[row].rgbColor[2] = graphColor->backgroundColor().blue();
    }
    else if (column == 2)
    {
        QTableWidgetItem* graphWidth = ui->tblGraphs->item(row, 2);
        ctx.graphics[row].widthInPixels = graphWidth->text().toDouble();
    }
    else if (column == 3)
    {
        QTableWidgetItem* showWithDots = ui->tblGraphs->item(row, 3);
        ctx.graphics[row].plotDotLine = (showWithDots->checkState() == Qt::CheckState::Checked);
    }
    else if (column == 4)
    {
        QTableWidgetItem* visible = ui->tblGraphs->item(row, 4);
        ctx.graphics[row].visible = (visible->checkState() == Qt::CheckState::Checked);
    }

    emit signalRedrawGraphs(false, true);
}

void plotter_values::on_tblPoints_cellChanged(int row, int column)
{
    if (ignorePointsCellChanged)
        return;

    int graphCurrent = ui->tblGraphs->currentRow();
    if (ui->tblGraphs->selectedItems().isEmpty())
        return;

    QTableWidgetItem* xCoordItem = ui->tblPoints->item(row, 0);
    QTableWidgetItem* yCoordItem = ui->tblPoints->item(row, 1);

    GraphsContext& ctx = graphContext();

    if (xCoordItem && column == 0)
    {
        double xUpdate = xCoordItem->text().toDouble();
        ctx.graphics[graphCurrent].xCoord[row] = xUpdate;
    }

    if (yCoordItem && column == 1)
    {
        double yUpdate = yCoordItem->text().toDouble();
        ctx.graphics[graphCurrent].yCoord[row] = yUpdate;
    }
    emit signalRedrawGraphs(true, false);
}

void plotter_values::on_tblGraphs_itemSelectionChanged()
{
    if (ui->tblGraphs->selectedItems().isEmpty())
    {
        ui->tblPoints->clearContents();
        ui->tblPoints->setRowCount(0);
        ui->lblPerGraph->setText(QString(""));
        return;
    }

    GraphsContext::Plot2D& plot = graphContext().graphics[ui->tblGraphs->currentRow()];
    size_t numPoints = plot.size();
    ui->tblPoints->setRowCount(static_cast<int>(numPoints));

    bool prevState = ignorePointsCellChanged;
    ignorePointsCellChanged = true;
    for (size_t i = 0; i < numPoints; ++i)
    {
        {
            QTableWidgetItem* xItem = ui->tblPoints->item(static_cast<int>(i), 0);
            if (xItem == nullptr)
                ui->tblPoints->setItem(static_cast<int>(i), 0, new QTableWidgetItem(QString::number(plot.xCoord[i])));
            else
                xItem->setText(QString::number(plot.xCoord[i]));
        }

        {
            QTableWidgetItem* yItem = ui->tblPoints->item(static_cast<int>(i), 1);
            if (yItem == nullptr)
                ui->tblPoints->setItem(static_cast<int>(i), 1, new QTableWidgetItem(QString::number(plot.yCoord[i])));
            else
                yItem->setText(QString::number(plot.yCoord[i]));
        }
    }
    ignorePointsCellChanged = prevState;
    ui->lblPerGraph->setText(QString("Points per graph: %1").arg(plot.name.c_str()));
}

void plotter_values::on_btnRefreshView_clicked()
{
    slotRedrawGraphs(true, true);
}

void plotter_values::on_btnIverseGraphVisible_clicked()
{
    GraphsContext& ctx = graphContext();
    for (size_t i = 0; i < ctx.graphics.size(); ++i)
        ctx.graphics[i].visible = !ctx.graphics[i].visible;

    emit signalRedrawGraphs(true, true);
    slotRedrawGraphs(true, true);
}

void plotter_values::on_btnDeleteAllGraphs_clicked()
{
    GraphsContext& ctx = graphContext();
    ctx.graphics.clear();

    emit signalRedrawGraphs(true, true);
    slotRedrawGraphs(true, true);
}
