#include <QApplication>
#include <QFontDatabase>

#include "main_window.h"
#include <QDebug>
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("arcane");
    app.setOrganizationDomain("arcane.su");
    app.setApplicationName("app");
    app.setApplicationVersion("3");

    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Helvetica-Bold.ttf");
    
    arcane::app::MainWindow w;
    w.show();

    return app.exec();
}
