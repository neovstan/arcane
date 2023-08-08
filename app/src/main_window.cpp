#include "main_window.h"

#include <QMouseEvent>
#include <QPropertyAnimation>

#include "ui_main_window.h"

using namespace arcane::app;

MainWindow::MainWindow() : QMainWindow(), ui(new Ui::MainWindow), allowed_(false), minimized_(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    ui->setupUi(this);

    connect(ui->buttonClose, &QPushButton::clicked, this, &MainWindow::closeButtonClicked);
    connect(ui->buttonMinimize, &QPushButton::clicked, this, &MainWindow::minimizeButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeButtonClicked()
{
    animatedlyClose();
}

void MainWindow::minimizeButtonClicked()
{
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() != QEvent::WindowStateChange)
        return;

    const auto currentEvent = dynamic_cast<QWindowStateChangeEvent *>(event);
    const auto oldState = currentEvent->oldState();
    const auto state = windowState();

    if (state == Qt::WindowMinimized && !minimized_) {
        setWindowState(currentEvent->oldState());
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
    anim->start();
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
    anim->start();
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
    anim->start();
}
