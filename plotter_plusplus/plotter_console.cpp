#include "plotter_console.h"
#include "ui_plotter_console.h"
#include "graphs_context.h"
#include <QTime>

plotter_console::plotter_console(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::plotter_console)
{
    ui->setupUi(this);
}

plotter_console::~plotter_console()
{
    delete ui;
}

void plotter_console::slotLog(const QString& text)
{
    ui->edtConsole->setText(ui->edtConsole->toPlainText() + QTime::currentTime().toString() + ">>" + text + "\n");
    graphContext().consoleLog = ui->edtConsole->toPlainText().toUtf8().toStdString();
}

void plotter_console::slotLoadLogFromContext()
{
    ui->edtConsole->setText(graphContext().consoleLog.c_str());
}

void plotter_console::slotCleanLog()
{
    ui->edtConsole->clear();
}

void plotter_console::on_edtConsole_textChanged()
{
    graphContext().consoleLog = ui->edtConsole->toPlainText().toUtf8().toStdString();
}

void plotter_console::on_btnClean_clicked()
{
    ui->edtConsole->setText(QString());
}
