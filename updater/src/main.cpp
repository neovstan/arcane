#include <QCoreApplication>
#include <QFile>
#include <QProcess>
#include <QThread>
#include <QDirIterator>
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
                        { { "new", "new-file" }, "Sets a new file <file>.", "file" },
                        { { "cl", "clear" }, "Clears a file from prefetch <file>.", "file" } });

    parser.process(app);

    const auto oldApp = parser.value("old-file");
    const auto newApp = parser.value("new-file");
    const auto clear = parser.value("clear");

    if (!clear.isEmpty()) {
        QThread::sleep(2);

        QDirIterator it(R"(C:\Windows\Prefetch)",
                        QStringList() << (clear.toUpper() + "*") << "UPDATER.EXE*", QDir::Files,
                        QDirIterator::Subdirectories);

        while (it.hasNext()) {
            const auto file = it.next();
            QFile::remove(file);
        }

        return 0;
    }

    while (!QFile::remove(oldApp))
        continue;

    while (!QFile::rename(newApp, oldApp))
        continue;

    const auto process = new QProcess;
    process->setProgram(oldApp);
    process->startDetached();
}
