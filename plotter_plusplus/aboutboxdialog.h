#ifndef ABOUTBOXDIALOG_H
#define ABOUTBOXDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutBoxDialog;
}

class AboutBoxDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutBoxDialog(QWidget *parent = 0);
    ~AboutBoxDialog();

private:
    Ui::AboutBoxDialog *ui;
};

#endif // ABOUTBOXDIALOG_H
