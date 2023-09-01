#include "authorization.h"

#include <QTimer>
#include <QSettings>
#include <QGraphicsOpacityEffect>

#include <utils/utils.h>
#include <protected_string/protected_string.h>

#include "query.h"
#include "client.h"
#include "notification.h"

#include "packets/initialization.hpp"

#include "ui_authorization.h"

using namespace arcane::app;

Authorization::Authorization(Client *client, QWidget *parent)
    : QWidget(parent),
      ui(new Ui::Authorization),
      opacityEffect_(new QGraphicsOpacityEffect),
      client_(client)
{
    ui->setupUi(this);

    setGraphicsEffect(opacityEffect_);

    connect(ui->buttonSignin, &QPushButton::clicked, this, &Authorization::signinButtonClicked);
    connect(client_, &Client::read, this, &Authorization::packetHandler);

    QSettings settings;

    ui->inputNickname->setText(settings.value("nickname").toString());
    ui->inputPassword->setText(settings.value("password").toString());
}

Authorization::~Authorization()
{
    delete ui;
}

QGraphicsOpacityEffect *Authorization::getOpacityEffect() const
{
    return opacityEffect_;
}

void Authorization::packetHandler(const QString &answer)
{
    try {
        const auto json = nlohmann::json::parse(answer.toStdString());
        const auto &id = json["query"];

        if (id == scoped_protected_std_string("initialization")) {
            initializationPacket(json);
        }
    } catch (...) {
        new Notification(tr("The data you entered is incorrect"), parentWidget());
    }
}

void Authorization::initializationPacket(const packets::Initialization &packet)
{
    if (packet.actualAppVersion > QApplication::applicationVersion().toInt()) {
        Q_EMIT update();
        return;
    }

    QSettings settings;
    settings.setValue("nickname", ui->inputNickname->text());
    settings.setValue("password", ui->inputPassword->text());

    Q_EMIT initialization(packet);
}

void Authorization::signinButtonClicked()
{
    const auto username = ui->inputNickname->text().toStdString();
    const auto password = ui->inputPassword->text().toStdString();
    const auto hwid = utils::hwid();

    Query::init(username, password, hwid);
    Query::send(client_, std::string(scoped_protected_string("initialization")));
}
