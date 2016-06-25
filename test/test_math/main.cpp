#include "test_math.h"
#include <QtGui/QApplication>
#include "../../base/math/math2.h"
using namespace math;
#include <assert.h>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	test_math w;
	w.show();

	if (is_clock_wise( vector2d(0, 0), vector2d(5, 5), vector2d(5, 0) ))
	{
		int n = 0;
	}
	if ( !is_clock_wise( vector2d(5, 0), vector2d(5, 5), vector2d(0, 0) ))
	{
		int n = 0;
	}

	return a.exec();
}
