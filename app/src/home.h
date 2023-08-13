#ifndef ARCANE_APP_HOME_H
#define ARCANE_APP_HOME_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Home;
}
QT_END_NAMESPACE

class QSvgWidget;
class QGraphicsOpacityEffect;

namespace arcane::app {
class Client;

namespace packets {
class Load;
}

class Home : public QWidget
{
    Q_OBJECT

public:
    explicit Home(Client *client, QWidget *parent = nullptr);
    ~Home() override;

signals:
    Q_SIGNAL void load();
    Q_SIGNAL void loadFinished();

public:
    void setDaysRemaining(unsigned int days);
    void setNickname(const QString &nickname);
    void setPassword(const QString &password);

public:
    QGraphicsOpacityEffect *getOpacityEffect() const;

private:
    void packetHandler(const QString &answer);
    void loadPacket(const packets::Load &packet);

private slots:
    void loadButtonClicked();

private:
    static void *getGameProcessHandle();

private:
    const int defaultAnimationDuration;

private:
    Ui::Home *ui;
    QGraphicsOpacityEffect *opacityEffect_;

private:
    Client *client_;

private:
    QString nickname_;
    QString password_;
};
} // namespace arcane::app

#endif // ARCANE_APP_HOME_H
