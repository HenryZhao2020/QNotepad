#include "SingleApplication"
#include "MainWindow.h"
#include "Attr.h"
#include "FileUtil.h"

#include <QTranslator>
#include <QLibraryInfo>
#include <QFontDatabase>
#include <QTimer>

/**
 * @brief Processes communication between different program instances.
 * @param message Sent message to the primary instance.
 */
void processMessage(int, QByteArray message) {
    if (message == "New") {
        MainWindow::newWindow();
    } else if (message.startsWith("Open")) {
        MainWindow::open(message.mid(5));
    }
}

/**
 * @brief Sets up the program, then starts the game.
 * @param argc Number of arguments.
 * @param argv List of arguments.
 * @return Execution code.
 */
int main(int argc, char **argv) {
    SingleApplication app{argc, argv, true};

    // Read the arguments
    if (app.isSecondary()) {
        // If no arguments are given, open a new window
        if (argc == 1) {
            app.sendMessage("New");
        // Otherwise, open the files given by the arguments
        } else {
            for (int i = 1; i < argc; ++i) {
                app.sendMessage("Open " + QByteArray{argv[i]});
            }
        }
        // Exit the secondary instance
        return 0;
    }

    // Load saved attributes if possible
    Attr::get().load();

    // Update the display language
    QTranslator baseTrans, appTrans;
    if (Attr::get().lang != Lang::ENGLISH) {
        const QString basePath{QLibraryInfo::path(QLibraryInfo::TranslationsPath)};
        const QString code{LangUtil::getLangCode(Attr::get().lang)};

        if (baseTrans.load("qtbase_" + code, basePath)) {
            app.installTranslator(&baseTrans);
        }
        if (appTrans.load(":/i18n/TTT_" + code)) {
            app.installTranslator(&appTrans);
        }
    }

    // For displaying monospaced characters
    QFontDatabase::addApplicationFont(":/fonts/CascadiaCode.ttf");

    // Style application
    app.setStyle("Fusion");
    app.setStyleSheet(FileUtil::readAll(":/conf/Styles.qss"));

    // Save attributes on quit
    QObject::connect(&app, &QApplication::aboutToQuit, &app, [] {
        Attr::get().save();
    });

    QObject::connect(&app, &SingleApplication::receivedMessage,
                     &app, &processMessage);

    // Detect theme change every second
    QTimer themeTimer;
    QObject::connect(&themeTimer, &QTimer::timeout, &app, [] {
        // Ensure light/dark mode update
        QApplication::setStyle("Fusion");
        MainWindow::updateEditorFont();
    });
    themeTimer.start(1000);

    // If no arguments are given, open a new window
    if (argc == 1) {
        MainWindow::newWindow();
    // Otherwise, open the files given by the arguments
    } else {
        for (int i = 1; i < argc; ++i) {
            MainWindow::open(argv[i]);
        }
    }

    return app.exec();
}
