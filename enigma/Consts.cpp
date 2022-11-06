/*
 * Consts.cpp
 */

#include"Consts.h"

void showUint( uint *source, uint length ) {
	for ( uint i = 0; i < length; i++ )
		cout << i << "\t" << source[i] << endl;
}

void showOK() {
	cout << "OK" << endl;
}

void showHR() {
	cout << "----------------------------------------" << endl;
}



