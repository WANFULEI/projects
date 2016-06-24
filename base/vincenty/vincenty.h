/*!
 * \file vincenty.h
 * \date 2016/06/24 22:19
 *
 * \author ÎºÇå
 * Contact: 897810981@qq.com
 *
 * \brief 
 *
 * TODO: long description
 *
 * \note
*/
#ifndef VINCENTY_H
#define VINCENTY_H

#include "vincenty_global.h"

namespace vincenty{
	VINCENTY_EXPORT bool calc_dist(double x1, double y1, double x2, double y2, double &dist, double &a1, double &a2);
	VINCENTY_EXPORT void calc_coor(double x1, double y1, double dist, double a, double &x, double &y, double &a2);

	inline bool calc_dist(double x1, double y1, double x2, double y2, double &dist, double &a1){
		double tmp;
		return calc_dist(x1, y1, x2, y2, dist, a1, tmp);
	}
	inline void calc_coor(double x1, double y1, double dist, double a, double &x, double &y){
		double tmp;
		return calc_coor(x1, y1, dist, a, x, y, tmp);
	}
}

#endif // VINCENTY_H
