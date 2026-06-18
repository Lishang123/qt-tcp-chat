#include "dialog.h"

#include <QApplication>

#include "Application.hpp"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    qInfo("Starting dialog");
    w.show();
    return QCoreApplication::exec();
}
