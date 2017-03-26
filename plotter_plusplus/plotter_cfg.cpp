#include "plotter_cfg.h"
#include "ui_plotter_cfg.h"
#include "graphs_context.h"

#include <QMessageBox>
#include <QStringList>

#include <assert.h>

Plotter_Cfg::Plotter_Cfg(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plotter_Cfg),
    server(this),
    totalProcessedCommands(0),
    totalProcessedBytes(0)
{
    ui->setupUi(this);
    connect(&server, SIGNAL(newConnection()), this, SLOT(slotNewIncomeConnection()));
}

Plotter_Cfg::~Plotter_Cfg()
{
    delete ui;
}

bool Plotter_Cfg::startServer(int port)
{
    bool startListen = server.listen(QHostAddress::Any, quint16(port));
    if (!startListen)
    {
        QMessageBox::critical(this, tr("Plotter++"), tr("Unable to start listern port %1: %2").arg(quint16(port)).arg(server.errorString()));
    }
    return startListen;
}

void Plotter_Cfg::slotNewIncomeConnection()
{
    QTcpSocket* sIncome = server.nextPendingConnection();
    serverClients.append(sIncome);

    connect(sIncome, SIGNAL(disconnected()), this, SLOT(slotClientDisconnected()));
    connect(sIncome, SIGNAL(readyRead()), this, SLOT(slotReadyInputDataFromClient()));

    log(QString("New income connection"));

    ui->edtClientsCount->setText(QString::number(serverClients.size()));
}

void Plotter_Cfg::slotCleanLog()
{
    ui->edtLog->clear();
}

void Plotter_Cfg::slotReadyInputDataFromClient()
{
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    if (pClient->isOpen())
    {
        QByteArray result;
        while (!pClient->atEnd())
        {
            result.append(pClient->readAll());
            QCoreApplication::processEvents();
        }
        log(QString("Receive %1 bytes from client").arg(result.size()));

        QByteArray& allComands = serverClientsCommands[pClient];
        allComands.append(result);

        // process all income messages
        {
            int notProcessedPart = 0;
            unsigned long long numCommands = 0;

            GraphsContext& ctx = graphContext();
            bool newPlots = false;
            bool newPointsForPlots = false;

            QString command;

            for (int nextByteIndex = notProcessedPart; nextByteIndex < allComands.size(); ++nextByteIndex)
            {
                if (allComands[nextByteIndex] != '\n')
                {
                    command.append(QChar(allComands[nextByteIndex]));
                    continue;
                }

                // Save point in bytestream where we are
                // allComands[nextByteIndex] == '\n', so:
                //  -- next command starts from nextByteIndex + 1
                //  -- nextByteIndex + 1 can be dicarded after processing
                notProcessedPart = nextByteIndex + 1;

                {
                    //  "plot,graphname,x,y\n"
                    //  "log,message\n"
                    //  "image,width,heigt,size\n" and then "size bytes"
                    //  "clean plot|log\n"

                    QStringList cmdAndArgs = command.split(",");
                    assert(cmdAndArgs.size() >= 1);

                    if (cmdAndArgs.size() == 0)
                    {
                        log(QString("Unknown command. Maybe you forgot comma \'%1\'").arg(command));
                    }
                    else if (cmdAndArgs[0] == "image" && cmdAndArgs.size() == 4)
                    {
                        assert(cmdAndArgs.size() == 4);
                        int width = cmdAndArgs[1].toInt();
                        int heigt = cmdAndArgs[2].toInt();
                        int size  = cmdAndArgs[3].toInt();

                        log(QString("Waiting for data for image"));
                        while (allComands.size() - notProcessedPart < size)
                        {
                            allComands.append(pClient->readAll());
                            QCoreApplication::processEvents();
                        }
                        log(QString("All data for image %1 x %2 have been received").arg(width).arg(heigt));

                        unsigned char* bitmap = new unsigned char[size];
                        int filledSize = 0;
                        for (nextByteIndex = notProcessedPart; filledSize < size; ++nextByteIndex, ++filledSize)
                            bitmap[filledSize] = allComands[nextByteIndex];

                        nextByteIndex--; // decrement next byte index because on next iteration it will be incremented
                        notProcessedPart = nextByteIndex + 1;

                        emit updateImageFromNetwork(bitmap, width, heigt, size);
                        delete[] bitmap;
                        numCommands++;
                    }
                    else if (cmdAndArgs[0] == "log" && cmdAndArgs.size() == 2)
                    {
                        assert(cmdAndArgs.size() == 2);
                        emit updateLogFromNetwork(cmdAndArgs[1]);
                        numCommands++;
                    }
                    else if (cmdAndArgs[0] == "plotCfg" )
                    {
                        int index = ctx.findPlotByName(cmdAndArgs[1].toStdString());
                        if (index == -1)
                        {
                            GraphsContext::Plot2D newPlot;
                            newPlot.name = cmdAndArgs[1].toStdString();
                            for (int i = 0; i < 3; ++i)
                                newPlot.rgbColor[i] = qrand()%256;
                            ctx.graphics.push_back(newPlot);
                            index = static_cast<int>(ctx.graphics.size() - 1);
                        }

                        newPlots = true; // Force update plots

                        if (cmdAndArgs[2] == QString("width"))
                        {
                            assert(cmdAndArgs.size() == 4);
                            ctx.graphics[index].widthInPixels = QString(cmdAndArgs[3]).toFloat();
                            numCommands++;
                        }
                        else if (cmdAndArgs[2] == QString("dotLine"))
                        {
                            assert(cmdAndArgs.size() == 4);
                            ctx.graphics[index].plotDotLine = (QString(cmdAndArgs[3]) == "1" || QString(cmdAndArgs[3]).toLower() == "true");
                            numCommands++;
                        }
                        else if (cmdAndArgs[2] == QString("visible"))
                        {
                            assert(cmdAndArgs.size() == 4);
                            ctx.graphics[index].visible = (QString(cmdAndArgs[3]) == "1" || QString(cmdAndArgs[3]).toLower() == "true");
                            numCommands++;
                        }
                        else if (cmdAndArgs[2] == QString("color_rgb"))
                        {
                            assert(cmdAndArgs.size() == 3 + 3);
                            ctx.graphics[index].rgbColor[0] = QString(cmdAndArgs[3]).toInt();
                            ctx.graphics[index].rgbColor[1] = QString(cmdAndArgs[4]).toInt();
                            ctx.graphics[index].rgbColor[2] = QString(cmdAndArgs[5]).toInt();
                            numCommands++;
                        }
                    }
                    else if (cmdAndArgs[0] == "plot" && cmdAndArgs.size() == 4)
                    {
                        assert(cmdAndArgs.size() == 4);

                        int index = ctx.findPlotByName(cmdAndArgs[1].toStdString());

                        //qDebug() << "Income command: " + commands[i];
                        //qDebug() << "Graph index of '" << cmdAndArgs[1].toStdString().c_str()
                        //                               << "' : " << index;

                        if (index == -1)
                        {
                            // new plot
                            GraphsContext::Plot2D newPlot;
                            newPlot.name = cmdAndArgs[1].toStdString();
                            for (int i = 0; i < 3; ++i)
                                newPlot.rgbColor[i] = qrand()%256;
                            ctx.graphics.push_back(newPlot);
                            index = static_cast<int>(ctx.graphics.size() - 1);
                            newPlots = true;
                        }
                        else
                        {
                            newPointsForPlots = true;
                        }

                        ctx.graphics[index].add(cmdAndArgs[2].toDouble(),
                                                cmdAndArgs[3].toDouble());
                        numCommands++;

                        if (ctx.graphics[index].size() > ui->spbMaximumPointsPerPlot->value())
                        {
                            int n = static_cast<int>(ctx.graphics[index].size() - ui->spbMaximumPointsPerPlot->value());
                            ctx.graphics[index].removeFirstNPoint(n);
                        }
                    }
                    else if (cmdAndArgs[0] == "clean" && cmdAndArgs.size() == 2)
                    {
                        assert(cmdAndArgs.size() == 2);

                        if (cmdAndArgs[1] == QString("plots"))
                        {
                            for (size_t i = 0; i < ctx.graphics.size(); ++i)
                            {
                                ctx.graphics[i].xCoord.clear();
                                ctx.graphics[i].yCoord.clear();
                            }

                            newPlots = newPointsForPlots = true;
                            numCommands++;
                        }
                        else if (cmdAndArgs[1] == QString("log"))
                        {
                            ctx.consoleLog.clear();
                            emit signalConsoleLogLoad();
                            numCommands++;
                        }
                    }
                    else
                    {
                        log(QString("Unknown command \'%1\'").arg(command));
                    }
                    command.clear();
                }
            }

            log(QString("Processed %1 commands").arg(numCommands));
            allComands.remove(0, notProcessedPart);
            if (newPlots || newPointsForPlots)
            {
                emit updatePlotsFromNetwork(newPointsForPlots, newPlots);
            }

            totalProcessedBytes += notProcessedPart;
            totalProcessedCommands += numCommands;
            ui->edtProcessCommands->setText( QString::number(totalProcessedCommands) );
            ui->edtProcessedKBytes->setText(QString::number(totalProcessedBytes/1024));
        }
    }
}

