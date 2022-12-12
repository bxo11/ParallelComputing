#ifndef FUNCTION_H
#define FUNCTION_H

#include<stdlib.h>
#include"Point3D.h"

class Function {
protected:
	Point3D *maxPoint;
	Point3D *minPoint;
	void setMax( double xmax, double ymax, double zmax );
	void setMin( double xmin, double ymin, double zmin  );
public:
	Function() {}
	virtual ~Function() {}

	virtual double value( double x, double y, double z ) = 0;

	Point3D *min() {
		return minPoint;
	}

	Point3D *max() {
		return maxPoint;
	}
};

#endif

