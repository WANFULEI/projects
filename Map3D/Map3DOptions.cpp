#include "Map3DOptions.h"
#include "Component\GlobalInstance.h"
#include "osgEarth\TerrainEngineNode"
#include "osgEarthUtil\VerticalScale"
#include "osgEarth\MapNode"
#include "osg\View"
#include "osgEarth\Extension"
#include "osgEarthUtil\Controls"
#include "osgEarthSymbology\Color"
#include "osgEarthUtil\Sky"
#include "osgEarthSymbology\Style"


Map3DOptions::Map3DOptions(QWidget *parent)
	:QDialog(parent)
{
	setupUi(this);
	m_sky = 0;
}


Map3DOptions::~Map3DOptions(void)
{
}

void Map3DOptions::on_sliderVerticalScale_sliderReleased()
{
	auto effect = global->getMapNode()->getTerrainEngine()->getEffect<osgEarth::Util::VerticalScale>();
	if(effect == 0) return;
	effect->setScale(sliderVerticalScale->value());
}

void Map3DOptions::on_checkSky_stateChanged(int state)
{
	if(m_sky == 0){
		m_sky = osgEarth::Extension::create("sky_simple", osgEarth::ConfigOptions());
	}
	if(state == Qt::Checked){
		global->getMapNode()->addExtension(m_sky);

		// Check for a View interface:
		auto viewIF = osgEarth::ExtensionInterface<osg::View>::get( m_sky );
		if ( viewIF )
			viewIF->connect( global->getViewer() );


		auto canvas = osgEarth::Util::Controls::ControlCanvas::getOrCreate( global->getViewer() );

		osgEarth::Util::Controls::Container* mainContainer;
		mainContainer = new osgEarth::Util::Controls::VBox();
		mainContainer->setAbsorbEvents( true );
		mainContainer->setBackColor( osgEarth::Symbology::Color(osgEarth::Symbology::Color::Black, 0.8) );
		mainContainer->setHorizAlign( osgEarth::Util::Controls::Control::ALIGN_LEFT );
		mainContainer->setVertAlign( osgEarth::Util::Controls::Control::ALIGN_BOTTOM );
		canvas->addControl( mainContainer );


		auto controlIF = osgEarth::ExtensionInterface<osgEarth::Util::Controls::Control>::get(m_sky);
		if ( controlIF )
			controlIF->connect( mainContainer );

		auto parents = global->getMapNode()->getParents();
		for(int i=0; i<parents.size(); ++i){
			auto *sky = dynamic_cast<osgEarth::Util::SkyNode *>(parents[i]);
			if(sky){
				sky->setLighting(0);
			}
		}

	}else if(state == Qt::Unchecked){
		global->getMapNode()->removeExtension(m_sky);
	}
}
