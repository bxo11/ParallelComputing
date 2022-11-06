/*
 * SimpleMessageComparator.cpp
 *
 *  Created on: 8 paź 2022
 *      Author: oramus
 */

#include "SimpleMessageComparator.h"
#include<iostream>

SimpleMessageComparator::SimpleMessageComparator() {
	message = 0;
	messageLength = 0;
	expectedLength = 0;
	expected = 0;
}

SimpleMessageComparator::~SimpleMessageComparator() {
}

bool SimpleMessageComparator::messageDecoded( uint *message ) {
	uint comparisions = messageLength - expectedLength;
	this->message = message;

	for ( uint comparision = 0; comparision <= comparisions; comparision++ ) {
		if ( expectedFragmentFound( comparision ) ) {
			return true;
		}
	}
	return false;
}

bool SimpleMessageComparator::expectedFragmentFound( uint positionInMessage ) {
	for ( uint position = 0; position < expectedLength; position++ ) {
		if ( message[ positionInMessage + position ] != expected[ position ] )
			return false;
	}
	return true;
}
