#include "aboutboxdialog.h"
#include "ui_aboutboxdialog.h"

#include <QtGlobal>
#include <sstream>
#include <string>

namespace {
    std::string gPlotterVersion = "17.03";
}

AboutBoxDialog::AboutBoxDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutBoxDialog)
{
    ui->setupUi(this);

    std::stringstream info;
    info << "Build date and time: " << __DATE__ << " / " << __TIME__ <<
             " / QT version: " << QT_VERSION_STR << "/ Ploter++ version: " << gPlotterVersion.c_str() << "\n";
    ui->edtBuildInfo->setText(info.str().c_str());
}

AboutBoxDialog::~AboutBoxDialog()
{
    delete ui;
}
