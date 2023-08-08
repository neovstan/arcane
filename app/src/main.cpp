#include <QApplication>
#include <QFontDatabase>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName("arcane");
    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");

    arcane::app::MainWindow w;
    w.show();

    return app.exec();
}
