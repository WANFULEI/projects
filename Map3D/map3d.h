#ifndef MAP3D_H
#define MAP3D_H

#include "map3d_global.h"
#include "component.h"

namespace osgEarth{
	namespace Util{
		class EarthManipulator;
	}
}
class QgsMapCanvas;
class Map3D : public Component
{
	Q_OBJECT
public:
	Map3D();
	~Map3D();

	void initialize();

private:
	QgsMapCanvas *m_mapCanvas;
	osgEarth::Util::EarthManipulator *m_manip;

private slots:
	void slot_extentsChanged();
};

#endif // MAP3D_H
