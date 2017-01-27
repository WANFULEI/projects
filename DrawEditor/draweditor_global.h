#ifndef DRAWEDITOR_GLOBAL_H
#define DRAWEDITOR_GLOBAL_H

#include <QtCore/qglobal.h>
#include "osgEarthSymbology/Color"
#include "QColor"
#include "osg/Vec3d"
#include "qgspoint.h"

#ifdef DRAWEDITOR_LIB
# define DRAWEDITOR_EXPORT Q_DECL_EXPORT
#else
# define DRAWEDITOR_EXPORT Q_DECL_IMPORT
#endif

inline osgEarth::Symbology::Color QColor2OsgEarthColor(QColor color){
	return osgEarth::Symbology::Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() );
}

inline osg::Vec3d QgsPoint2Vec3d(QgsPoint pt, double z = 1000){
	return osg::Vec3(pt.x(), pt.y(), z);
}

inline QgsPoint Vec3d2QgsPoint(osg::Vec3d pt){
	return QgsPoint(pt.x(), pt.y());
}

#endif // DRAWEDITOR_GLOBAL_H
