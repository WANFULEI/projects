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

LayerInterfaceImpl::LayerInterfaceImpl(void)
{
}


LayerInterfaceImpl::~LayerInterfaceImpl(void)
{
}


bool LayerInterfaceImpl::addRasterLayer(QString layerName, QString filePath)
{
	osgEarth::Drivers::GDALOptions gdal;
	gdal.url() = filePath.toStdString();
	osgEarth::ImageLayer *layer = new osgEarth::ImageLayer(layerName.toStdString(), gdal);
	if(!layer->open().isOK()){
		delete layer;
		return false;
	}
	global->getMap3D()->addImageLayer(layer);
	return true;
}

bool LayerInterfaceImpl::addVectorLayer(QString layerName, QString filePath)
{
	osgEarth::Symbology::Style style;
	GDALDataset *poDS = (GDALDataset *)GDALOpenEx( filePath.toStdString().c_str(), GDAL_OF_VECTOR, 0, 0, 0 );
	if(poDS == 0) return false;
	OGRLayer *layer = poDS->GetLayer(0);
	if(layer == 0) return false;
	layer->ResetReading();
	OGRFeature *feature = layer->GetNextFeature();
	if(feature == 0) return false;
	if(wkbFlatten(feature->GetGeometryRef()->getGeometryType()) == wkbPoint){
		// 		PointSymbol *pointSymbol = style.getOrCreateSymbol<PointSymbol>();
		// 		pointSymbol->fill()->color() = Color::Red;
		// 		pointSymbol->size() = 12;

		osg::Image *image = osgDB::readImageFile("c:\\osgeo4w\\images\\flags\\vi.png");
		osgEarth::Symbology::IconSymbol *markerSymbol = style.getOrCreateSymbol<osgEarth::Symbology::IconSymbol>();
		//markerSymbol->setImage(image);
		markerSymbol->placement() = osgEarth::Symbology::IconSymbol::PLACEMENT_VERTEX;
		markerSymbol->url() = osgEarth::Symbology::StringExpression("c:\\osgeo4w\\images\\flags\\vi.png");
		//		osg::Node *node = osgDB::readNodeFile("D:\\osg-file-path\\cow.osgt");
		// 		markerSymbol->setModel(node);
		// 		markerSymbol->alignment() = MarkerSymbol::ALIGN_CENTER_CENTER;
		// 		markerSymbol->scale() = 1000;
	}else{
		osgEarth::Symbology::LineSymbol *lineSymbol = style.getOrCreateSymbol<osgEarth::Symbology::LineSymbol>();
		lineSymbol->stroke()->color() = osgEarth::Symbology::Color::Yellow;
		lineSymbol->stroke()->width() = 1.0f;
	}
	OGRFeature::DestroyFeature(feature);
	GDALClose( poDS );

	osgEarth::Symbology::TextSymbol* text = style.getOrCreateSymbol<osgEarth::Symbology::TextSymbol>();
	text->font() = "ºÚÌå";
	text->encoding()= osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
	text->content() = osgEarth::Symbology::StringExpression( "[name]" );
	//text->priority() = NumericExpression( "[pop_cntry]" );
	text->removeDuplicateLabels() = true;
	text->size() = 12;
	text->alignment() = osgEarth::Symbology::TextSymbol::ALIGN_CENTER_CENTER;
	text->fill()->color() = osgEarth::Symbology::Color::White;
	text->halo()->color() = osgEarth::Symbology::Color::Black;

	osgEarth::Drivers::OGRFeatureOptions featureOptions;
	featureOptions.url() = filePath.toStdString();

	osgEarth::Drivers::FeatureGeomModelOptions geomOptions;
	geomOptions.featureOptions() = featureOptions;
	geomOptions.styles() = new osgEarth::Symbology::StyleSheet();
	geomOptions.styles()->addStyle(style);


	global->getMap3D()->addModelLayer(new osgEarth::ModelLayer(layerName.toStdString(), geomOptions));
	return true;
}

bool LayerInterfaceImpl::addElevationLayer(QString layerName, QString filePath)
{
	osgEarth::Drivers::GDALOptions gdal;
	gdal.url() = filePath.toStdString();
	osgEarth::ElevationLayer *layer = new osgEarth::ElevationLayer(layerName.toStdString(), gdal);
	string name = layer->getName();
	if(!layer->open().isOK()){
		delete layer;
		return false;
	}
	global->getMap3D()->addElevationLayer(layer);
	return true;
}

bool LayerInterfaceImpl::createLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool LayerInterfaceImpl::removeLayer(QString layerName)
{
	throw std::logic_error("The method or operation is not implemented.");
}
