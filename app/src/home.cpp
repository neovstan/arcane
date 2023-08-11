#include "home.h"

#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include <winapi_utils/winapi_utils.h>
#include <protected_string/protected_string.h>

#include "query.h"
#include "client.h"
#include "manual_map.h"

#include "packets/load.hpp"

#include "ui_home.h"

using namespace arcane::app;

Home::Home(Client *client, QWidget *parent)
    : QWidget(parent),
      defaultAnimationDuration(200),
      ui(new Ui::Home),
      opacityEffect_(new QGraphicsOpacityEffect),
      client_(client)
{
    ui->setupUi(this);

    setGraphicsEffect(opacityEffect_);

    connect(ui->buttonLoad, &QPushButton::clicked, this, &Home::loadButtonClicked);
    connect(client_, &Client::read, this, &Home::packetHandler);
}

Home::~Home()
{
    delete ui;
}

void Home::setDaysRemaining(int days)
{
    const auto text = ui->labelDaysRemaining;
    text->setText(text->text().arg(days));
}

void Home::setNickname(const QString &nickname)
{
    ui->labelNickname->setText(nickname);
}

QGraphicsOpacityEffect *Home::getOpacityEffect() const
{
    return opacityEffect_;
}

void Home::packetHandler(const QString &answer)
{
    try {
        const auto json = nlohmann::json::parse(answer.toStdString());
        const auto &id = json["id"];

        if (id == std::string(scoped_protected_string("load"))) {
            loadPacket(json);
        }
    } catch (...) {
    }

    Q_EMIT loadFinished();
}

void Home::loadPacket(const packets::Load &packet)
{
    const auto dll = QByteArray::fromHex(packet.dll.c_str());

    try {
        const auto handle = getGameProcessHandle();
        if (!handle) {
            return;
        }

        ManualMap injection(handle, dll);
        injection.give();
    } catch (const ManualMap::Exception &e) {
    }
}

void Home::loadButtonClicked()
{
    Q_EMIT load();
    Query::send(client_, "load");
}

void *Home::getGameProcessHandle()
{
    return winapi_utils::find_process_handle_by_pattern(
            0x7D34F0,
            "\x56\x8B\x74\x24\x08\x8B\x46\x40\x85\xC0\x74\x0B\x8B\x4C\x24\x0C"
            "\x51"
            "\x56\xFF\xD0\x83\xC4\x08\x6A\x01\x56\xE8\xE9\x2A\x05\x00\x5E");
}
