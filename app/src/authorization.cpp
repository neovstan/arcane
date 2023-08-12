#include "authorization.h"

#include <QTimer>
#include <QSettings>
#include <QCryptographicHash>
#include <QGraphicsOpacityEffect>

#include <infoware/infoware.hpp>
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
        const auto &id = json["id"];

        if (id == std::string(scoped_protected_string("initialization"))) {
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
    const auto password = QCryptographicHash::hash(ui->inputPassword->text().toLocal8Bit(),
                                                   QCryptographicHash::Sha256)
                                  .toHex()
                                  .toStdString();
    const auto hwid = getHwid();

    Query::init(username, password, hwid);
    Query::send(client_, std::string(scoped_protected_string("initialization")));
}

std::string Authorization::getHwid()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << iware::system::OS_info().full_name.c_str();
    stream << iware::cpu::model_name().c_str();
    stream << iware::system::memory().physical_total;

    for (const auto &gpu : iware::gpu::device_properties()) {
        stream << gpu.name.c_str();
        stream << gpu.memory_size;
    }

    const auto hash = QCryptographicHash::hash(data, QCryptographicHash::Sha256);
    return hash.toHex().toStdString();
}
