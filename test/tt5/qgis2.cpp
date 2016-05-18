#include "qgis2.h"
#include "QMessageBox"
#include <QgsPluginLayer.h>
#include <QgsMapLayerLegend.h>
#include <qgspainteffect.h>

qgis2::qgis2(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	QString myPluginsDir = "G:/projects/gis/QGIS/build/output/plugins/RelWithDebInfo";
	QgsProviderRegistry::instance(myPluginsDir);
	//创建地图画布
	mpMapCanvas= new QgsMapCanvas(0,0);
	mpMapCanvas->freeze(false);
	mpMapCanvas->enableAntiAliasing(true);
	mpMapCanvas->setCanvasColor(QColor(255,255,255));
	mpMapCanvas->useImageToRender(false);
	mpMapCanvas->setVisible(true);
	mpMapCanvas->refresh();
	mpMapCanvas->show();
	mpMapCanvas->setFocus();


	//将窗口部件布局
	mpMapLayout = new QVBoxLayout();
	mpMapLayout->addWidget(mpMapCanvas);
	ui.frameMap->setLayout(mpMapLayout);
	setCentralWidget(ui.frameMap);

	//创建Action行为
	connect(ui.mpActionPan,SIGNAL(triggered()),this,SLOT(panMode()));
	connect(ui.mpActionZoomIn,SIGNAL(triggered()),this,SLOT(zoomInMode()));
	connect(ui.mpActionZoomOut,SIGNAL(triggered()),this,SLOT(zoomOutMode()));
	connect(ui.mpActionAddLayer,SIGNAL(triggered()),this,SLOT(addLayer()));

	//创建工具条功能
	mpMapToolBar=addToolBar(tr("File"));

	mpMapToolBar->addAction(ui.mpActionAddLayer);
	mpMapToolBar->addAction(ui.mpActionPan);
	mpMapToolBar->addAction(ui.mpActionZoomIn);
	mpMapToolBar->addAction(ui.mpActionZoomOut);
	//创建maptool功能
	mpPanTool= new QgsMapToolPan(mpMapCanvas);
	mpPanTool->setAction(ui.mpActionPan);
	mpZoomInTool = new QgsMapToolZoom(mpMapCanvas,FALSE);
	mpZoomInTool->setAction(ui.mpActionZoomIn);
	mpZoomOutTool = new QgsMapToolZoom(mpMapCanvas,TRUE);
	mpZoomOutTool->setAction(ui.mpActionZoomOut);



}

