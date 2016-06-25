#ifndef MATH_H
#define MATH_H

#include "math_global.h"
#include <QPointF>
#include <qlist.h>
#include <math.h>
#include "vector2d.h"


namespace math
{
	enum  Sign
	{
		NEGATIVE = -1, ZERO = 0, POSITIVE = 1,

		// Orientation constants:
		RIGHT_TURN = -1, LEFT_TURN = 1,

		CLOCKWISE = -1, COUNTERCLOCKWISE = 1,

		COLLINEAR = 0, COPLANAR = 0, DEGENERATE = 0,

		// Oriented_side constants:
		ON_NEGATIVE_SIDE = -1, ON_ORIENTED_BOUNDARY = 0, ON_POSITIVE_SIDE = 1,

		// Comparison_result constants:
		SMALLER = -1, EQUAL = 0, LARGER = 1
	};

	typedef Sign Oriented_side;
	typedef Sign Orientation;
	typedef Sign Comparison_result;

	#define IS_DOUBLE_EQUAL(a,b) fabs((a) - (b)) < 
	#define IS_DOUBLE_EQUAL2(a,b,mis) fabs((a) - (b)) < (mis)

	MATH_EXPORT double qt_angle2north_angle(double angle);
	MATH_EXPORT double normalize_north_angle(double angle);
	MATH_EXPORT Orientation is_clock_wise(const QPointF & p1,const QPointF & p2,const QPointF & p3);

	MATH_EXPORT bool is_3point_same_dir(const QPointF & p1,const QPointF & p2,const QPointF & p3,double mis);
	MATH_EXPORT void simplier_polyline(QList<QPointF> & pts,double mis);
	MATH_EXPORT void simplier_polyline(QList<QPoint> & pts,int mis);

	MATH_EXPORT bool is_clock_wise(const vector2d &p1, const vector2d &p2, const vector2d &p3);
};

#endif // MATH_H
