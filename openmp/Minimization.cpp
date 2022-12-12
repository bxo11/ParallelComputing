/*
 * Minimization.cpp
 *
 *  Created on: 24 lis 2022
 *      Author: oramus
 */

#include <sys/time.h>
#include <iostream>

#include "Minimization.h"
#include "Point3D.h"

using namespace std;

Minimization::Minimization(Function *fn, double timeLimit) :
		function(fn) {
	setTimeLimit(timeLimit);

	Point3D *max = fn->max();
	Point3D *min = fn->min();

	cout << "MIN = " << *min << endl;
	cout << "MAX = " << *max << endl;

	maxX = max->getX();
	maxY = max->getY();
	maxZ = max->getZ();

	minX = min->getX();
	minY = min->getY();
	minZ = min->getZ();
}

Minimization::~Minimization() {
}

bool Minimization::hasTimeToContinue() {
	struct timeval tf;
	gettimeofday(&tf, nullptr);
	double now = tf.tv_sec * 1000 + tf.tv_usec * 0.001;

	if (now < timeLimit)
		return true;
	return false;
}

void Minimization::setTimeLimit(double msec) {
	struct timeval tf;
	gettimeofday(&tf, nullptr);
	timeLimit = tf.tv_sec * 1000 + tf.tv_usec * 0.001 + msec; // ustawiamy czas zakonczenia obliczen
}

void Minimization::addToHistory() {
	Point3D *point = new Point3D(x,y,z);
	history.push_back( point );
}

double Minimization::limit(double min, double max, double v) {
	if (v < min)
		return min;
	if (v > max)
		return max;
	return v;
}

double Minimization::limitX(double v) {
	return limit(minX, maxX, v);
}

double Minimization::limitY(double v) {
	return limit(minY, maxY, v);
}

double Minimization::limitZ(double v) {
	return limit(minZ, maxZ, v);
}
