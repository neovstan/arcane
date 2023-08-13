#include "notification.h"

#include <QLabel>
#include <QBoxLayout>
#include <QPushButton>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

using namespace arcane::app;

Notification::Notification(const QString &text, QWidget *parent, bool noButton)
    : QWidget(parent), defaultAnimationDuration(200), opacityEffect_(new QGraphicsOpacityEffect)
{
    move(0, 40);
    setFixedSize(parent->size() - QSize(0, 40));
    setGraphicsEffect(opacityEffect_);

    const auto shadow = new QWidget(this);
    shadow->setStyleSheet("background-color: rgba(0, 0, 0, 150)");
    shadow->setFixedSize(size());

    QFont font(":/fonts/Roboto-Bold.ttf");
    font.setPixelSize(16);

    const auto window = new QWidget(this);
    window->setStyleSheet(R"(
border-radius: 10px;
background-color: qlineargradient(spread:pad, x1:0, y1:1, x2:1, y2:0, stop:0 rgba(18, 18, 18, 255), stop:1 rgba(42, 42, 42, 255));
)");

    QFontMetrics metrics(font);
    const auto windowSize = metrics.boundingRect(text).size() + QSize(23, noButton ? 23 : 45);
    window->setFixedSize(windowSize);

    const auto pos = (size() - window->size()) / 2;
    window->move(pos.width(), pos.height() - 40);

    const auto label = new QLabel(text, window);
    label->setStyleSheet(R"(
font-family: 'Roboto';
font-size: 16px;
color: white;
background-color: 0;
)");
    label->move(10, 10);

    if (!noButton) {
        const auto buttonWidget = new QWidget(window);
        buttonWidget->setStyleSheet("background-color: 0");
        buttonWidget->move(0, 30);
        buttonWidget->setFixedWidth(window->width());

        const auto closeButton = new QPushButton("OK");
        closeButton->setStyleSheet(R"(
font-family: 'Roboto';
font-size: 14px;
color: grey;
)");
        closeButton->setMaximumWidth(30);
        connect(closeButton, &QPushButton::clicked, this,
                [this]() { animatedlyChangeVisibility(false, defaultAnimationDuration); });

        const auto horizontalLayout = new QHBoxLayout(buttonWidget);
        horizontalLayout->addWidget(closeButton);
    }

    animatedlyChangeVisibility(true, defaultAnimationDuration);
}

void Notification::animatedlyChangeVisibility(bool show, int msecs)
{
    const auto anim = new QPropertyAnimation(opacityEffect_, "opacity");

    connect(anim, &QPropertyAnimation::finished, this, [this, show]() {
        if (!show) {
            hide();
            deleteLater();
        }
    });

    anim->setDuration(msecs);
    anim->setStartValue(show ? 0.0 : 1.0);
    anim->setEndValue(show ? 1.0 : 0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    if (show)
        this->show();

    setEnabled(show);
}
