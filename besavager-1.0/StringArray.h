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

#ifndef __SAVAGER_STRING_ARRAY_H__
#define __SAVAGER_STRING_ARRAY_H__

#include <List.h>
#include <besavager/Strings.h>

class SStringArray {
public:
	SStringArray();

	SStringArray(const char *string); // string: first item
	SStringArray(const SString &string);

	SStringArray(const char **array); // array: NULL-terminated array of strings
	SStringArray(const SStringArray &array);

	~SStringArray();

	SStringArray&	operator=(const char **array);
	SStringArray&	operator=(const SStringArray &array);

	SStringArray&	operator+=(const char *string);
	SStringArray&	operator+=(const SString &string);
	SStringArray&	operator+=(const char **array);
	SStringArray&	operator+=(const SStringArray &array);

	bool			AppendItem(const char *item);
	bool			AppendItem(const SString &item);
	SStringArray&	AppendArray(const SStringArray &array);

	bool			RemoveItem(int32 index);
	SStringArray&	RemoveItems(int32 index, int32 count);

	bool			ReplaceItem(int32 index, const char *string);
	bool			ReplaceItem(int32 index, const SString &string);

	SStringArray&	SortItems(int (*cmp)(const SString*, const SString*));
	bool			SwapItems(int32 indexA, int32 indexB);
	bool			MoveItem(int32 fromIndex, int32 toIndex);

	bool IsEmpty() const;
	void MakeEmpty();

	const SString* ItemAt(int32 index) const;
	const SString* FirstItem() const;
	const SString* LastItem() const;

	int32 CountItems() const;

	// return value: string index if found, else return -1
	int32 FindString(const char *string, int32 startIndex = 0, bool all_equal = true) const;
	int32 IFindString(const char *string, int32 startIndex = 0, bool all_equal = true) const;

private:
	BList list;
};


#endif /* __SAVAGER_STRING_ARRAY_H__ */
