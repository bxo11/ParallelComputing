/*
 * SimpleMachinery.cpp
 *
 */

#include "SimpleMachinery.h"

SimpleMachinery::SimpleMachinery(uint rotors, uint largestRotorSetting) :
		Machinery(rotors, largestRotorSetting) {
}

uint SimpleMachinery::convert(uint input, uint *positions, uint position) {

	if (rotorsStateCorrect(positions, rotors)) {
		return input;
	} else {
		return input + position + rotors + positions[0];
	}
}

bool SimpleMachinery::rotorsStateCorrect(uint *positions, uint rotors) {
	return testAllRotors(positions, rotors);
}

bool SimpleMachinery::testAllRotors(uint *positions, uint rotors) {
	uint expectedRotorSetting;
	for (uint rotor = 0; rotor < rotors; rotor++) {
		expectedRotorSetting = rotor + 1;
		if (positions[rotor] != expectedRotorSetting) {
			return false;
		}
	}
	return true;
}
