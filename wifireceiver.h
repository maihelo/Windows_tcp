#ifndef WIFIRECEIVER_H
#define WIFIRECEIVER_H

#include <QMainWindow>
#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostInfo>
#include <QPainter>
#include "qcustomplot.h"
namespace Ui {
class WifiReceiver;
}

class WifiReceiver : public QMainWindow
{
    Q_OBJECT

public:
    explicit WifiReceiver(QWidget *parent = 0);
    ~WifiReceiver();

private slots:
    void on_BTN_connect_clicked();
    void on_BTN_disconnect_clicked();
    void readMeassage();
    void newSocketConnect();
    bool on_saveButton_clicked();

    void on_BTN_connect1_clicked();

private:
    Ui::WifiReceiver *ui;
    QTcpServer *g_tcpServer;
    QTcpSocket *g_tcpSocket;
    QString g_tcpIp;
    int g_tcpPort;
    int g_posArr[2];
};

#endif // WIFIRECEIVER_H
