#include <iostream>

#include "mainwindow.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "Application.hpp"
#include "loginform.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "tcp-chat-client_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }
    Application application;

    //Must add this line otherwise the socket is not closed!
    QObject::connect(&a, &QCoreApplication::aboutToQuit, &application, &Application::disconnectFromServer);
    {
        LoginForm loginForm(&application);
        if (loginForm.exec() != QDialog::Accepted)
            return 0;
    }
    MainWindow window(&application);
    window.show();
    return a.exec();
}
