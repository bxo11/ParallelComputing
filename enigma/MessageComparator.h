/*
 * MessageComparator.h
 */

#ifndef MESSAGECOMPARATOR_H_
#define MESSAGECOMPARATOR_H_

#include"Consts.h"

class MessageComparator {
protected:
	uint *expected;
	uint expectedLength;
	uint messageLength;
public:
	MessageComparator();
	void setExpectedFragment(uint *expected, uint expectedLength) {
		this->expected = expected;
		this->expectedLength = expectedLength;
	}
	void setMessageLength(uint messageLength) {
		this->messageLength = messageLength;
	}
	virtual bool messageDecoded(uint *message) = 0;
	virtual ~MessageComparator();
};

#endif /* MESSAGECOMPARATOR_H_ */
