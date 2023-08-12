#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QCommandLineParser>
#include <QCommandLineOption>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription("arcane updater");
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({ { { "old", "old-file" }, "Sets an old file <file>.", "file" },
                        { { "new", "new-file" }, "Sets a new file <file>.", "file" } });

    parser.process(app);

    const auto oldApp = parser.value("old-file");
    const auto newApp = parser.value("new-file");

    while (!QFile::remove(oldApp))
        continue;

    while (!QFile::rename(newApp, oldApp))
        continue;

    const auto process = new QProcess;
    process->setProgram(oldApp);
    process->startDetached();
}
