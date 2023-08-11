#include "main_window.h"

#include <QSvgWidget>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

#include <protected_string/protected_string.h>

#include "client.h"
#include "authorization.h"
#include "home.h"

#include "packets/initialization.hpp"

#include "ui_main_window.h"

using namespace arcane::app;
#include <QDebug>
MainWindow::MainWindow()
    : QMainWindow(),
      defaultAnimationDuration(200),
      ui(new Ui::MainWindow),
      client_(new Client(QString(scoped_protected_string("localhost")), 4289, this)),
      authorization_(new Authorization(client_, this)),
      loading_(new QSvgWidget(":/animations/loading.svg", this)),
      loadingOpacityEffect_(new QGraphicsOpacityEffect),
      home_(new Home(client_, this)),
      allowed_(false),
      minimized_(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
    ui->setupUi(this);

    loading_->setFixedSize(150, 150);
    const auto loadingPos = (size() - loading_->size()) / 2;
    loading_->move(loadingPos.width(), loadingPos.height());
    loading_->setGraphicsEffect(loadingOpacityEffect_);

    authorization_->getOpacityEffect()->setOpacity(1.0);
    home_->getOpacityEffect()->setOpacity(0.0);
    loadingOpacityEffect_->setOpacity(0.0);

    home_->hide();
    loading_->hide();

    connect(ui->buttonClose, &QPushButton::clicked, this, &MainWindow::closeButtonClicked);
    connect(ui->buttonMinimize, &QPushButton::clicked, this, &MainWindow::minimizeButtonClicked);

    connect(authorization_, &Authorization::initialization, this, &MainWindow::initialization);
    connect(home_, &Home::load, this, &MainWindow::load);
    connect(home_, &Home::loadFinished, this, &MainWindow::loadFinished);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialization(const packets::Initialization &packet)
{
    home_->setDaysRemaining(packet.daysRemaining);
    home_->setNickname(packet.username.c_str());

    animatedlyChangeWidgetVisibility(authorization_, authorization_->getOpacityEffect(), false,
                                     defaultAnimationDuration);
    animatedlyChangeWidgetVisibility(home_, home_->getOpacityEffect(), true,
                                     defaultAnimationDuration);
}

void MainWindow::load()
{
    animatedlyChangeWidgetVisibility(home_, home_->getOpacityEffect(), false,
                                     defaultAnimationDuration);

    animatedlyChangeWidgetVisibility(loading_, loadingOpacityEffect_, true,
                                     defaultAnimationDuration * 5);
}

void MainWindow::loadFinished()
{
    animatedlyChangeWidgetVisibility(home_, home_->getOpacityEffect(), true,
                                     defaultAnimationDuration);

    animatedlyChangeWidgetVisibility(loading_, loadingOpacityEffect_, false,
                                     defaultAnimationDuration);
}

void MainWindow::closeButtonClicked()
{
    animatedlyClose();
}

void MainWindow::minimizeButtonClicked()
{
    animatedlyMinimize();
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() != QEvent::WindowStateChange)
        return;

    const auto currentEvent = dynamic_cast<QWindowStateChangeEvent *>(event);
    const auto oldState = currentEvent->oldState();
    const auto state = windowState();

    if (state == Qt::WindowMinimized && !minimized_) {
        setWindowState(oldState);
        animatedlyMinimize();
        event->accept();
    } else if (oldState == Qt::WindowMinimized && minimized_) {
        animatedlyMaximize();
        event->accept();
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    setFocus();

    const auto area = ui->titleBar->size();
    const auto newPos = event->pos();

    if (event->button() == Qt::LeftButton && (newPos.x() <= area.width())
        && (newPos.y() <= area.height())) {
        mousePos_ = event->globalPos();
        allowed_ = true;
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (!allowed_)
        return;

    const auto globalPos = event->globalPos();
    const auto delta = globalPos - mousePos_;

    move(pos() + delta);
    mousePos_ = globalPos;

    event->accept();
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    allowed_ = false;
    event->accept();
}

void MainWindow::animatedlyClose()
{
    setDisabled(true);

    const auto anim = new QPropertyAnimation(this, "windowOpacity");

    connect(anim, &QPropertyAnimation::finished, this, [this]() { QApplication::exit(); });

    anim->setDuration(100);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animatedlyMinimize()
{
    setDisabled(true);

    minimized_ = true;

    const auto anim = new QPropertyAnimation(this, "windowOpacity");

    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        setWindowState(Qt::WindowMinimized);
        setDisabled(false);
    });

    anim->setDuration(100);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animatedlyMaximize()
{
    setDisabled(true);

    const auto anim = new QPropertyAnimation(this, "windowOpacity");

    connect(anim, &QPropertyAnimation::finished, this, [this]() {
        setDisabled(false);
        minimized_ = false;
    });

    anim->setDuration(100);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::animatedlyChangeWidgetVisibility(QWidget *widget, QGraphicsOpacityEffect *opacity,
                                                  bool show, int msecs)
{
    const auto anim = new QPropertyAnimation(opacity, "opacity");

    connect(anim, &QPropertyAnimation::finished, this, [widget, show, &opacity]() {
        if (!show) {
            widget->hide();
        }
    });

    anim->setDuration(msecs);
    anim->setStartValue(show ? 0.0 : 1.0);
    anim->setEndValue(show ? 1.0 : 0.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    if (show)
        widget->show();

    widget->setEnabled(show);
}
