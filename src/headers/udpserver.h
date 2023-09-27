#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QWidget>
#include <QByteArray>
#include <QUdpSocket>
#include <QNetworkDatagram>

namespace Ui {
    class UdpServer;
}

class UdpServer : public QWidget {
Q_OBJECT

public:
    explicit UdpServer(QWidget *parent = nullptr);

    ~UdpServer() override;

signals:

    void Receivepackage(QByteArray data);

    void ServerStart();

    void ServerEnd();

private:
    Ui::UdpServer *ui;

    QUdpSocket *server;

private slots:
    void readPendingDatagrams();
};

#endif // UDPSERVER_H
