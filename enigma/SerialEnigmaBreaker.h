/*
 * SerialEnigmaBreaker.h
 *
 *  Created on: 8 paź 2022
 *      Author: oramus
 */

#ifndef SERIALENIGMABREAKER_H_
#define SERIALENIGMABREAKER_H_

#include"EnigmaBreaker.h"

class SerialEnigmaBreaker : public EnigmaBreaker {
private:
	bool solutionFound( uint *rotorSettingsProposal );
public:
	SerialEnigmaBreaker( Enigma *enigma, MessageComparator *comparator );

	void crackMessage();
	void getResult( uint *rotorPositions );

	virtual ~SerialEnigmaBreaker();
};

#endif /* SERIALENIGMABREAKER_H_ */
