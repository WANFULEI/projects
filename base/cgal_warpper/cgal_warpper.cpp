// cgal_warpper.cpp : Defines the exported functions for the DLL application.
//

#include "cgal_warpper.h"

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Polygon_2.h>

#include <list>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Point_2<K> Point;
typedef CGAL::Polygon_2<K> Polygon_2;
typedef Polygon_2::Vertex_iterator VertexIterator;
typedef Polygon_2::Edge_const_iterator EdgeIterator;


Point convert(const QPointF & pt)
{
	return Point(pt.x(),pt.y());
}

Polygon_2 construct_polygon(const QList<QPointF> & pts)
{
	Polygon_2 p;
	for (int i=0;i<pts.size();++i)
	{
		p.push_back(convert(pts[i]));
	}
	return p;
}

CGAL_WARPPER_API bool cgal_warpper::polygon2::is_simple(const QList<QPointF> & pts)
{
	return construct_polygon(pts).is_simple();
}

CGAL_WARPPER_API bool cgal_warpper::polygon2::is_convex(const QList<QPointF> & pts)
{
	return construct_polygon(pts).is_convex();
}

CGAL_WARPPER_API cgal_warpper::Orientation cgal_warpper::polygon2::orientation(const QList<QPointF> & pts)
{
	return (cgal_warpper::Orientation)construct_polygon(pts).orientation();
}

CGAL_WARPPER_API double cgal_warpper::polygon2::area(const QList<QPointF> & pts)
{
	return construct_polygon(pts).area();
}

CGAL_WARPPER_API cgal_warpper::Bounded_side cgal_warpper::polygon2::bounded_side(const QList<QPointF> & pts,const QPointF pt)
{
	return (cgal_warpper::Bounded_side)construct_polygon(pts).bounded_side(convert(pt));
}
