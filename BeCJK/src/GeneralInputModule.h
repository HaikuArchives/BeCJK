// BeCJK - Input Server Add-On for CJK Input
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// General Input Module

#ifndef __SINPUT_GENERAL_MODULE_H__
#define __SINPUT_GENERAL_MODULE_H__

#include <SupportDefs.h>
#include <besavager/HashTable.h>

class SInputModule;

class SGeneralInputModuleFrequence {
public:
	SGeneralInputModuleFrequence();
	~SGeneralInputModuleFrequence();

	void Reset();

	void ToFront(const char *string);
	void ToBack(const char *string);

	int32 Lookup(const char *string); // return -1 when error

private:
	SStringInt32Hash *hash;
	SStringArray *array;
};


class SGeneralInputModule : public SInputModule {
public:
	SGeneralInputModule(const char *file_name);
	virtual ~SGeneralInputModule();

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

	virtual void ModuleActivated(bool activated);
	virtual void MessageReceivedFromMenu(BMessage *message);

private:
	char *name;
	char *locale_name;

	char *select_keys;
	char *version;
	char *copyright;
	char *help;

	char custom_page_up_key;
	char custom_page_down_key;

	bool allow_changed;

	SStringArray *keyname;
	SStringArray *keymap;

	SStringHash *key_value_hash;
	SStringHash *value_key_hash;
	SStringHash *key_index_hash_array;

	SStringArray *select_items;
	BList *select_items_key;

	SString original;
	SString buffer;
	SString display;

	SStringArray *words;
	SStringArray *words_key;

	SGeneralInputModuleFrequence *freq;

	bool words_first;
	bool auto_select;
	bool auto_fix_up;
	bool enter_to_output_original;
	bool adjust_order_by_frequence;

	int32 nchange;

	int32 nselect;
	int32 npage;

	s_input_confirmed_style confirmed;

	bool IsValid();

	void EmptyTable();

	void LookForSelectItems(const char* first_item = NULL, const char* first_item_key = NULL, bool all_match = false, bool allow_auto_select = false);
	void ItemSelected(int32 index);
	void EmptySelectItems();

	void RefreshDisplay();

	char *table_fname;
	bool Load(const char *table_file_name);

	bool force_select_key;

	BMenu *menu;

	BMenu* GetMenu();

	bool activated;

	void WriteHeaderToTable();
};


#endif /* __SINPUT_GENERAL_MODULE_H__ */
