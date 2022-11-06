#ifndef MPIEnigmaBreaker_H_
#define MPIEnigmaBreaker_H_

#include"EnigmaBreaker.h"

class MPIEnigmaBreaker : public EnigmaBreaker {
private:
	bool solutionFound( uint *rotorSettingsProposal );
public:
	MPIEnigmaBreaker( Enigma *enigma, MessageComparator *comparator);

	void crackMessage();
	void getResult( uint *rotorPositions );

	virtual ~MPIEnigmaBreaker();
};

#endif