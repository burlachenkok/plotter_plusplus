#ifndef PLOTTER_CFG_H
#define PLOTTER_CFG_H

#include <QString>
#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMap>

namespace Ui {
    class Plotter_Cfg;
}

class Plotter_Cfg : public QWidget
{
    Q_OBJECT

public:
    explicit Plotter_Cfg(QWidget *parent = 0);
    ~Plotter_Cfg();

signals:
    void updatePlotsFromNetwork(bool changePoints, bool changePlots);
    void updateLogFromNetwork(const QString& text);
    void signalConsoleLogLoad();
    void updateImageFromNetwork(const void* bitmap, int width, int height, int sizeInBytes);
protected:
    bool startServer(int port);
    void stopServer();

    void log(const QString& text);
public slots:
    void slotClientDisconnected();
    void slotReadyInputDataFromClient();

    void slotStartListen();
    void slotStopListen();
    void slotNewIncomeConnection();
    void slotCleanLog();

private slots:
    void on_btnCleanStat_clicked();

private:
    Ui::Plotter_Cfg *ui;
    QTcpServer server;
    QList<QTcpSocket*> serverClients;
    QMap<QTcpSocket*, QByteArray> serverClientsCommands;
    unsigned long long totalProcessedCommands;
    unsigned long long totalProcessedBytes;
};

#endif // PLOTTER_CFG_H
