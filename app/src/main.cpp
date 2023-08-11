#include <QApplication>
#include <QFontDatabase>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("arcane");

    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Helvetica-Bold.ttf");

    arcane::app::MainWindow w;
    w.show();

    return app.exec();
}
