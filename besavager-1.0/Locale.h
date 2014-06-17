// BeSavager - Savager Workroom Development Toolkit for BeOS
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
// Locale Functions for Menu,Message...

#ifndef __SAVAGER_LOCALE_H__
#define __SAVAGER_LOCALE_H__

#include <SupportDefs.h>
#include <List.h>
#include <Locker.h>
#include <Looper.h>
#include <Messenger.h>
#include <besavager/HashTable.h>

typedef struct s_translation {
	char *original;
	char *translated;
} s_translation;

typedef struct s_languages {
	char *lang;
	s_translation *locale;
	int32 translated;
} s_languages;


// the "lang"(Int32) region: index of the language
//
// 
// switch(message->what)
// {
//    ...
//    case S_LOCALE_CHANGED:
//          {
//             uint32 lang = 0;
//             message->FindInt32("lang", (int32*)&lang);
//             ...
//
#define S_LOCALE_CHANGED		'slc_'

class SLocaleManager;

class SLocale : public SHashTable {
public:
	SLocale();
	virtual ~SLocale();

	virtual uint32 Hash(const void* key);
	virtual bool KeyEqual(const void* a, const void* b);
	virtual void KeyDestroy(void* key);
	virtual void ValueDestroy(void *value);

	void InsertLocale(s_languages lang);

	void Lock();
	void Unlock();

	uint32		CountLocale() const;
	uint32		Locale() const;
	const char*	GetLocale(uint32 index);
	bool		RemoveLocale(uint32 index);

	// SetLocale(),GetString(): don't need to call Lock()/Unlock()
	void SetLocale(uint32 index);
	const char* GetString(const char* string);
	const char* GetString(const char* string, uint32 index);


	// AddMessenger(),RemoveMessenger: don't need to call Lock()/Unlock()
	// just a link!
	bool AddMessenger(BMessenger &msgr);
	bool RemoveMessenger(BMessenger &msgr);

private:
	BList list;
	uint32 locale;
	BLocker locker;
	
	SLocaleManager *manager;
};


#endif /* __SAVAGER_LOCALE_H__ */
