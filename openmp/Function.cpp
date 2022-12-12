#include"Function.h"
#include<math.h>

void Function::setMax( double xmax, double ymax, double zmax ) {
	maxPoint = new Point3D( xmax, ymax, zmax );
}

void Function::setMin( double xmin, double ymin, double zmin ) {
	minPoint = new Point3D( xmin, ymin, zmin );
}
