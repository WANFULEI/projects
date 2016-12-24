#include <QtGui/QApplication>
#include <QtnRibbonStyle.h>

#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication app( argc, argv );
    app.setStyle( new Qtitan::RibbonStyle() );
    app.setApplicationName( "Qtitan Ribbon Sample" );
    app.setOrganizationName("Qtitan");

    MainWindow w;
    w.show();
    return app.exec();
}
