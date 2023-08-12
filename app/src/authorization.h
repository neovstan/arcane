#ifndef ARCANE_APP_AUTHORIZATION_H
#define ARCANE_APP_AUTHORIZATION_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Authorization;
}
QT_END_NAMESPACE

class QGraphicsOpacityEffect;

namespace arcane::app {
class Client;

namespace packets {
class Initialization;
} // namespace packets

class Authorization : public QWidget
{
    Q_OBJECT

public:
    explicit Authorization(Client *client, QWidget *parent = nullptr);
    ~Authorization() override;

signals:
    Q_SIGNAL void update();
    Q_SIGNAL void initialization(const packets::Initialization &packet);

public:
    QGraphicsOpacityEffect *getOpacityEffect() const;

private:
    void packetHandler(const QString &answer);
    void initializationPacket(const packets::Initialization &packet);

private slots:
    void signinButtonClicked();

private:
    std::string getHwid();

private:
    Ui::Authorization *ui;
    QGraphicsOpacityEffect *opacityEffect_;

private:
    Client *client_;
};
} // namespace arcane::app

#endif // ARCANE_APP_AUTHORIZATION_H
