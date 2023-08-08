#ifndef ARCANE_APP_MAIN_WINDOW_H
#define ARCANE_APP_MAIN_WINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

namespace arcane {
namespace app {
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow() override;

private slots:
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

private:
    Ui::MainWindow *ui;

private:
    bool allowed_;
    bool minimized_;
    QPoint mousePos_;
};
} // namespace app
} // namespace arcane

#endif // ARCANE_APP_MAIN_WINDOW_H
