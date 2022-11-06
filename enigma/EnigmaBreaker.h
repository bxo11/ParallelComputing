/*
 * EnigmaBreaker.h
 */

#ifndef ENIGMABREAKER_H_
#define ENIGMABREAKER_H_

#include"Enigma.h"
#include"MessageComparator.h"
#include"Consts.h"

class EnigmaBreaker {
protected:
	Enigma *enigma;
	MessageComparator *comparator;
	uint *messageToDecode;
	uint *messageProposal;
	uint messageLength;
	uint *rotorPositions;
	uint rotors;
public:
	EnigmaBreaker( Enigma *enigma, MessageComparator *comparator );

	virtual void setSampleToFind( uint *expected, uint expectedLength );
	virtual void setMessageToDecode( uint *message, uint messageLength );
	virtual void crackMessage() = 0;
	virtual void getResult( uint *rotorPositions ) = 0;

	virtual ~EnigmaBreaker();
};

#endif /* ENIGMABREAKER_H_ */
