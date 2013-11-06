/*
 * Copyright (c)1998 by Angry Red Planet.
 *
 * This code is distributed under a modified form of the
 * Artistic License.  A copy of this license should have
 * been included with it; if this wasn't the case, the
 * entire package can be obtained at
 * <URL:http://www.angryredplanet.com/>.
 *
 * ----------------------------------------------------------------------
 *
 * ArpLongArray.h
 *
 * Currently for private use.
 *
 * ----------------------------------------------------------------------
 *
 * Known Bugs
 * ~~~~~~~~~~
 *
 * ----------------------------------------------------------------------
 *
 * To Do
 * ~~~~~
 *
 * ----------------------------------------------------------------------
 *
 * History
 * ~~~~~~~
 *
 * Dec 6, 1998:
 *	First public release.
 *
 */

#ifndef ARP_LONG_ARRAY_H
#define ARP_LONG_ARRAY_H

class ArpLongArray {
	public:
		ArpLongArray();
		ArpLongArray(long);
		virtual ~ArpLongArray();

		long Count();
		long At(long);
		void AtPut(long, long);
		void AtNextPut(long);
		void ShrinkTo(long);
		
		void RemoveAt(long);
		void AddAt(long, long);

		void MakeEmpty();

	protected:
		long	*elements;
		long	size,		// The total number of longs malloc'ed
				count;		// the index of the highest position
							// with actual data.

		void GrowToAtLeast(long);
		long Buffer();
};

#endif /* ARP_LONG_ARRAY_H */