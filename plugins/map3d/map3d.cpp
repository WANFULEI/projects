#include "map3d.h"

#include <osg/Notify>
#include <osg/Version>
#include <osgEarth/ImageUtils>
#include <osgEarth/MapNode>
#include <osgEarthAnnotation/AnnotationData>
#include <osgEarthAnnotation/AnnotationNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/ScaleDecoration>
#include <osgEarthAnnotation/TrackNode>
#include <osgEarthQt/LayerManagerWidget>
#include <osgEarthQt/MapCatalogWidget>
#include <osgEarthQt/DataManager>
#include <osgEarthQt/AnnotationListWidget>
#include <osgEarthQt/LOSControlWidget>
#include <osgEarthQt/TerrainProfileWidget>
#include <osgEarthUtil/AnnotationEvents>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/Sky>
#include <osgEarthUtil/Ocean>
#include <osgEarthSymbology/Style>
#include <osgEarthSymbology/Geometry>
#include <osgEarthFeatures/Feature>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthUtil/ObjectLocator>
#include <osgEarthAnnotation/ModelNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthSymbology/Stroke>
#include <osgEarthSymbology/Fill>
#include "../../framecore/framegui/framegui.h"

using namespace osg;
using namespace osgEarth::Annotation;
using namespace osgEarth::Symbology;
using namespace osgEarth::Util;

extern "C"
{
	MAP3D_EXPORT framecore::component * create_component(const QString & class_name){
		if (class_name == "map3d")
		{
			return map3dcom::get_instance();
		}
		return 0;
	}
};

map3d::map3d()
{
	std::vector<std::string> files;
	files.push_back("gdal_tiff.earth");
	osg::Node* earthNode = osgDB::readNodeFiles( files );
	osgEarth::MapNode * mapnode = osgEarth::MapNode::findMapNode(earthNode);

	osg::Group* root = new osg::Group();
	root->addChild( earthNode );

	osgViewer::Viewer * viewer = dynamic_cast<osgViewer::Viewer*>(getViewer());
	viewer->setSceneData( root );
// 	viewer->setThreadingModel(osgViewer::Viewer::DrawThreadPerContext);
// 	viewer->setCameraManipulator(new osgEarth::Util::EarthManipulator());

// 	osgEarth::QtGui::ViewVector views;
// 	getViews( views );
// 
// 	for(osgEarth::QtGui::ViewVector::iterator i = views.begin(); i != views.end(); ++i )
// 	{
// 		i->get()->getCamera()->addCullCallback(new osgEarth::Util::AutoClipPlaneCullCallback(mapnode));
// 	}

	const osgEarth::SpatialReference * srs = mapnode->getMapSRS()->getGeographicSRS();

	osg::Group * features = new osg::Group();
	root->addChild(features);

	Style style;
	osg::Image * image = osgDB::readImageFile("F:/test/tabBkg.png");
	// 	MatrixTransform * matrix = new MatrixTransform(Matrixd::scale(100,100,100));
	// 	matrix->addChild(image);

	double xmin = -180;
	double xmax = 180;
	double ymin = -90;
	double ymax = 90;
	for (int i=0;i<1 * 100;++i)
	{
		double x = xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX;
		double y = ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX;


		osgEarth::Annotation::PlaceNode * place = new osgEarth::Annotation::PlaceNode(mapnode,
			GeoPoint(srs,Vec3d(x,y,0),AltitudeMode::ALTMODE_RELATIVE),
			image,
			"test place node",
			style);

		// 		osgEarth::Util::ObjectLocatorNode * place = new osgEarth::Util::ObjectLocatorNode(mapnode->getMap());
		// 		place->getLocator()->setPosition(osg::Vec3d(x,  y, 0 ) ); 
		// 		place->addChild(matrix);

		features->addChild(place);
	}

	ModelSymbol * model_symbol = style.getOrCreateSymbol<ModelSymbol>();
// 	model_symbol->url() = StringExpression("cow.osg");
// 	model_symbol->autoScale() = true;
 	model_symbol->scale() = 100000;
	osg::Node * model_node = osgDB::readNodeFile("glider.osgt");
	model_symbol->setModel(model_node);

	for (int i=0;i<1 * 100;++i)
	{
		double x = xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX;
		double y = ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX;


		ModelNode * model = new ModelNode(mapnode,style);
		model->setPosition(GeoPoint(srs,Vec3d(x,y,10 * 10000),AltitudeMode::ALTMODE_RELATIVE));

		features->addChild(model);
	}

	Style style2;
	PolygonSymbol * polygon_symbol = style2.getOrCreate<PolygonSymbol>();
	polygon_symbol->fill() = Fill(1,0,1,0.0);
	LineSymbol * line_symbol = style2.getOrCreate<LineSymbol>();
	Stroke stroke;
	stroke.width() = 2.0;
	stroke.color() = Color(1,0,0,1);
	stroke.widthUnits() = Units::PIXELS;
	line_symbol->stroke() = stroke;

	for (int i=0;i<100;++i)
	{
		double x = xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX;
		double y = ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX;

		RectangleNode * rectangle = new RectangleNode(mapnode,
			GeoPoint(srs,Vec3d(x,y,10 * 10000),AltitudeMode::ALTMODE_RELATIVE),
			10 * 10000,
			10 * 10000,
			style2);

		features->addChild(rectangle);
	}

	for (int i=0;i<100;++i)
	{
		double x = xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX;
		double y = ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX;

		EllipseNode * ellipse = new EllipseNode(mapnode,
								GeoPoint(srs,Vec3d(x,y,10 * 10000),AltitudeMode::ALTMODE_RELATIVE),
								10 * 10000,
								10 * 10000,
								0,
								style2);

		features->addChild(ellipse);
	}

// 	for (int i=0;i<100;++i)
// 	{
// 		double x = xmin + (xmax - xmin) * ((double) qrand()) / RAND_MAX;
// 		double y = ymin + (ymax - ymin) * ((double) qrand()) / RAND_MAX;

		Vec3dVector vec;
		vec.push_back(Vec3d(0,0,100000));
		vec.push_back(Vec3d(10,0,100000));
		vec.push_back(Vec3d(5,10,100000));
		osgEarth::Symbology::Polygon * geometry = new osgEarth::Symbology::Polygon(&vec);
		osgEarth::Features::Feature * feature = new osgEarth::Features::Feature(geometry,
												srs,
												style2,1);
		osgEarth::Annotation::FeatureNode * feature_node = new osgEarth::Annotation::FeatureNode(mapnode,feature);


		features->addChild(feature_node);
//	}
}
map3d::~map3d()
{

}


map3dcom::map3dcom()
{
	m_map3d = 0;
}


bool map3dcom::initialize()
{
	if (m_map3d == 0)
	{
		m_map3d = new map3d;
	}
	framegui::center_widget * widget = framegui::framegui::get_instance()->get_type_object<framegui::center_widget>("map3d");
	if (widget)
	{
		widget->set_widget(m_map3d);
		return true;
	}
	delete m_map3d;
	return false;
}
