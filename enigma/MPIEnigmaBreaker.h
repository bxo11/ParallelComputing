#ifndef MPIENIGMABREAKER_H_
#define MPIENIGMABREAKER_H_

#include "EnigmaBreaker.h"

class MPIEnigmaBreaker : public EnigmaBreaker
{
private:
	bool solutionFound(uint *rotorSettingsProposal);
	uint *expectedBuffer;
	uint expectedLength;
public:
	MPIEnigmaBreaker(Enigma *enigma, MessageComparator *comparator);
	void crackMessage();
	void getResult(uint *rotorPositions);
	virtual ~MPIEnigmaBreaker();
	void setSampleToFind(uint *expected, uint expectedLength);
	void setMessageToDecode(uint *message, uint messageLength);
};

#endif