/*
 * SerialEnigmaBreaker.cpp
 *
 *  Created on: 8 paź 2022
 *      Author: oramus
 */

#include "SerialEnigmaBreaker.h"

SerialEnigmaBreaker::SerialEnigmaBreaker( Enigma *enigma, MessageComparator *comparator ) : EnigmaBreaker(enigma, comparator ) {
}

SerialEnigmaBreaker::~SerialEnigmaBreaker() {
	delete[] rotorPositions;
}

void SerialEnigmaBreaker::crackMessage() {
	uint rotorLargestSetting = enigma->getLargestRotorSetting();

	/**
	 * Poniższy kod (w szczególności pętle) jest paskudny. Można to
	 * napisac ładniej i, przede wszystkim w wersji uniwersalnej,
	 * ale szkoda czasu. Ta proteza tu wystarczy.
	 * Umawiamy się niniejszym, że więcej niż maxRotors rotorów nie będzie!
	 */

	uint *rMax = new uint[ MAX_ROTORS ];
	for ( uint rotor = 0; rotor < MAX_ROTORS; rotor++ ) {
		if ( rotor < rotors )
			rMax[ rotor ] = rotorLargestSetting;
		else
			rMax[ rotor ] = 0;
	}

	uint *r = new uint[ MAX_ROTORS ];

	for ( r[0] = 0; r[0] <= rMax[0]; r[0]++ )
		for ( r[1] = 0; r[1] <= rMax[1]; r[1]++ )
			for ( r[2] = 0; r[2] <= rMax[2]; r[2]++ )
				for ( r[3] = 0; r[3] <= rMax[3]; r[3]++ )
					for ( r[4] = 0; r[4] <= rMax[4]; r[4]++ )
						for ( r[5] = 0; r[5] <= rMax[5]; r[5]++ )
							for ( r[6] = 0; r[6] <= rMax[6]; r[6]++ )
								for ( r[7] = 0; r[7] <= rMax[7]; r[7]++ )
									for ( r[8] = 0; r[8] <= rMax[8]; r[8]++ )
										for ( r[9] = 0; r[9] <= rMax[9]; r[9]++ ) {
											if ( solutionFound( r ) )
												goto EXIT_ALL_LOOPS;
										}
	EXIT_ALL_LOOPS:

	delete[] rMax;
	delete[] r;
}

bool SerialEnigmaBreaker::solutionFound( uint *rotorSettingsProposal ) {
	for ( uint rotor = 0; rotor < rotors; rotor++ )
		rotorPositions[ rotor ] = rotorSettingsProposal[ rotor ];

	enigma->setRotorPositions(rotorPositions);
	uint *decodedMessage = new uint[ messageLength ];

	for (uint messagePosition = 0; messagePosition < messageLength; messagePosition++ ) {
		decodedMessage[ messagePosition ] = enigma->code(messageToDecode[ messagePosition ] );
	}

	bool result = comparator->messageDecoded(decodedMessage);

	delete[] decodedMessage;

	return result;
}

void SerialEnigmaBreaker::getResult( uint *rotorPositions ) {
	for ( uint rotor = 0; rotor < rotors; rotor++ ) {
		rotorPositions[ rotor ] = this->rotorPositions[ rotor ];
	}
}
