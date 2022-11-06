/*
 * Consts.h
 */

#ifndef CONSTS_H_
#define CONSTS_H_

#include<iostream>

using namespace std;

typedef unsigned int uint;

// ta stała nie zmieni wartości ;-)
const uint MAX_ROTORS = 10;

const int MPI_ROOT_PROCESS_RANK = 0;

const double MAX_DELAY = 5.0;
//było błędnie MAX_TIMEOUT;

void showUint( uint *source, uint length );
void showOK();
void showHR();

#endif /* CONSTS_H_ */
