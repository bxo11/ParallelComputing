#ifndef MINIMUM_H
#define MINIMUM_H

#include"Function.h"
#include"Minimization.h"

class SimpleMinimization: public Minimization {
private:
	void generateRandomPosition(double* pointerX,double* pointerY,double* pointerZ, drand48_data buffer, int rseed, double randValue);
	struct drand48_data buffer;
	int rseed;
public:
	void find(double dr_ini, double dr_fin, int idleStepsLimit);
	SimpleMinimization(Function *f, double timeLimit);
	~SimpleMinimization();
};

#endif

