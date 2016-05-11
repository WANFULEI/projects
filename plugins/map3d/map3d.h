#ifndef MAP3D_H
#define MAP3D_H

#include "map3d_global.h"
#include <osgEarthQt/ViewerWidget>
#include "../../framecore/framecore/framecore.h"
#include "../../base/baseset2/baseset2.h"

class MAP3D_EXPORT map3d : public osgEarth::QtGui::ViewerWidget
{
public:
	map3d();
	~map3d();

private:

};

class map3dcom : public framecore::component , public baseset::instance2<map3dcom>
{
public:
	map3dcom();
private:
	virtual bool initialize();
private:
	map3d * m_map3d;
};


#endif // MAP3D_H
