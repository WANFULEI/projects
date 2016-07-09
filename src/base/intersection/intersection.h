#ifdef INTERSECTION_EXPORTS
#define INTERSECTION_API __declspec(dllexport)
#else
#define INTERSECTION_API __declspec(dllimport)
#endif

#include <QtCore/QPointF>
#include <QtCore/QLineF>
#include <QtCore/QList>
#include <QtCore/QRectF>

namespace intersection2
{
	enum Type
	{
		None,
		Point,
		Line,
		Surface
	};
	struct Result 
	{
		Result(){
			type = None;
			index = -1;
		}
		Type type;
		int index;
	};

	INTERSECTION_API bool intersection(const QPointF & p1,const QPointF & p2,int mis = 8);
	INTERSECTION_API Result intersection(const QLineF & line,const QPointF & pt,int mis = 8);
	INTERSECTION_API Result intersection(const QList<QPointF> & pts,const QPointF & pt,bool surface = false,int mis = 8);

	inline bool intersection(const QPoint & p1,const QPoint & p2,int mis = 8)
	{
		return intersection(QPointF(p1),QPointF(p2),mis);
	}
	inline Result intersection(const QLine & line,const QPoint & pt,int mis = 8)
	{
		return intersection(QLineF(line),pt,mis);
	}
	inline Result intersection(const QList<QPoint> & pts,const QPoint & pt,bool surface = false,int mis = 8)
	{
		QList<QPointF> pts2;
		for (int i=0;i<pts.size();++i)
		{
			pts2 << pts[i];
		}
		return intersection(pts2,QPointF(pt),surface,mis);
	}
}

