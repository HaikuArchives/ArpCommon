/*
 *
 * Copyright (c)1998 by Angry Red Planet.
 *
 * This code is distributed under a modified form of the
 * Artistic License.  A copy of this license should have
 * been included with it; if this wasn't the case, the
 * entire package can be obtained at
 * <URL:http://www.angryredplanet.com/>.
 */

/********************************************************
* ArpLongArray.cpp
* My implementation of a growable array.  Need to generalize
* this so it works with any data type!
* It is indexed starting at 0.
* This is kind of designed so that all the elements put
* in it are >=0.  That doesn't HAVE to be the case, but
* -1 (B_ERROR) is used as the error code.
********************************************************/

#ifndef ARPCOLLECTIONS_ARPLONGARRAY_H
#include <ArpCollections/ArpLongArray.h>
#endif

#include <malloc.h>

#ifndef _ERRORS_H
#include <Errors.h>
#endif

ArpLongArray::ArpLongArray() {
	size = -1;
	count = -1;
	elements = NULL;
}

ArpLongArray::ArpLongArray(long sizeArg) {
	size = sizeArg;
	count = -1;
	elements = (long*)malloc(sizeof(long) * size);
}

ArpLongArray::~ArpLongArray() {
	MakeEmpty();
}

	
//************************************************************************
// Public - accessing
//------------------------------------------------------------------------

long ArpLongArray::Count() {
	return(count);
}

// Answer the value at the index or B_ERROR
long ArpLongArray::At(long index) {
	if (index > count) return(B_ERROR);
	return(elements[index]);	
}

void ArpLongArray::AtPut(long index, long value) {
	if (index >= size) GrowToAtLeast(index);
	elements[index] = value;
	if (index > count) count = index;
}

void ArpLongArray::AtNextPut(long value) {
	if ((count+1) >= size) GrowToAtLeast(count+1);
	count++;
	elements[count] = value;
}

void ArpLongArray::ShrinkTo(long index) {
	if (elements == NULL) return;
	if (index < 0) {
		free(elements);
		return;
	}
	
	long	*tmp;
	tmp = (long*)malloc(sizeof(long) * (index + Buffer()));
	if (count > index) count = index;
	for (long i=0; i<count; i++) tmp[i] = elements[i];
	size = index + Buffer();
	free(elements);
	elements = tmp;
}

// remove the item at index.
// NOTE that this does not currently shrink the array
void ArpLongArray::RemoveAt(long index) {
	if (index > count) return;
	
	for (long i=index; i<=count; i++) elements[i] = elements[i+1];
	count--;
}

void ArpLongArray::AddAt(long index, long value) {
	if (index >= size) GrowToAtLeast(index);

	count++;
	for (long i=count; i>index; i--) elements[i] = elements[i-1];
	elements[index] = value;
}

void ArpLongArray::MakeEmpty() {
	size = -1;
	count = -1;
	if (elements) free(elements);
	elements = NULL;
}


//************************************************************************
// Protected - growing
//------------------------------------------------------------------------

void ArpLongArray::GrowToAtLeast(long index) {
	long	*tmp;
	long	newSize;
	
	(index >= size) ? (newSize = index + Buffer())
					: (newSize = size + Buffer());
					
	tmp = (long*)malloc(sizeof(long) * newSize);
	for (long i=0; i<count; i++) tmp[i] = elements[i];
	if (elements) free(elements);
	elements = tmp;
	size = newSize;
}

long ArpLongArray::Buffer() {
	return(10);
}
