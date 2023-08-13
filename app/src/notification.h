#ifndef ARCANE_APP_NOTIFICATION_H
#define ARCANE_APP_NOTIFICATION_H

#include <QWidget>

class QGraphicsOpacityEffect;

namespace arcane::app {
class Notification : public QWidget
{
    Q_OBJECT

public:
    explicit Notification(const QString &text, QWidget *parent = nullptr, bool noButton = false);

    void animatedlyChangeVisibility(bool show, int msecs);

private:
    const int defaultAnimationDuration;

private:
    QGraphicsOpacityEffect *opacityEffect_;
};
} // namespace arcane::app

#endif // ARCANE_APP_NOTIFICATION_H
