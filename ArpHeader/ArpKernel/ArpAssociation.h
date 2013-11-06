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
 * ArpAssociation.h
 *
 * ----------------------------------------------------------------------
 *
 * Known Bugs
 * ~~~~~~~~~~
 *
 * Don't use this -- it is not really for public consumption.
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

#ifndef ARPKERNAL_ARPASSOCIATION_H
#define ARPKERNEL_ARPASSOCIATION_H

class ArpAssociation {
	public:
		ArpAssociation(void *keyArg = 0, void *valArg = 0);

		void* Key();
		void SetKey(void*);
		void* Value();
		void SetValue(void*);

	private:
		void	*key, *value;
};

#endif /* ARPKERNEL_ARPASSOCIATION_H */
