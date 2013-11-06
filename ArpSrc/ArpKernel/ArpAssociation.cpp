/*
	
	ArpAssociation.cpp
	
	Copyright (c)1998 by Angry Red Planet.

	This code is distributed under a modified form of the
	Artistic License.  A copy of this license should have
	been included with it; if this wasn't the case, the
	entire package can be obtained at
	<URL:http://www.angryredplanet.com/>.
*/

/********************************************************
* ArpAssociation.cpp
********************************************************/

#ifndef ARPKERNEL_ARPASSOCIATION_H
#include "ArpKernel/ArpAssociation.h"
#endif


ArpAssociation::ArpAssociation(void *keyArg, void *valArg) {
	key = keyArg;
	value = valArg;
}


//************************************************************************
// Public - accessing
//------------------------------------------------------------------------

void* ArpAssociation::Key() {
	return(key);
}

void ArpAssociation::SetKey(void *keyArg) {
	key = keyArg;
}

void* ArpAssociation::Value() {
	return(value);
}

void ArpAssociation::SetValue(void *valArg) {
	value = valArg;
}
