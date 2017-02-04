#include "LayerInterfaceImpl.h"
#include "component/GlobalInstance.h"
#include "osgEarthDrivers/gdal/GDALOptions"
#include "osgEarth/ImageLayer"
#include "osgEarthSymbology/Style"
#include "gdal_priv.h"
#include "ogrsf_frmts.h"
#include "osgEarthDrivers/feature_ogr/OGRFeatureOptions"
#include "osgEarthDrivers/model_feature_geom/FeatureGeomModelOptions"
#include <string>
using namespace std;

using namespace osgEarth;
using namespace osgEarth::Drivers;
using namespace osgEarth::Symbology;

LayerInterfaceImpl::LayerInterfaceImpl(void)
{
}


LayerInterfaceImpl::~LayerInterfaceImpl(void)
{
}


int LayerInterfaceImpl::addRasterLayer(QString layerName, QString filePath)
{
	GDALOptions gdal;
	gdal.url() = filePath.toStdString();
	ImageLayer *layer = new ImageLayer(layerName.toStdString(), gdal);
	if(!layer->open().isOK()){
		delete layer;
		return 0;
	}
	global->getMap3D()->addImageLayer(layer);
	return (int)layer;
}

int LayerInterfaceImpl::addVectorLayer(QString layerName, QString filePath)
{
	Style style;
	GDALDataset *poDS = (GDALDataset *)GDALOpenEx( filePath.toStdString().c_str(), GDAL_OF_VECTOR, 0, 0, 0 );
	if(poDS == 0) return 0;
	OGRLayer *layer = poDS->GetLayer(0);
	if(layer == 0) return 0;
	layer->ResetReading();
	OGRFeature *feature = layer->GetNextFeature();
	if(feature == 0) return 0;
	if(wkbFlatten(feature->GetGeometryRef()->getGeometryType()) == wkbPoint){
		// 		PointSymbol *pointSymbol = style.getOrCreateSymbol<PointSymbol>();
		// 		pointSymbol->fill()->color() = Color::Red;
		// 		pointSymbol->size() = 12;

		osg::Image *image = osgDB::readImageFile("c:\\osgeo4w\\images\\flags\\vi.png");
		IconSymbol *markerSymbol = style.getOrCreateSymbol<osgEarth::Symbology::IconSymbol>();
		//markerSymbol->setImage(image);
		markerSymbol->placement() = IconSymbol::PLACEMENT_VERTEX;
		markerSymbol->url() = StringExpression("c:\\osgeo4w\\images\\flags\\vi.png");
		//		osg::Node *node = osgDB::readNodeFile("D:\\osg-file-path\\cow.osgt");
		// 		markerSymbol->setModel(node);
		// 		markerSymbol->alignment() = MarkerSymbol::ALIGN_CENTER_CENTER;
		// 		markerSymbol->scale() = 1000;
	}else{
		LineSymbol *lineSymbol = style.getOrCreateSymbol<LineSymbol>();
		lineSymbol->stroke()->color() = Color::Yellow;
		lineSymbol->stroke()->width() = 1.0f;
	}
	OGRFeature::DestroyFeature(feature);
	GDALClose( poDS );

	TextSymbol* text = style.getOrCreateSymbol<TextSymbol>();
	text->font() = "ºÚÌå";
	text->encoding()= TextSymbol::ENCODING_UTF8;
	text->content() = StringExpression( "[name]" );
	//text->priority() = NumericExpression( "[pop_cntry]" );
	//text->removeDuplicateLabels() = true;
	text->size() = 12;
	text->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
	text->fill()->color() = Color::White;
	text->halo()->color() = Color::Black;

	OGRFeatureOptions featureOptions;
	featureOptions.url() = filePath.toStdString();

	FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = featureOptions;
	geomOptions.styles() = new StyleSheet();
	geomOptions.styles()->addStyle(style);

	ModelLayer *layer2 = new ModelLayer(layerName.toStdString(), geomOptions);
	global->getMap3D()->addModelLayer(layer2);
	return (int)layer2;
}

int LayerInterfaceImpl::addElevationLayer(QString layerName, QString filePath)
{
	GDALOptions gdal;
	gdal.url() = filePath.toStdString();
	ElevationLayer *layer = new ElevationLayer(layerName.toStdString(), gdal);
	string name = layer->getName();
	if(!layer->open().isOK()){
		delete layer;
		return 0;
	}
	global->getMap3D()->addElevationLayer(layer);
	return (int)layer;
}

bool LayerInterfaceImpl::createLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool LayerInterfaceImpl::removeLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}
