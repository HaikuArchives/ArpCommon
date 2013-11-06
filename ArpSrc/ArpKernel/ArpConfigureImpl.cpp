/*
	
	ArpConfigureImpl.cpp
	
	Copyright (c)1999 by Angry Red Planet.

	This code is distributed under a modified form of the
	Artistic License.  A copy of this license should have
	been included with it; if this wasn't the case, the
	entire package can be obtained at
	<URL:http://www.angryredplanet.com/>.
*/

#ifndef _AUTOLOCK_H
#include <be/support/Autolock.h>
#endif

#ifndef _MENUFIELD_H
#include <be/interface/MenuField.h>
#endif

#ifndef _POPUPMENU_H
#include <be/interface/PopUpMenu.h>
#endif

#ifndef _MENUITEM_H
#include <be/interface/MenuItem.h>
#endif

#ifndef _COLORCONTROL_H
#include <be/interface/ColorControl.h>
#endif

#ifndef _STRINGVIEW_H
#include <be/interface/StringView.h>
#endif

#ifndef _TEXTCONTROL_H
#include <be/interface/TextControl.h>
#endif

#ifndef _CHECKBOX_H
#include <be/interface/CheckBox.h>
#endif

#ifndef ARPKERNEL_ARPCONFIGUREIMPL_H
#include <ArpKernel/ArpConfigureImpl.h>
#endif

#ifndef ARPKERNEL_ARPMESSAGE_H
#include <ArpKernel/ArpMessage.h>
#endif

#ifndef ARPKERNEL_ARPSTRING_H
#include <ArpKernel/ArpString.h>
#endif

#ifndef ARPCOLLECTIONS_ARPPTRVECTOR_H
#include <ArpCollections/ArpPtrVector.h>
#endif

#ifndef ARPKERNEL_ARPDEBUG_H
#include <ArpKernel/ArpDebug.h>
#endif

ArpMOD();

ArpConfigureImpl::ArpConfigureImpl(BMessenger configTarget,
									BView* configRoot,
									BMessage& settings)
	: mConfigTarget(configTarget), mConfigRoot(configRoot),
	  mSettings(settings),
	  mAttachments(0)
{
	mAttachments = new ArpPtrVector<BMessage*>;
	
	ArpD(cdb << ADH << "Configuring settings: " << mSettings << endl);
}

ArpConfigureImpl::~ArpConfigureImpl()
{
	if( mAttachments ) {
		for( size_t i=0; i<mAttachments->size(); i++ ) {
			delete mAttachments->at(i);
		}
	}
	delete mAttachments;
	mAttachments = 0;
}

void ArpConfigureImpl::AttachedToWindow()
{
	if( !mConfigRoot ) return;
	
	UpdateTargets(mConfigRoot, mConfigRoot);
	
	BMessenger w(mConfigRoot);
	BMessage add_watch(ARP_ADD_WATCHER_MSG);
	add_watch.AddMessenger("watch", w);
	mConfigTarget.SendMessage(&add_watch);
}

void ArpConfigureImpl::DetachedFromWindow()
{
	if( !mConfigRoot ) return;
	
	BMessenger w(mConfigRoot);
	BMessage rem_watch(ARP_REM_WATCHER_MSG);
	rem_watch.AddMessenger("watch", w);
	mConfigTarget.SendMessage(&rem_watch);
}

void ArpConfigureImpl::RebuildPanel() const
{
	if( mConfigPanel.IsValid() ) {
		ArpD(cdb << ADH << "Asking configuration panel to rebuild." << endl);
		BMessage rebuild(ARP_REBUILD_PANEL_MSG);
		mConfigPanel.SendMessage(&rebuild);
	} else {
		ArpD(cdb << ADH << "*** Asked for panel rebuild, but bad messenged." << endl);
	}
}

enum {
	CHECKBOX_PARAM = 0,
	TEXTCONTROL_PARAM = 1
};

