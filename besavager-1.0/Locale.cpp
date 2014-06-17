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

#include "Locale.h"
#include "Strings.h"
#include <Autolock.h>


class SLocaleManager : public BLooper {
public:
	SLocaleManager();
	virtual ~SLocaleManager();

	virtual void MessageReceived(BMessage *message);

	// just a link
	bool AddMessenger(BMessenger &msgr);
	bool RemoveMessenger(BMessenger &msgr);

private:
	BList messengers_list;
};


bool
SLocaleManager::AddMessenger(BMessenger &msgr)
{
	BMessenger *messenger = new BMessenger();
	*messenger = msgr;

	if(!messengers_list.AddItem((void*)messenger))
	{
		delete messenger;
		return false;
	}
	
	return true;
}


bool
SLocaleManager::RemoveMessenger(BMessenger &msgr)
{
	BMessenger *messenger = NULL;

	if(!messengers_list.IsEmpty())
	{
		for(int32 i = 0; (messenger = (BMessenger*)messengers_list.ItemAt(i)) != NULL; i++)
		{
			if(*messenger == msgr)
			{
				messenger = (BMessenger*)messengers_list.RemoveItem(i);
				if(messenger)
				{
					delete messenger;
					return true;
				}
				break;
			}
		}
	}

	return false;
}


SLocaleManager::SLocaleManager()
	: BLooper("Savager Locale Manager Looper")
{
}


SLocaleManager::~SLocaleManager()
{
	BMessenger *messenger = NULL;
	if(!messengers_list.IsEmpty())
	{
		for(int32 i = 0; (messenger = (BMessenger*)messengers_list.ItemAt(i)) != NULL; i++)
		{
			if(messenger) delete messenger;
		}
		
		messengers_list.MakeEmpty();
	}
}


void
SLocaleManager::MessageReceived(BMessage *message)
{
	if(!message) return;

	BAutolock autolocker(this);

	switch(message->what)
	{
		case S_LOCALE_CHANGED:
			{
				uint32 lang = 0;
				if(message->FindInt32("lang", (int32*)&lang) != B_OK) break;
				
				if(!messengers_list.IsEmpty())
				{
					BMessenger *msgr = NULL;
		
					for(int32 i = 0; (msgr = (BMessenger*)messengers_list.ItemAt(i)) != NULL; i++)
					{
						msgr->SendMessage(message);
					}
				}
			}
			break;

		default:
			BLooper::MessageReceived(message);
	}
}


SLocale::SLocale()
	: SHashTable(), locale(0)
{
	manager = new SLocaleManager();
	if(manager)
	{
		manager->Run();
	}
}


SLocale::~SLocale()
{
	if(CountLocale() > 0)
	{
		MakeEmpty();

		char *Item;
		for(int32 i = 0; (Item = (char*)list.ItemAt(i)) != NULL; i++) delete[] Item;
	}

	if(manager)
	{
		manager->Lock();
		manager->Quit();
	}
}


uint32
SLocale::Hash(const void* key)
{
	const char *p = (char*)key;
	uint32 h = *p;

	if(h)
		for(p += 1; *p != '\0'; p++) h = (h << 5) - h + *p;

	return h;
}


bool
SLocale::KeyEqual(const void* a, const void* b)
{
	const char *string1 = (char*)a;
	const char *string2 = (char*)b;

	return(strcmp(string1, string2) == 0);
}


void
SLocale::KeyDestroy(void* key)
{
	char *string = (char*)key;
	delete[] string;
}


void
SLocale::ValueDestroy(void* value)
{
	SStringHash *hash = (SStringHash*)value;
	delete hash;
}


void
SLocale::InsertLocale(s_languages lang)
{
	if(!lang.lang) return;

	SStringHash *hash = (SStringHash*)Lookup(lang.lang);

	if(!hash)
	{
		char *key_string = Sstrdup(lang.lang);

		SStringHash *value_hash = new SStringHash();

		if(lang.locale && lang.translated > 0)
		{
			for(int32 i = 0; i < lang.translated; i++)
				value_hash->InsertString(lang.locale[i].original,
										 lang.locale[i].translated);
		}

		Insert((void*)key_string, (void*)value_hash);

		list.AddItem(Sstrdup(key_string));
	}
	else /* follow the previous translation */
	{
		if(lang.locale && lang.translated > 0)
		{
			for(int32 i = 0; i < lang.translated; i++)
				hash->InsertString(lang.locale[i].original, lang.locale[i].translated);
		}
	}
}


bool
SLocale::RemoveLocale(uint32 index)
{
	if(Remove(GetLocale(index)))
	{
		char *lang = (char*)list.RemoveItem(index);
		if(lang) delete[] lang;
	}
	
	return false;
}


void
SLocale::Lock()
{
	locker.Lock();
}


void
SLocale::Unlock()
{
	locker.Unlock();
}


uint32
SLocale::CountLocale() const
{
	return list.CountItems();
}


void
SLocale::SetLocale(uint32 index)
{
	Lock();

	if(index >= CountLocale())
	{
		Unlock();
		return;
	}

	locale = index;
	
	if(manager)
	{
		BMessage msg(S_LOCALE_CHANGED);
		msg.AddInt32("lang", (int32)locale);

		BMessenger msgr(NULL, manager);
		msgr.SendMessage(&msg);
	}

	Unlock();
}


const char*
SLocale::GetLocale(uint32 index)
{
	if(index >= CountLocale()) return NULL;
	
	return((const char*)list.ItemAt(index));
}


uint32
SLocale::Locale() const
{
	return locale;
}


const char*
SLocale::GetString(const char* string, uint32 index)
{
	Lock();

	SStringHash *hash = (SStringHash*)Lookup(GetLocale(index));
	if(!hash)
	{
		Unlock();
		return string;
	}

	const char* found = (const char*)hash->Lookup(string);

	Unlock();

	if(!found) return string;
	else return found;
}


const char*
SLocale::GetString(const char* string)
{
	return GetString(string, Locale());
}


bool
SLocale::AddMessenger(BMessenger &msgr)
{
	Lock();

	if(manager)
	{
		bool ret = manager->AddMessenger(msgr);
		Unlock();
		return ret;
	}

	Unlock();

	return false;
}


bool
SLocale::RemoveMessenger(BMessenger &msgr)
{
	Lock();

	if(manager)
	{
		bool ret = manager->RemoveMessenger(msgr);
		Unlock();
		return ret;
	}

	Unlock();

	return false;
}
