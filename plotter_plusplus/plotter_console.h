#ifndef PLOTTER_CONSOLE_H
#define PLOTTER_CONSOLE_H

#include <QWidget>

namespace Ui {
    class plotter_console;
}

class plotter_console : public QWidget
{
    Q_OBJECT

public:
    explicit plotter_console(QWidget *parent = 0);
    ~plotter_console();

public slots:
    void slotLog(const QString& text);
    void slotCleanLog();
    void slotLoadLogFromContext();

private slots:
    void on_edtConsole_textChanged();

    void on_btnClean_clicked();

private:
    Ui::plotter_console *ui;
};

#endif // PLOTTER_CONSOLE_H
