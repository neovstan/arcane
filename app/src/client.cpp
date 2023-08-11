#include "client.h"

#include <QObject>
#include <QTcpSocket>
#include <QTimer>

using namespace arcane::app;

Client::Client(const QString &host, quint16 port, QObject *parent)
    : QObject(parent), host_(host), port_(port), socket_(new QTcpSocket(this))
{
    connect(socket_, &QTcpSocket::connected, this, &Client::connected);
    connect(socket_, &QTcpSocket::disconnected, this, &Client::disconnected);
    connect(socket_, &QTcpSocket::readyRead, this, &Client::readyRead);
}

void Client::send(const QByteArray &query)
{
    query_ = query;
    socket_->connectToHost(host_, port_);

    const auto bufferSize = 16 * 1024 * 1024;
    socket_->setReadBufferSize(bufferSize);
    socket_->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, bufferSize);
}

void Client::connected()
{
    socket_->write(query_);
}

void Client::disconnected()
{
    query_ = {};

    Q_EMIT read(buffer_);
    buffer_ = {};
}

void Client::readyRead()
{
    buffer_.push_back(socket_->readAll());
}
