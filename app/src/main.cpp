#include <QApplication>
#include <QFontDatabase>
#include <QProcess>

#include <protected_string/protected_string.h>

#include "main_window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    app.setOrganizationName("arcane");
    app.setOrganizationDomain("arcane.su");
    app.setApplicationName("app");
    app.setApplicationVersion("5");

    QFontDatabase::addApplicationFont(":/fonts/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/Helvetica-Bold.ttf");

    arcane::app::MainWindow w;
    w.show();

    const auto result = app.exec();

    const auto path = QApplication::applicationFilePath();
    const auto name = path.mid(path.lastIndexOf("/") + 1);

    const auto process = new QProcess;
    process->setProgram(scoped_protected_qstring("updater.exe"));
    process->setArguments(QStringList() << "--clear" << name);
    process->startDetached();

    return result;
}
