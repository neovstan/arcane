#ifndef ARCANE_APP_TCPSOCKET_H
#define ARCANE_APP_TCPSOCKET_H

#include <QObject>

class QTcpSocket;

namespace arcane::app {
class Client : public QObject
{
    Q_OBJECT

public:
    Client(const QString &host, quint16 port, QObject *parent = nullptr);

    void send(const QByteArray &query);

signals:
    void read(const QByteArray &answer);

private slots:
    void connected();
    void disconnected();
    void readyRead();

private:
    QString host_;
    quint16 port_;
    QTcpSocket *socket_;
    QByteArray query_;
    QByteArray buffer_;
};
} // namespace arcane::app

#endif // ARCANE_APP_TCPSOCKET_H
