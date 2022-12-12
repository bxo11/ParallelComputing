/*
 * SimpleTestFunction.cpp
 */

#include<math.h>

#include "SimpleTestFunction.h"

SimpleTestFunction::SimpleTestFunction() {
	setMax(6.0,6.0,6.0);
	setMin(0.0,0.0,0.0);
}

SimpleTestFunction::~SimpleTestFunction() {
}

double SimpleTestFunction::value(double x, double y, double z) {
	double dx = x - 2;
	double dy = y - 3;
	double dz = z - 4;

	return sin(x) + sin(y) + sin(z) + cos(x) + cos(y) + cos(z)
			- 30.0 * exp(-sqrt(dx * dx + dy * dy + dz * dz) / 20.0);
}
