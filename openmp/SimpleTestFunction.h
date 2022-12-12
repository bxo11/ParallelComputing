/*
 * SimpleTestFunction.h
 */

#ifndef SIMPLETESTFUNCTION_H_
#define SIMPLETESTFUNCTION_H_

#include "Function.h"

class SimpleTestFunction: public Function {
public:
	SimpleTestFunction();
	virtual ~SimpleTestFunction();

	double value( double x, double y, double z );
};

#endif /* SIMPLETESTFUNCTION_H_ */
