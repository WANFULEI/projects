// intersection.cpp : Defines the exported functions for the DLL application.
//

#include "intersection.h"
#include <QtGui/QPolygonF>



INTERSECTION_API bool intersection2::intersection(const QPointF & p1,const QPointF & p2,int mis /*= 8*/)
{
	return (p2 - p1).manhattanLength() <= mis;
}

QRectF calc_envelope(const QList<QPointF> & polyline,int mis /*= 0*/)
{
	return QPolygonF(polyline.toVector()).boundingRect().adjusted(-mis,-mis,mis,mis);
}

INTERSECTION_API intersection2::Result intersection2::intersection(const QList<QPointF> & pts,const QPointF & pt,bool surface /*= false*/,int mis /*= 8*/)
{
	Result res;
	if (pts.size() == 0)
	{
		return res;
	}

	if (!calc_envelope(pts,mis).contains(pt))
	{
		return res;
	}

	for(int i=0;i<pts.size();++i)
	{
		if(intersection(pts[i],pt,mis))
		{
			res.type = Point;
			res.index = i;
			return res;
		}
	}

	QList<QPointF> pts2(pts);
	if (surface)
	{
		pts2 << pts2[0];
	}
	for (int i=0;i<pts2.size()-1;++i)
	{
		res = intersection(QLineF(pts2[i],pts2[i+1]),pt,mis);
		if (res.type != None)
		{
			res.type = Line;
			res.index = i;
			return res;
		}
	}

	if (surface)
	{
		if (QPolygonF(pts2.toVector()).containsPoint(pt,Qt::OddEvenFill))
		{
			res.type = Surface;
			res.index = 0;
		}
	}

	return res;
}

INTERSECTION_API intersection2::Result intersection2::intersection(const QLineF & line,const QPointF & pt,int mis /*= 8*/)
{
	Result res;

	if (!calc_envelope(QList<QPointF>() << line.p1() << line.p2(),mis).contains(pt))
	{
		return res;
	}

	if(intersection(line.p1(),pt,mis))
	{
		res.type = Point;
		res.index = 0;
	}
	else if(intersection(line.p2(),pt,mis))
	{
		res.type = Point;
		res.index = 1;
	}
	else
	{
		QLineF line2 = QLineF::fromPolar(mis,line.angle() + 90);
		QLineF line3 = QLineF::fromPolar(mis,line.angle() - 90);
		QLineF line4(line2.p2(),line3.p2());
		line4.translate(pt);
		if (line4.intersect(line,0) == QLineF::BoundedIntersection)
		{
			res.type = Line;
			res.index = 0;
		}
	}
	return res;
}