BMessage* ArpConfigureImpl::AttachCheckBox(const char* param,
										int32 mask)
{
	BMessage* msg = new BMessage(CONFIG_REPORT_MSG);
	
	if( msg ) {
		status_t res;
		res = msg->AddString("parameter", param);
		if( res == B_OK ) {
			res = msg->AddInt32("type", CHECKBOX_PARAM);
		}
		if( res == B_OK ) {
			res = msg->AddInt32("mask", mask);
		}
		if( res != B_OK ) {
			ArpD(cdb << ADH << "Error creating CheckBox msg: "
					<< res << endl);
			delete msg;
			return 0;
		}
	}
	
	BMessage* attach;
	
	try {
		attach = new BMessage(*msg);
	} catch(...) {
		delete msg;
		throw;
	}
	
	try {
		mAttachments->push_back(attach);
	} catch(...) {
		delete attach;
		delete msg;
		throw;
	}
	
	return msg;
}

BMessage* ArpConfigureImpl::AttachTextControl(const char* param)
{
	BMessage* msg = new BMessage(CONFIG_REPORT_MSG);
	
	if( msg ) {
		status_t res;
		res = msg->AddString("parameter", param);
		if( res == B_OK ) {
			res = msg->AddInt32("type", TEXTCONTROL_PARAM);
		}
		if( res != B_OK ) {
			ArpD(cdb << ADH << "Error creating TextControl msg: "
					<< res << endl);
			delete msg;
			return 0;
		}
	}
	
	BMessage* attach;
	
	try {
		attach = new BMessage(*msg);
	} catch(...) {
		delete msg;
		throw;
	}
	
	try {
		mAttachments->push_back(attach);
	} catch(...) {
		delete attach;
		delete msg;
		throw;
	}
	
	return msg;
}

status_t ArpConfigureImpl::MessageReceived(const BMessage* message)
{
	if( !message ) return B_ERROR;
	
	if( message->what == CONFIG_REPORT_MSG ) {
		ArpD(cdb << ADH << "Reporting configuration change: " << *message
						<< endl);
		int32 reportType;
		const char* param;
		status_t res = ParseUpdateMessage(*message, &reportType, &param);
		if( res != B_OK ) return res;
		
		switch( reportType ) {
			case CHECKBOX_PARAM: {
				int32 mask;
				res = message->FindInt32("mask", &mask);
				if( res != B_OK ) return res;
				return ReportCheckBox(param, mask);
			} break;
			case TEXTCONTROL_PARAM: {
				return ReportTextControl(param);
			} break;
		}
	
	} else if( message->what == ARP_PUT_CONFIGURATION_MSG ) {
		ArpD(cdb << ADH << "*** Configuration changed." << endl);
		BMessage settings;
		status_t res = message->FindMessage("settings", &settings);
		if( res != B_OK ) return res;
		ArpUpdateMessage(mSettings, settings);
		ArpD(cdb << ADH << "New settings: " << mSettings << endl);
		return RefreshControls(settings);
	
	} else if( message->what == ARP_SET_CONFIG_PANEL_MSG ) {
		ArpD(cdb << ADH << "Setting config panel target: "
				<< *message << endl);
		message->FindMessenger("panel", &mConfigPanel);
		return B_OK;
	}
	
	return B_ERROR;
}

status_t ArpConfigureImpl::RefreshControls(const BMessage& settings)
{
	if( !mAttachments ) return B_ERROR;
	
	status_t res = B_OK;
	for( size_t i=0; i<mAttachments->size(); i++ ) {
		if( (*mAttachments)[i] != 0 ) {
			status_t ret = RefreshControl(settings, *((*mAttachments)[i]));
			if( res == B_OK ) res = ret;
		}
	}
	return res;
}

status_t ArpConfigureImpl::RefreshControl(const BMessage& settings,
										const BMessage& message)
{
	int32 reportType;
	const char* param;
	status_t res = ParseUpdateMessage(message, &reportType, &param);
	if( res != B_OK ) return res;
	
	switch( reportType ) {
		case CHECKBOX_PARAM: {
			int32 mask;
			res = message.FindInt32("mask", &mask);
			if( res != B_OK ) return res;
			return UpdateCheckBox(param, settings, mask);
		} break;
		case TEXTCONTROL_PARAM: {
			return UpdateTextControl(param, settings);
		} break;
	}
	
	return B_ERROR;
}

