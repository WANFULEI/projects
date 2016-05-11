// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the CGAL_WARPPER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// CGAL_WARPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef CGAL_WARPPER_EXPORTS
#define CGAL_WARPPER_API __declspec(dllexport)
#else
#define CGAL_WARPPER_API __declspec(dllimport)
#endif


#include <QtCore/QList>
#include <QtCore/QPointF>

namespace cgal_warpper
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

	enum  Bounded_side
	{
		ON_UNBOUNDED_SIDE = -1,
		ON_BOUNDARY,
		ON_BOUNDED_SIDE
	};

	

	namespace polygon2
	{
		CGAL_WARPPER_API bool is_simple(const QList<QPointF> & pts);
		CGAL_WARPPER_API bool is_convex(const QList<QPointF> & pts);
		CGAL_WARPPER_API Orientation orientation(const QList<QPointF> & pts);
		CGAL_WARPPER_API double area(const QList<QPointF> & pts);
		CGAL_WARPPER_API Bounded_side bounded_side(const QList<QPointF> & pts,const QPointF);
	}
}
