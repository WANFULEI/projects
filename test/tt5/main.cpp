#include "qgis2.h"
#include <QtGui/QApplication>
#include "windows.h"

class QgsApplication;
int MyMethod(QgsApplication & a)
{
	__try
	{
		return a.exec();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		int n = 0;
	}
}

int main(int argc, char *argv[])
{
	QTextCodec *codec = QTextCodec::codecForName("System");  
	QTextCodec::setCodecForCStrings(codec);  
	QTextCodec::setCodecForLocale(codec);  
	QTextCodec::setCodecForTr(codec);  


	QgsApplication a(argc, argv,TRUE);  
	qgis2 w;  
	w.show();  
	return MyMethod(a);

}
