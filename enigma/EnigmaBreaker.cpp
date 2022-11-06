/*
 * EnigmaBreaker.cpp
 */

#include "EnigmaBreaker.h"

EnigmaBreaker::EnigmaBreaker(Enigma *enigma, MessageComparator *comparator) {
	this->enigma = enigma;
	this->comparator = comparator;
	this->messageLength = 0;
	this->messageToDecode = 0;
	this->messageProposal = 0;
	this->rotors = enigma->getNumberOfRotors();
	this->rotorPositions = new uint[ rotors ];
}

EnigmaBreaker::~EnigmaBreaker() {
	delete[] messageProposal;
}

void EnigmaBreaker::setMessageToDecode( uint *message, uint messageLength ) {
	comparator->setMessageLength(messageLength);
	this->messageLength = messageLength;
	this->messageToDecode = message;
	messageProposal = new uint[ messageLength ];
}

void EnigmaBreaker::setSampleToFind(uint *expected, uint expectedLength ) {
	comparator->setExpectedFragment(expected, expectedLength);
}
