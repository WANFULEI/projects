#pragma once

namespace math{

class vector2d
{
public:
	vector2d(void) { x = y = 0; }
	vector2d(double x, double y) :x(x), y(y) {}
	~vector2d(void) {}

public:
	double x, y;
};

vector2d operator-(const vector2d &v1, const vector2d &v2){
	return vector2d(v1.x-v2.x, v1.y-v2.y);
}

double operator*(const vector2d &v1, const vector2d &v2){
	return v1.x*v2.y - v2.x*v1.y;
}

}