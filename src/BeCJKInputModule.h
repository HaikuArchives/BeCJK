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

#ifndef __SINPUT_MODULE_H__
#define __SINPUT_MODULE_H__

#include <SupportDefs.h>
#include <Bitmap.h>
#include <Locker.h>
#include <Messenger.h>
#include <Menu.h>
#include <besavager/StringArray.h>

typedef enum
{
	CONFIRMED_NONE,
	CONFIRMED_DISPLAY,
	CONFIRMED_ORIGINAL,
} s_input_confirmed_style;


typedef enum
{
	LOCALE_ENGLISH = 0,
	LOCALE_SIMPLIFIED_CHINESE = 1,
	LOCALE_TRADITIONAL_CHINESE = 2,
	LOCALE_JAPANESE = 3,
	LOCALE_KOREAN = 4,
} s_input_locale;


typedef enum s_input_encoding {
	ENCODING_UNKNOWN = 0,
	ENCODING_SIMPLIFIED_CHINESE = 1,
	ENCODING_TRADITIONAL_CHINESE = 2,
	ENCODING_JAPANESE = 3,
	ENCODING_KOREAN = 4,
} s_input_encoding;


class SInputAddOn;
class SInputPanel;

class SInputModule {
public:
	SInputModule(const char *name);
	SInputModule(const char *name, const BBitmap *icon);
	SInputModule(const char *name, const char **xpm_icon);
	virtual ~SInputModule();

	const char* Name() const;
	const BBitmap *Icon() const;

	void SetName(const char* name);
	void SetIcon(const BBitmap *icon);
	void SetIcon(const char** xpm_icon);
	void SetMenu(BMenu *menu);

	virtual void ModuleActivated(bool activated);
	virtual void MessageReceivedFromMenu(BMessage *message);

	virtual bool InitCheck();

	virtual bool KeyFilter(char key, bool shift_pressed = false);
	virtual bool PageUp();
	virtual bool PageDown();

	virtual void Reset();
	virtual bool IsEmpty();
	virtual const char* Original() const;
	virtual const char* Display() const;

	virtual bool AutoHideSelector();
	virtual const SStringArray *SelectItems(int32 *start_item_index = NULL, int32 *select_item_index = NULL) const;
	virtual bool Selection(int32 *selection_start = NULL, int32 *selection_end = NULL) const;
	virtual const char* SelectKeys() const;

	virtual s_input_confirmed_style Confirmed() const;
	virtual void InsertWord(const char *word, const char *key);

	virtual bool ItemChangedFromSelector(int32 index);
	virtual bool ItemSelectedFromSelector(int32 index);

	virtual void LocaleChanged(s_input_locale locale);
	s_input_locale Locale() const;

	s_input_encoding Encoding() const;

	bool Lock();
	void Unlock();
	bool IsLocked() const;

protected:
	s_input_encoding encoding;

	bool IsCJKPunctuation();
	bool IsAllCorner();

	virtual char* GetCornerOrPunctuation(char c);

private:
	friend class SInputAddOn;
	friend class SInputPanel;

	BLocker *locker;
	BMessenger *messenger;

	char *fName;
	BBitmap *fIcon;
	BMenu *fMenu;

	s_input_locale locale;

	BMenu* Menu();
};


#endif /* __SINPUT_MODULE_H__ */
