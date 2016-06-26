#include "tt8.h"
#include <QtGui/QApplication>
#include "../../map/map_wgt/map_widget.h"
#include "QTextCodec"
#include "QDir"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "cpl_conv.h" // for CPLMalloc()
#include "../../map/map_wgt/raster_layer.h"
#include "../../control/layer_widget/tools_layer.h"
#include "../../map/map_wgt/vector_layer.h"
#include "qdebug"

#include <log4cpp/Category.hh>
#include <exception>
#include "../../base/vincenty/vincenty.h"

using namespace map_wgt;

int main(int argc, char *argv[])
{
	QTextCodec *codec = QTextCodec::codecForName("System");  
	QTextCodec::setCodecForCStrings(codec);  
	QTextCodec::setCodecForLocale(codec);  
	QTextCodec::setCodecForTr(codec); 

	log4cpp::initialize_logger("test.log","test");
	
	GDALAllRegister();

	QApplication a(argc, argv);
	QDir::setCurrent(a.applicationDirPath());

	map_widget map;
	
 	raster_layer *layer = new raster_layer;
 	layer->load("data\\NE2_HR_LC_SR_W_DR.tif");
 	map.attach(layer);
 	vector_layer * vlayer = new vector_layer;
 	vlayer->load("data\\ne_10m_admin_0_countries.shp");
 	map.attach(vlayer);

//	map.select_tool(layerwidget::Pan);
	layer_wgt::pan_layer *pan = new layer_wgt::pan_layer;
	map.attach(pan);
	map.set_active_layer(pan);

	map.show();
	map.set_window(-18,18,-9,9);
//	map.redraw();

	QImage *p = new QImage(-1, -1, QImage::Format_ARGB32);

	return a.exec();
}