status_t ArpConfigureImpl::UpdateCheckBox(const char* param,
										const BMessage& settings,
										int32 mask)
{
	ArpD(cdb << ADH << "ArpConfigureImpl: update " << param << endl);
	type_code type;
	BCheckBox* view = FindCheckBox(param, &type);
	if( !view ) return B_ERROR;
	
	status_t res;
	if( type == B_BOOL_TYPE ) {
		bool state;
		if( (res=settings.FindBool(param, &state)) == B_OK ) {
			view->SetValue(state ? B_CONTROL_ON : B_CONTROL_OFF);
		}
	} else if( type == B_INT32_TYPE ) {
		int32 flags;
		if( (res=settings.FindInt32(param, &flags)) == B_OK ) {
			view->SetValue((flags&mask) ? B_CONTROL_ON : B_CONTROL_OFF);
		}
	} else {
		ArpD(cdb << ADH << "Unable to handle parameter type: " << type << endl);
		res = B_ERROR;
	}
	
	return res;
}

status_t ArpConfigureImpl::ReportCheckBox(const char* param,
									int32 mask)
{
	ArpD(cdb << ADH << "ArpConfigureImpl: report " << param << endl);
	type_code type;
	BCheckBox* view = FindCheckBox(param, &type);
	if( !view ) return B_ERROR;
	
	BMessage settings;
	status_t res;
	if( type == B_BOOL_TYPE ) {
		bool state = view->Value() ? true : false;
		res = settings.AddBool(param, state);
		if( res == B_OK ) {
			res = mSettings.ReplaceBool(param, state);
		}
	} else if( type == B_INT32_TYPE ) {
		int32 flags;
		res = mSettings.FindInt32(param, &flags);
		flags = (flags&~mask) | (view->Value() ? mask : 0);
		if( res == B_OK ) {
			res = settings.AddInt32(param, flags);
		}
		if( res == B_OK ) {
			res = mSettings.ReplaceInt32(param, flags);
		}
	} else {
		ArpD(cdb << ADH << "Unable to handle parameter type: " << type << endl);
		res = B_ERROR;
	}
	
	if( res == B_OK ) {
		BMessage update(ARP_PUT_CONFIGURATION_MSG);
		res = update.AddMessage("settings", &settings);
		if( res == B_OK ) {
			ArpD(cdb << ADH << "Sending update message: " << update << endl);
			mConfigTarget.SendMessage(&update);
		}
	}
	
	return res;
}

status_t ArpConfigureImpl::UpdateTextControl(const char* param,
										const BMessage& settings)
{
	ArpD(cdb << ADH << "ArpConfigureImpl: update " << param << endl);
	type_code type;
	BTextControl* view = FindTextControl(param, &type);
	if( !view ) return B_ERROR;
	
	status_t res;
	if( type == B_STRING_TYPE ) {
		const char* str;
		if( (res=settings.FindString(param, &str)) == B_OK ) {
			if( str ) view->SetText(str);
			else view->SetText("");
		}
	} else if( type == B_INT32_TYPE ) {
		int32 val;
		if( (res=settings.FindInt32(param, &val)) == B_OK ) {
			ArpString str(val);
			view->SetText(str);
		}
	} else {
		ArpD(cdb << ADH << "Unable to handle parameter type: " << type << endl);
		res = B_ERROR;
	}
	
	return res;
}

