#pragma once

namespace math{

class vector3d
{
public:
	vector3d(void) { x = y = z = 0; }
	vector3d(double x, double y, double z) :x(x), y(y), z(z) {}
	~vector3d(void) {}

public:
	double x, y, z;
};

vector3d operator-(const vector3d &v1, const vector3d &v2){
	return vector3d(v1.x-v2.x, v1.y-v2.y, v1.z-v2.z);
}

vector3d operator*(const vector3d &v1, const vector3d &v2){
	return vector3d(v1.y*v2.z-v2.y*v1.z, v2.x*v1.z-v1.x*v2.z, v1.x*v2.y-v2.x*v1.y);
}

}