void Plotter_Cfg::stopServer()
{
    server.close();
    for (int i = 0; i < serverClients.size(); ++i)
    {
        serverClients[i]->close();
        delete serverClients[i];
    }
    serverClients.clear();
    ui->edtClientsCount->setText(QString::number(serverClients.size()));

    totalProcessedCommands = 0;
    totalProcessedBytes = 0;
    ui->edtProcessCommands->setText(QString::number(totalProcessedCommands));
    ui->edtProcessedKBytes->setText(QString::number(totalProcessedBytes/1024));

}

void Plotter_Cfg::log(const QString& text)
{
    ui->edtLog->setText(ui->edtLog->toPlainText() + text + "\n");
}

void Plotter_Cfg::slotClientDisconnected()
{
    QTcpSocket* pClient = static_cast<QTcpSocket*>(QObject::sender());
    log(QString("Client have been disconnected"));
    serverClients.removeOne(pClient);
    ui->edtClientsCount->setText(QString::number(serverClients.size()));
}

void Plotter_Cfg::slotStartListen()
{
   if (!startServer(ui->edtPort->text().toInt()))
       return;

    ui->btnStartListen->setEnabled(false);
    ui->btnStopListen->setEnabled(true);
    ui->edtPort->setEnabled(false);

    log(QString("Start listen port: %1").arg(quint16(ui->edtPort->text().toInt())));
    log(QString("Now client can be connected via TCP and send planar text \"graphname,add,x,y\\n\""));
}


void Plotter_Cfg::slotStopListen()
{
    stopServer();
    ui->btnStartListen->setEnabled(true);
    ui->btnStopListen->setEnabled(false);
    ui->edtPort->setEnabled(true);
    log(QString("Stop listen port: %1").arg(quint16(ui->edtPort->text().toInt())));
}

void Plotter_Cfg::on_btnCleanStat_clicked()
{
    totalProcessedCommands = 0;
    totalProcessedBytes = 0;
    ui->edtProcessCommands->setText(QString::number(totalProcessedCommands));
    ui->edtProcessedKBytes->setText(QString::number(totalProcessedBytes/1024));
}
