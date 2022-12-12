/*
 * Minimization.h
 *
 *  Created on: 24 lis 2022
 *      Author: oramus
 */

#ifndef MINIMIZATION_H_
#define MINIMIZATION_H_

#include<vector>
#include"Function.h"
#include"Point3D.h"

using namespace std;

class Minimization {
private:
	double limit(double min, double max, double v);
protected:
	double minX, minY, minZ;
	double maxX, maxY, maxZ;
	double bestX, bestY, bestZ, bestV;
	double x, y, z;

	double timeLimit;
	Function *function;
	std::vector<Point3D*> history;

	void setTimeLimit(double msec);
	bool hasTimeToContinue();
	double limitX(double v);
	double limitY(double v);
	double limitZ(double v);
	void addToHistory();
public:
	Minimization(Function *function, double timeLimit);
	virtual ~Minimization();

	virtual void find(double dr_ini, double dr_fin, int idleStepsLimit) = 0;

	Point3D* getBestPosition() {
		return new Point3D(bestX, bestY, bestZ);
	}

	double getBestValueFound() {
		return bestV;
	}

	std::vector<Point3D*> getHistory() {
		return history;
	}

	int historySize() {
		return history.size();
	}
};

#endif /* MINIMIZATION_H_ */
