#include <QApplication>

#include "textedit.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("officestyle");
    app.setApplicationName("SDI Example");
    app.setOrganizationName("Qtitan");
    TextEdit *mainWin = new TextEdit;
    mainWin->show();
    return app.exec();
}
