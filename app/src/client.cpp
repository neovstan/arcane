#include "client.h"

#include <QObject>
#include <QTcpSocket>

#include <encryption/encryption.h>
#include <protected_string/protected_string.h>

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
    const auto protectedKeyString = scoped_protected_string(ARCANE_SERVER_KEY);
    const char *const key = protectedKeyString;

    encryption::cbc encryption(key, key);

    query_ = encryption.encrypt(query.toStdString()).c_str();
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

    const auto protectedKeyString = scoped_protected_string(ARCANE_SERVER_KEY);
    const char *const key = protectedKeyString;

    encryption::cbc encryption(key, key);

    buffer_ = encryption.decrypt(buffer_.toStdString()).c_str();

    Q_EMIT read(buffer_);
    buffer_ = {};
}

void Client::readyRead()
{
    buffer_.push_back(socket_->readAll());
}
