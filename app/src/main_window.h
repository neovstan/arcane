#ifndef ARCANE_APP_MAIN_WINDOW_H
#define ARCANE_APP_MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QSvgWidget;
class QGraphicsOpacityEffect;

namespace arcane::app {
class Client;
class Authorization;
class Home;

namespace packets {
class Initialization;
class Update;
} // namespace packets

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

private slots:
    void packetHandler(const QString &answer);
    void updatePacket(const packets::Update &packet);
    void update();
    void initialization(const packets::Initialization &packet);
    void load();
    void loadFinished();
    void closeButtonClicked();
    void minimizeButtonClicked();

private:
    void changeEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    void animatedlyClose();
    void animatedlyMinimize();
    void animatedlyMaximize();
    void animatedlyChangeWidgetVisibility(QWidget *widget, QGraphicsOpacityEffect *opacity,
                                          bool show, int msecs);

private:
    const int defaultAnimationDuration;

private:
    Ui::MainWindow *ui;

private:
    Client *client_;

private:
    Authorization *authorization_;
    Home *home_;
    QSvgWidget *loading_;
    QGraphicsOpacityEffect *loadingOpacityEffect_;

private:
    bool allowed_;
    bool minimized_;
    QPoint mousePos_;
};
} // namespace arcane::app

#endif // ARCANE_APP_MAIN_WINDOW_H
