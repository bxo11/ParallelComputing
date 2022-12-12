/*
 * Point3D.h
 */

#ifndef POINT3D_H_
#define POINT3D_H_

#include<iostream>

class Point3D {
	const double x, y, z;
public:
	Point3D(double _x, double _y, double _z);
	double getX() {
		return x;
	}
	double getY() {
		return y;
	}
	double getZ() {
		return z;
	}
	virtual ~Point3D();

	friend auto operator<<(std::ostream& os, Point3D const& p) -> std::ostream& {
	        return os << p.x << ", " << p.y << ", " << p.z;
	    }
};

#endif /* POINT3D_H_ */
