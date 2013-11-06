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
 * ArpConfigureImpl.h
 *
 * This is a helper class for implementing configuration views.
 * It contains generic functions for implementing your view's
 * AttachedToWindow() function, message handler, applying and updating
 * the view's controls, etc.
 *
 * Note that you should NOT inherit from this class, but just include
 * an instance as a member of your own class.
 *
 * ----------------------------------------------------------------------
 *
 * Known Bugs
 * ~~~~~~~~~~
 *
 * ----------------------------------------------------------------------
 *
 * History
 * ~~~~~~~
 *
 * 05/01/1999:
 *	• Created this file.
 *
 */

#ifndef ARPKERNEL_ARPCONFIGUREIMPL_H
#define ARPKERNEL_ARPCONGIGUREIMPL_H

#ifndef ARPKERNEL_ARPCONFIGURABLEI_H
#include <ArpKernel/ArpConfigurableI.h>
#endif

#ifndef _VIEW_H
#include <be/interface/View.h>
#endif

#ifndef _MESSAGE_H
#include <be/app/Message.h>
#endif

#ifndef _MESSENGER_H
#include <be/app/Messenger.h>
#endif

#ifndef _INVOKER_H
#include <be/app/Invoker.h>
#endif

// Forward references
class BCheckBox;
class BTextControl;

class ArpConfigureImpl
{
public:
	ArpConfigureImpl(BMessenger configTarget, BView* configRoot,
					 BMessage& settings);
	virtual ~ArpConfigureImpl();

	// ------ Attaching/Detaching Controls And Window ------
	
	// Call these in your own AttachedToWindow()/DetachedFromWindow()
	// functions.  Note that your configuration view -must- implement
	// the related BView functions, so that these can be called by you.
	void AttachedToWindow();
	void DetachedFromWindow();
	
	// Send a message to our owning ArpConfigurePanel, asking
	// it to rebuild the list of views.
	void RebuildPanel() const;
	
	// ------ High-Level Updating And Reporting Controls ------
	
	enum { CONFIG_REPORT_MSG = 'aCRP' };
	
	// Call these functions to get the BMessage object that you
	// will pass to the control's constructor.  For example:
	//   new BCheckBox(myRect, "MyParameterName",
	//                 "Select This Paramater",
	//                 mImpl.AttachCheckBox("MyParameterName"));
	
	// Create the BMessage report for an int32 or bool parameter
	// that is controlled by a check box.  If this is an int32
	// parameter, you must supply the bitmask 'mask' to indicate
	// which bit(s) are set/cleared by the checkbox.  Otherwise,
	// the checkbox will be directly tied to a bool parameter.
	BMessage* AttachCheckBox(const char* param, int32 mask = 0);
	
	// Create the BMessage report for a string or int32 parameter
	// that is controlled by a text control.
	BMessage* AttachTextControl(const char* param);
	
	// Call this in your own MessageReceived() function.  It returns
	// B_OK if the given message was handled by ArpConfigureImpl, in
	// which case you should return.  If any other value is returned,
	// you should handle the message yourself.
	status_t MessageReceived(const BMessage* message);
	
	// ------ Low-Level Updating And Reporting Controls ------
	
	status_t RefreshControls(const BMessage& settings);
	status_t RefreshControl(const BMessage& settings, const BMessage& info);
	
	status_t UpdateCheckBox(const char* param, const BMessage& settings,
							int32 mask = 0);
	status_t ReportCheckBox(const char* param, int32 mask = 0);

	status_t UpdateTextControl(const char* param, const BMessage& settings);
	status_t ReportTextControl(const char* param);

private:
	void UpdateTargets(BView* parent, BHandler* target) const;
	status_t ParseUpdateMessage(const BMessage& message,
								int32* type, const char** param) const;
	BCheckBox* FindCheckBox(const char* name, type_code* paramType) const;
	BTextControl* FindTextControl(const char* name, type_code* paramType) const;
								
	BMessenger mConfigTarget;
	BMessenger mConfigPanel;
	BView* mConfigRoot;
	BMessage& mSettings;
	
	ArpVectorI<BMessage*>* mAttachments;
};

#endif