status_t ArpConfigureImpl::ReportTextControl(const char* param)
{
	ArpD(cdb << ADH << "ArpConfigureImpl: report " << param << endl);
	type_code type;
	BTextControl* view = FindTextControl(param, &type);
	if( !view ) return B_ERROR;
	
	BMessage settings;
	status_t res = B_OK;
	if( type == B_STRING_TYPE ) {
		const char* str = view->Text();
		if( !str ) str = "";
		res = settings.AddString(param, str);
		if( res == B_OK ) {
			res = mSettings.ReplaceString(param, str);
		}
	} else if( type == B_INT32_TYPE ) {
		ArpString str(view->Text());
		bool valid = false;
		int32 val = str.AsInt(10,&valid);
		if( !valid ) res = B_ERROR;
		if( res == B_OK ) {
			res = settings.AddInt32(param, val);
		}
		if( res == B_OK ) {
			res = mSettings.ReplaceInt32(param, val);
		}
	} else {
		ArpD(cdb << ADH << "Unable to handle parameter type: " << type << endl);
		res = B_ERROR;
	}
	
	if( res == B_OK ) {
		BMessage update(ARP_PUT_CONFIGURATION_MSG);
		res = update.AddMessage("settings", &settings);
		if( res == B_OK ) {
			ArpD(cdb << ADH << "Sending update message: " << update << endl);
			mConfigTarget.SendMessage(&update);
		}
	}
	
	return res;
}

void ArpConfigureImpl::UpdateTargets(BView* parent, BHandler* target) const
{
	BView* child = parent->ChildAt(0);
	while( child ) {
		BView* cur = child;
		child = child->NextSibling();
		
		BPopUpMenu* menu = dynamic_cast<BPopUpMenu*>(cur);
		if( menu ) {
			menu->SetTargetForItems(target);
			continue;
		}
		
		BInvoker* invoke = dynamic_cast<BInvoker*>(cur);
		if( invoke ) {
			invoke->SetTarget(target);
			continue;
		}
		
		UpdateTargets(cur, target);
	}
}

status_t ArpConfigureImpl::ParseUpdateMessage(const BMessage& message,
											int32* type,
											const char** param) const
{
	if( message.what != CONFIG_REPORT_MSG ) return B_ERROR;
	
	status_t res = message.FindInt32("type", type);
	if( res != B_OK ) return res;
	
	res = message.FindString("parameter", param);
	if( res != B_OK ) return res;
	
	return B_OK;
}

BCheckBox* ArpConfigureImpl::FindCheckBox(const char* name,
										type_code* paramType) const
{
	ArpD(cdb << ADH << "Finding view named: " << name << endl);
	
	BView* found = mConfigRoot ? mConfigRoot->FindView(name) : 0;
	if( !found ) {
		ArpD(cdb << ADH << "View not found!" << endl);
		return 0;
	}
	
	ArpD(cdb << ADH << "View found: " << found->Name()
			<< " (" << found << ")" << endl);
			
	BCheckBox* view = dynamic_cast<BCheckBox*>(found);
	if( !view ) {
		ArpD(cdb << ADH << "View not a BCheckBox!" << endl);
		return 0;
	}
	
	ArpD(cdb << ADH << "CheckBox is: " << view << endl);
	
	if( paramType ) {
		int32 count;
		if( mSettings.GetInfo(name, paramType, &count) != B_OK ) {
			ArpD(cdb << ADH << "Parameter not in settings!" << endl);
			*paramType = 0;
		}
	}
	
	return view;
}

BTextControl* ArpConfigureImpl::FindTextControl(const char* name,
										type_code* paramType) const
{
	ArpD(cdb << ADH << "Finding view named: " << name << endl);
	
	BView* found = mConfigRoot ? mConfigRoot->FindView(name) : 0;
	if( !found ) {
		ArpD(cdb << ADH << "View not found!" << endl);
		return 0;
	}
	
	ArpD(cdb << ADH << "View found: " << found->Name()
			<< " (" << found << ")" << endl);
			
	BTextControl* view = dynamic_cast<BTextControl*>(found);
	if( !view ) {
		ArpD(cdb << ADH << "View not a BTextControl!" << endl);
		return 0;
	}
	
	ArpD(cdb << ADH << "TextControl is: " << view << endl);
	
	if( paramType ) {
		int32 count;
		if( mSettings.GetInfo(name, paramType, &count) != B_OK ) {
			ArpD(cdb << ADH << "Parameter not in settings!" << endl);
			*paramType = 0;
		}
	}
	
	return view;
}
