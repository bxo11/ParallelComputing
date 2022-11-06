/*
 * Machinery.h
 */

#ifndef MACHINERY_H_
#define MACHINERY_H_

#include "Consts.h"

class Machinery {
protected:
	uint rotors;
	uint largestRotorSetting;
public:
	Machinery(uint rotors, uint largestRotorSetting);
	uint getNumberOfRotors() {
		return rotors;
	}
	uint getLargestRotorSetting() {
		return largestRotorSetting;
	}
	virtual unsigned int convert(unsigned int input,
			unsigned int *rotorSettings, unsigned int ordinalNumber) = 0;
	virtual ~Machinery();
};

#endif /* MACHINERY_H_ */