qgis2::~qgis2()
{
	delete mpZoomOutTool;
	delete mpZoomInTool;
	delete mpPanTool;
	delete mpMapToolBar;
	delete mpMapCanvas;
	delete mpMapLayout;

}
void qgis2::panMode()
{
	mpMapCanvas->setMapTool(mpPanTool);
}
void qgis2::zoomInMode()
{
	mpMapCanvas->setMapTool(mpZoomInTool);
}
void qgis2::zoomOutMode()
{
	mpMapCanvas->setMapTool(mpZoomOutTool);
}
void qgis2::addLayer()
{
	QList<QgsMapCanvasLayer> layer_set;
	QFileInfo myRasterFileInfo("F:/earth-map/NE2_HR_LC_SR_W_DR/NE2_HR_LC_SR_W_DR.tif");
	//QFileInfo myRasterFileInfo("F:/earth-map/global/world/openstreet.xml");
	//QFileInfo myRasterFileInfo("F:/earth-map/global/world/tms.xml");
	QgsRasterLayer *raster_layer = new QgsRasterLayer(myRasterFileInfo.filePath(),myRasterFileInfo.completeBaseName());
	if (raster_layer->isValid())
	{
		QMessageBox::information(0,QString(tr("加载图层")),QString(tr("加载成功")),QMessageBox::Yes|QMessageBox::No);

		//raster_layer->setDrawingStyle(QgsRaster::SingleBandPseudoColor);
		//raster_layer->setColorShadingAlgorithm(QgsRaster::PseudoColorShader);
		//raster_layer->setContrastEnhancement(QgsContrastEnhancement::StretchToMinimumMaximum);

		QgsMapLayerRegistry::instance()->addMapLayer(raster_layer,1);

		mpMapCanvas->setExtent(raster_layer->extent());
		layer_set.append(QgsMapCanvasLayer(raster_layer));
		mpMapCanvas->setLayerSet(layer_set);
	}
	else
	{
		QMessageBox::information(0,QString(tr("加载图层")),QString(tr("加载失败")),QMessageBox::Yes|QMessageBox::No	   );
	}

	//读取矢量数据
 	QgsVectorLayer * file_vector_layer = new QgsVectorLayer("F:/earth-map/CHN_adm/CHN_adm1.shp","","ogr");
 	if (file_vector_layer->isValid())
 	{
		QGis::GeometryType geometry_type = file_vector_layer->geometryType();
		QgsSimpleFillSymbolLayerV2 * fill_layer1 = new QgsSimpleFillSymbolLayerV2;
		fill_layer1->setBorderColor(Qt::yellow);
		fill_layer1->setFillColor(QColor(0,255,0,60));
		//fill_layer1->setFillColor(Qt::transparent);
		QgsSymbolLayerV2List layer_list;
		layer_list << fill_layer1;
		QgsFillSymbolV2 * symbol = new QgsFillSymbolV2(layer_list);

		QgsVectorSimplifyMethod simplier;
		simplier.setForceLocalOptimization(0);
		simplier.setSimplifyHints(QgsVectorSimplifyMethod::FullSimplification);
		simplier.setThreshold(2);
		simplier.setMaximumScale(10 * 10000);
		file_vector_layer->setSimplifyMethod(simplier);

 		QgsSingleSymbolRendererV2 *myRenderer = new QgsSingleSymbolRendererV2(symbol);
 		//myLayer->setRenderer(myRenderer);

		myRenderer->paintEffect()->setEnabled(true);

 		file_vector_layer->setRendererV2(myRenderer);
 		//设置画布的extent
 		//mpMapCanvas->setExtent(file_vector_layer->extent());

 		layer_set.insert(0,file_vector_layer);
 		mpMapCanvas->setLayerSet(layer_set);

 		QgsMapLayerRegistry::instance()->addMapLayer(file_vector_layer);
 	}

	QgsVectorLayer * vector_layer = new QgsVectorLayer("point?crs=EPSG:4326&memoryid={e38fe2de-bcae-4ce8-91eb-8d8aa09b2a87}","test","memory"); //初始化矢量图层;
	if (vector_layer->isValid())
	{
		vector_layer->startEditing();

		//设置点标号
		//QgsLineSymbolV2 *pSym1 = new QgsLineSymbolV2;

		QgsStringMap fields;
		fields["line_color"] = "0,255,0,128";
		fields["line_width"] = QString::number(2);
		fields["line_style"] = "dash";
		fields["line_width_unit"] = "Pixel";
		fields["joinstyle"] = "RoundJoin";

		QgsSimpleLineSymbolLayerV2 * layer1 = new QgsSimpleLineSymbolLayerV2;
		layer1->setColor(Qt::black);
		layer1->setWidth(4);
		layer1->setOutputUnit(QgsSymbolV2::Pixel);
		layer1->setPenJoinStyle(Qt::RoundJoin);
		layer1->setPenCapStyle(Qt::RoundCap);

		QgsSimpleLineSymbolLayerV2 * layer2 = new QgsSimpleLineSymbolLayerV2;
		layer2->setColor(Qt::red);
		layer2->setWidth(2);
		layer2->setOutputUnit(QgsSymbolV2::Pixel);
		layer2->setPenJoinStyle(Qt::RoundJoin);
		layer2->setPenCapStyle(Qt::RoundCap);

		QgsSymbolLayerV2List layer_list;
		layer_list << layer1 << layer2;

		QgsLineSymbolV2 * symbol = new QgsLineSymbolV2(layer_list);

		//QgsLineSymbolV2 *pSym1 = QgsLineSymbolV2::createSimple(fields);
		//设置点的外轮廓线的颜色和线宽

		//pSym1->setColor(Qt::red);
		//pSym1->setSize(3//
		//pSym1->setWidth(0.3);
		//pSym1->set

		//pSym1->setLineWidth(0.3);

		//设置充填颜色和充填样式

		//pSym1->setFillColor(QColor(228,108,119));
		//pSym1->setFillStyle(Qt::SolidPattern);
		//设置符号大小

		//pSym1->setPointSize(1.0);
		//该方法决定了标号的形状
		//pSym1->setNamedPointSymbol("svg:education_school.svg");
		//pSym1->setNamedPointSymbol("hard:circle");
		//设置绘制的符号

		QgsSingleSymbolRendererV2 *render = new QgsSingleSymbolRendererV2(symbol);
		//应用于图层 

		//render->paintEffect()->setEnabled(true);
		

		vector_layer->setRendererV2(render);

		for(int index = 0; index < 1 * 10000; index ++){
			QgsPoint point;

			double xmin = -180;
			double xmax = 180;
			double ymin = -90;
			double ymax = 90;

			point.setX(xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX);
			point.setY(ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX);

			QgsPolyline polyline;
			polyline << point;
			point.setX(point.x() + 1);
			point.setY(point.y());
			polyline << point;
			point.setX(point.x() - .5);
			point.setY(point.y() + 1);
			polyline << point;

			QgsFeature feature;
			QgsGeometry *geo = QgsGeometry::fromPolyline(polyline);
			feature.setGeometry(geo);
			feature.setFeatureId(QgsFeatureId(index));
			vector_layer->addFeature(feature);
		}

		//设置图层LOD
		vector_layer->setScaleBasedVisibility(0);
		vector_layer->setMinimumScale(100 * 10000);
		vector_layer->setMaximumScale(800 * 10000);

		//添加到图层管理
		QgsMapLayerRegistry::instance()->addMapLayer( vector_layer, true );

		//添加到渲染图层列表
		layer_set.insert(0,vector_layer);
		mpMapCanvas->setLayerSet(layer_set);

		//刷新地图
		//mpMapCanvas->refresh();
	}

}
