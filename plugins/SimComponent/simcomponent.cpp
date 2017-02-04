#include "simcomponent.h"
#include "Component/GlobalInstance.h"
#include "WSim/WSimObject.h"
#include "osgEarthAnnotation/LocalGeometryNode"
#include "osg/Node"
#include "WSim/WSimControlThread.h"

using namespace osgEarth;
using namespace osgEarth::Annotation;

SimComponent::SimComponent()
{

}

SimComponent::~SimComponent()
{

}

class MySimObject : public WSimObject
{
public:


	virtual void update3D()
	{
		GeoPoint pt = m_node->getPosition();
		pt.x() += 0.0001;
		pt.y() += 0.0001;
		pt.z() = 10000;
		m_node->setPosition(pt);
	}

	LocalGeometryNode *m_node;
};

void SimComponent::initialize()
{
	osg::Node *plane = osgDB::readNodeFile("cow.osg");
	LocalGeometryNode *model = new LocalGeometryNode(global->getMapNode(), plane);
	model->setPosition(GeoPoint(global->getMap3D()->getSRS(), 0, 0, 0));
	model->setScale(osg::Vec3f(100000, 100000, 100000));
	global->getRoot()->addChild(model);

	MySimObject *obj = new MySimObject;
	obj->m_node = model;
	WSimControlThread::instance()->addSimObject(obj);
	//WSimControlThread::instance()->get3DThread()->setRunMaxFrameRate(60);
	WSimControlThread::instance()->start();

}

Q_EXPORT_PLUGIN2(SimComponent, SimComponent)