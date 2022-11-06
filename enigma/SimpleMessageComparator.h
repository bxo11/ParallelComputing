/*
 * SimpleMessageComparator.h
 *
 *  Created on: 8 paź 2022
 *      Author: oramus
 */

#ifndef SIMPLEMESSAGECOMPARATOR_H_
#define SIMPLEMESSAGECOMPARATOR_H_

#include "MessageComparator.h"

class SimpleMessageComparator: public MessageComparator {
private:
	uint *message;
	bool expectedFragmentFound( uint comparision );
public:
	SimpleMessageComparator();
	bool messageDecoded( uint *message );
	virtual ~SimpleMessageComparator();
};

#endif
