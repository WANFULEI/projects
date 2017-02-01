
#include <QtGui/QApplication>
#include "dem/dem.h"
#include "QDebug"
#include <iostream>
using namespace std;
#include <QFile>
#include "QtGui/QPixmap"
#include "QThread"
#include <windows.h>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	Dem dem("C:\\OSGeo4W\\data\\World_e-Atlas-UCSD_SRTM30-plus_v8_Hillshading.tiff");
	dem.open();
	cout << endl;
	cout << dem.getHeight(138.01, 35.05) << endl;

	Dem dem1("C:\\OSGeo4W\\data\\World_e-Atlas-UCSD_SRTM30-plus_v8.tif");
	dem1.open();
	cout << endl;
	cout << dem1.getHeight(95.83, 35.73);

	auto i = LoadLibraryA("C:\\OSGeo4W\\apps\\qgis\\bin\\qgis_gui.dll");

// 	auto datas = dem.getHeight(86, 86.25, 27.2, 27.5);
// 	for(int i=0; i<datas.size(); ++i){
// 		for(int j=0; j<datas[i].size(); ++j){
// 			cout << datas[i][j] << ",";
// 		}
// 		cout << endl;
// 	}
	return a.exec();
}
