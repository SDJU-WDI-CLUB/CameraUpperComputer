#include "udpserver.h"
#include "forms/ui_udpserver.h"
#include <QDebug>
#include <QAbstractButton>
#include <iostream>

UdpServer::UdpServer(QWidget *parent) :
        QWidget(parent),
        ui(new Ui::UdpServer) {
    ui->setupUi(this);
    server = new QUdpSocket(this);

    connect(ui->Connect, &QPushButton::pressed, [&]() {
        server->bind(ui->LocalPort->text().toInt());
        connect(server, &QUdpSocket::readyRead,
                this, &UdpServer::readPendingDatagrams);
        ui->Disconnect->setEnabled(true);
        ui->Connect->setEnabled(false);
        ui->RomteIp->setEnabled(false);
        ui->RomtePort->setEnabled(false);
        ui->LocalPort->setEnabled(false);
        emit this->ServerStart();
    });

    connect(ui->Disconnect, &QPushButton::pressed, [&]() {
        ui->Disconnect->setEnabled(false);
        ui->Connect->setEnabled(true);
        ui->RomteIp->setEnabled(true);
        ui->RomtePort->setEnabled(true);
        ui->LocalPort->setEnabled(true);
        server->close();
        server->disconnect();
        emit this->ServerEnd();
    });
}

UdpServer::~UdpServer() {
    delete ui;
}

void UdpServer::readPendingDatagrams() {
    while (server->hasPendingDatagrams()) {
        QNetworkDatagram datagram = server->receiveDatagram();
        auto data = datagram.data();
        std::cout << data.data() << std::endl;
        emit Receivepackage(data);
    }
}
