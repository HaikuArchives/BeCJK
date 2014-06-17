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
// String Array Functions

#include "StringArray.h"

SStringArray::SStringArray()
{
}


SStringArray::SStringArray(const char *string)
{
	AppendItem(string);
}


SStringArray::SStringArray(const SString &string)
{
	AppendItem(string);
}


SStringArray::SStringArray(const char **array)
{
	operator=(array);
}


SStringArray::SStringArray(const SStringArray &array)
{
	operator=(array);
}


SStringArray::~SStringArray()
{
	MakeEmpty();
}


void
SStringArray::MakeEmpty()
{
	if(!list.IsEmpty())
	{
		SString *data;
		for(int32 i = 0; (data = (SString*)list.ItemAt(i)) != NULL; i++) delete data;
		list.MakeEmpty();
	}
}


bool
SStringArray::IsEmpty() const
{
	return list.IsEmpty();
}


int32
SStringArray::CountItems() const
{
	return list.CountItems();
}


SStringArray&
SStringArray::operator=(const SStringArray &array)
{
	MakeEmpty();

	return AppendArray(array);
}


SStringArray&
SStringArray::operator=(const char **array)
{
	MakeEmpty();

	if(array)
	{
		while(*array)
		{
			AppendItem(*array);
			array++;
		}
	}

	return *this;
}


SStringArray&
SStringArray::operator+=(const char *string)
{
	AppendItem(string);
	return *this;
}


SStringArray&
SStringArray::operator+=(const SString &string)
{
	AppendItem(string);
	return *this;
}


SStringArray&
SStringArray::operator+=(const SStringArray &array)
{
	return AppendArray(array);
}


SStringArray&
SStringArray::operator+=(const char **array)
{
	if(array)
	{
		while(*array)
		{
			AppendItem(*array);
			array++;
		}
	}

	return *this;
}


bool
SStringArray::AppendItem(const char *item)
{
	if(!item) return false;

	SString *data = new SString(item);

	if(!data) return false;

	if(!list.AddItem((void*)data))
	{
		delete data;
		return false;
	}

	return true;
}


bool
SStringArray::AppendItem(const SString &item)
{
	SString *data = new SString(item);

	if(!data) return false;

	if(!list.AddItem((void*)data))
	{
		delete data;
		return false;
	}

	return true;
}


SStringArray&
SStringArray::AppendArray(const SStringArray &array)
{
	if(!array.list.IsEmpty())
	{
		const SString* data;
		for(int32 i = 0; (data = (const SString*)array.list.ItemAt(i)) != NULL; i++) AppendItem(*data);
	}

	return *this;
}


const SString*
SStringArray::ItemAt(int32 index) const
{
	const SString *string = (const SString*)list.ItemAt(index);

	return string;
}


const SString*
SStringArray::FirstItem() const
{
	const SString *string = (const SString*)list.FirstItem();

	return string;
}


const SString*
SStringArray::LastItem() const
{
	const SString *string = (const SString*)list.LastItem();

	return string;
}


bool
SStringArray::RemoveItem(int32 index)
{
	SString *string = (SString*)list.RemoveItem(index);

	if(string)
	{
		delete string;
		return true;
	}

	return false;
}


SStringArray&
SStringArray::RemoveItems(int32 index, int32 count)
{
	for(int32 i = index; i < index + count; i++)
	{
		SString *string = (SString*)list.ItemAt(i);
		if(string != NULL)
		{
			delete string;
			list.ReplaceItem(i, NULL);
		}
	}

	list.RemoveItems(index, count);

	return *this;
}


bool
SStringArray::ReplaceItem(int32 index, const char *string)
{
	SString *data = (SString*)list.ItemAt(index);

	if(data)
	{
		data->SetTo(string);
		return true;
	}

	return false;
}


bool
SStringArray::ReplaceItem(int32 index, const SString &string)
{
	SString *data = (SString*)list.ItemAt(index);

	if(data)
	{
		*data = string;
		return true;
	}
	
	return false;
}


SStringArray&
SStringArray::SortItems(int (*cmp)(const SString*, const SString*))
{
	list.SortItems((int (*)(const void*, const void*))cmp);

	return *this;
}


bool
SStringArray::SwapItems(int32 indexA, int32 indexB)
{
	if(indexA != indexB) return list.SwapItems(indexA, indexB);

	return true;
}


bool
SStringArray::MoveItem(int32 fromIndex, int32 toIndex)
{
	if(fromIndex != toIndex) return list.MoveItem(fromIndex, toIndex);

	return true;
}


int32
SStringArray::FindString(const char *string, int32 startIndex, bool all_equal) const
{
	if(!string) return -1;
	if(startIndex > CountItems() - 1) return -1;

	for(int32 i = startIndex; i < CountItems(); i++)
	{
		const SString *str = ItemAt(i);
		if(str)
		{
			if(!all_equal)
			{
				if(str->FindFirst(string) >= 0) return i;
			}
			else
			{
				if(*str == string) return i;
			}
		}
	}

	return -1;
}


int32
SStringArray::IFindString(const char *string, int32 startIndex, bool all_equal) const
{
	if(!string) return -1;
	if(startIndex > CountItems() - 1) return -1;

	for(int32 i = startIndex; i < CountItems(); i++)
	{
		const SString *str = ItemAt(i);
		if(str)
		{
			if(!all_equal)
			{
				if(str->IFindFirst(string) >= 0) return i;
			}
			else
			{
				if(str->ICompare(string) == 0) return i;
			}
		}
	}

	return -1;
}
