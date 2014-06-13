// BeCJK - Input Server Add-On for CJK Input
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU Library General Public
// License along with this library; if not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
//
//
// Input Module

#include <ctype.h>
#include <besavager/XPM.h>

#include "AllMessages.h"
#include "BeCJKInputModule.h"
#include "ChinputSymbol.h"

SInputModule::SInputModule(const char *name)
	: locker(NULL), messenger(NULL), fName(NULL), fIcon(NULL), fMenu(NULL)
{
	locale = LOCALE_ENGLISH;
	encoding = ENCODING_UNKNOWN;

	SetName(name);
}


SInputModule::SInputModule(const char *name, const BBitmap *icon)
	: locker(NULL), messenger(NULL), fName(NULL), fIcon(NULL)
{
	locale = LOCALE_ENGLISH;
	encoding = ENCODING_UNKNOWN;

	SetName(name);
	SetIcon(icon);
}


SInputModule::SInputModule(const char *name, const char **xpm_icon)
	: locker(NULL), messenger(NULL), fName(NULL), fIcon(NULL)
{
	locale = LOCALE_ENGLISH;
	encoding = ENCODING_UNKNOWN;

	SetName(name);
	SetIcon(xpm_icon);
}


const char*
SInputModule::Name() const
{
	return fName;
}


void
SInputModule::SetName(const char* name)
{
	if(fName)
	{
		delete[] fName;
		fName = NULL;
	}

	if(name) fName = Sstrdup(name);

	if(messenger) messenger->SendMessage(S_MODULE_CHANGED_MSG);
}


const BBitmap*
SInputModule::Icon() const
{
	return fIcon;
}


void
SInputModule::SetIcon(const BBitmap* icon)
{
	if(fIcon)
	{
		delete fIcon;
		fIcon = NULL;
	}

	if(icon) fIcon = new BBitmap(icon);

	if(messenger) messenger->SendMessage(S_MODULE_CHANGED_MSG);
}


void
SInputModule::SetIcon(const char** xpm_icon)
{
	if(fIcon)
	{
		delete fIcon;
		fIcon = NULL;
	}

	if(xpm_icon)
	{
		SXPM image(xpm_icon);
		fIcon = image.ConvertToBitmap();
	}

	if(messenger) messenger->SendMessage(S_MODULE_CHANGED_MSG);
}


SInputModule::~SInputModule()
{
	if(fName) delete[] fName;
	if(fIcon) delete fIcon;
	if(messenger) delete messenger;
}


bool
SInputModule::InitCheck()
{
	return false;
}


bool
SInputModule::KeyFilter(char key, bool shift_pressed)
{
	return false;
}


bool
SInputModule::PageUp()
{
	return false;
}


bool
SInputModule::PageDown()
{
	return false;
}


void
SInputModule::Reset()
{
}


bool
SInputModule::IsEmpty()
{
	return true;
}


const char*
SInputModule::Original() const
{
	return NULL;
}


const char*
SInputModule::Display() const
{
	return NULL;
}


const SStringArray*
SInputModule::SelectItems(int32 *start_item_index, int32 *select_item_index) const
{
	return NULL;
}


bool
SInputModule::Selection(int32 *selection_start, int32 *selection_end) const
{
	return false;
}


const char*
SInputModule::SelectKeys() const
{
	return NULL;
}


s_input_confirmed_style
SInputModule::Confirmed() const
{
	return CONFIRMED_NONE;
}


void
SInputModule::InsertWord(const char *word, const char *key)
{
}


void
SInputModule::LocaleChanged(s_input_locale locale)
{
	SInputModule::locale = locale;
}


s_input_locale
SInputModule::Locale() const
{
	return locale;
}


s_input_encoding
SInputModule::Encoding() const
{
	return encoding;
}


bool SInputModule::Lock()
{
	return(locker ? locker->Lock() : false);
}


void SInputModule::Unlock()
{
	if(locker) locker->Unlock();
}


bool SInputModule::IsLocked() const
{
	return(locker ? locker->IsLocked() : false);
}


bool
SInputModule::IsCJKPunctuation()
{
	if(!messenger) return false;

	BMessage reply;
	BMessage msg(S_MODULE_CJK_PUNCTUATION_REQUESTED_MSG);
	messenger->SendMessage(&msg, &reply, 100000, 100000);

	bool result = false;
	reply.FindBool("result", &result);
	
	return result;
}


bool
SInputModule::IsAllCorner()
{
	if(!messenger) return false;

	BMessage reply;
	BMessage msg(S_MODULE_ALL_CORNER_REQUESTED_MSG);
	messenger->SendMessage(&msg, &reply, 100000, 100000);

	bool result = false;
	reply.FindBool("result", &result);

	return result;
}


char*
SInputModule::GetCornerOrPunctuation(char c)
{
	return chinput_get_corner_punct(c, encoding, IsAllCorner(), IsCJKPunctuation());
}


bool
SInputModule::AutoHideSelector()
{
	return true;
}


bool
SInputModule::ItemChangedFromSelector(int32 index)
{
	return false;
}


bool
SInputModule::ItemSelectedFromSelector(int32 index)
{
	return false;
}


void
SInputModule::SetMenu(BMenu *menu)
{
	fMenu = menu;
}


BMenu*
SInputModule::Menu()
{
	if(!fMenu) return NULL;

	BMessage msg;
	if(fMenu->Archive(&msg, true) == B_OK)
	{
		BArchivable *arch = BMenu::Instantiate(&msg);
		if(arch)
		{
			BMenu *menu = dynamic_cast<BMenu*>(arch);
			if(!menu) delete arch;
			return menu;
		}
	}

	return NULL;
}


void
SInputModule::ModuleActivated(bool activated)
{
}


void
SInputModule::MessageReceivedFromMenu(BMessage *message)
{
}
