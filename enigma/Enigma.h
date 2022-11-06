/*
 * Enigma.h
 */

#ifndef ENIGMA_H_
#define ENIGMA_H_

#include"Machinery.h"
#include"Consts.h"

class Enigma {
private:
	uint *rotorPositions;
	uint ordinalNumber;
	Machinery *machinery;
	uint rotors;
	uint largestRotorSetting;
public:
	Enigma(Machinery *machinery);
	void setRotorPositions(uint *positions);
	uint code(uint input);

	virtual ~Enigma();

	uint getNumberOfRotors() const {
		return rotors;
	}

	uint getLargestRotorSetting() const {
		return largestRotorSetting;
	}
};

#endif /* ENIGMA_H_ */
