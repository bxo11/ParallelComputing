/*
 * SimpleMachinery.h
 */

#ifndef SIMPLEMACHINERY_H_
#define SIMPLEMACHINERY_H_

#include "Machinery.h"
#include"Consts.h"

class SimpleMachinery: public Machinery {
private:
	bool rotorsStateCorrect(uint *positions, uint rotors);
	bool testAllRotors(uint *positions, uint rotors);
public:
	SimpleMachinery(uint rotors, uint largestRotorSetting);

	virtual uint convert(uint input, uint *positions, uint position);
};

#endif /* SIMPLEMACHINERY_H_ */
