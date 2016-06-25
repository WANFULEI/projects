#include "math2.h"
#include <QLineF>

MATH_EXPORT double math::qt_angle2north_angle(double angle)
{
	angle = 90 - angle;
	return normalize_north_angle(angle);
}

MATH_EXPORT double math::normalize_north_angle(double angle)
{
	while (angle < 0)
	{
		angle += 360;
	}
	while (angle > 360)
	{
		angle -= 360;
	}
	return angle;
}

MATH_EXPORT bool math::is_3point_same_dir(const QPointF & p1,const QPointF & p2,const QPointF & p3,double mis)
{
	return IS_DOUBLE_EQUAL2(QLineF(p1,p2).angle(),QLineF(p2,p3).angle(),mis);
}

MATH_EXPORT void math::simplier_polyline(QList<QPointF> & pts,double mis)
{
	for (int i=0;i<pts.size()-2;)
	{
		if (is_3point_same_dir(pts[i],pts[i+1],pts[i+2],mis))
		{
			pts.removeAt(i+1);
		}
		else
		{
			++i;
		}
	}
}

bool math::is_clock_wise(const vector2d &p1, const vector2d &p2, const vector2d &p3)
{
	//向量AC叉乘一下向量AB,结果大于0为逆时针,小于0为顺时针 
	return (p2 - p1) * (p3 - p1) < 0;
}

MATH_EXPORT void math::simplier_polyline(QList<QPoint> & pts,int mis)
{
	for (int i=0;i<pts.size()-1;)
	{
		if ((pts[i+1]-pts[i]).manhattanLength() < mis)
		{
			pts.removeAt(i+1);
		}
		else
		{
			++i;
		}
	}
}

MATH_EXPORT math::Orientation math::is_clock_wise(const QPointF & p1,const QPointF & p2,const QPointF & p3)
{
	return CLOCKWISE;
}

