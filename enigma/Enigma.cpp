/*
 * Enigma.cpp
 */

#include "Enigma.h"

Enigma::Enigma( Machinery *machinery ): machinery(machinery) {
	ordinalNumber = 0;
	rotorPositions = 0;
	rotors = machinery->getNumberOfRotors();
	largestRotorSetting = machinery->getLargestRotorSetting();
}

void Enigma::setRotorPositions( uint* rotorPositions ) {
	this->rotorPositions = rotorPositions;
	ordinalNumber = 0;
}

unsigned int Enigma::code( uint input ) {
	ordinalNumber++;
	return machinery->convert( input, rotorPositions, ordinalNumber );
}

Enigma::~Enigma() {
}

