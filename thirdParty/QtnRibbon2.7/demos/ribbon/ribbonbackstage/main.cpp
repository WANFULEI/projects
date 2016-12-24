#include <QtGui/QApplication>

#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setStyle("ribbonstyle");

    app.setApplicationName("Qtitan Ribbon Controls Sample");
    app.setOrganizationName("Qtitan");

    MainWindow w;
    w.show();
    return app.exec();
}
