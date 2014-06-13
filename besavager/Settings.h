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
// Settings

#ifndef __SAVAGER_SETTINGS_H__
#define __SAVAGER_SETTINGS_H__

#include <SupportDefs.h>
#include <FindDirectory.h>
#include <File.h>
#include <Rect.h>


#define S_SETTINGS_MAX_STRING_LENGTH	1024


class SSettings {
public:
	SSettings();
	SSettings(const char* appendString, directory_which which, bool create_default_if_not_exist = false);
	SSettings(const char* filename, bool create_default_if_not_exist = false);
	virtual ~SSettings();

	bool SetTo(const char* appendString, directory_which which, bool create_default_if_not_exist = false);
	bool SetTo(const char* filename, bool create_default_if_not_exist = false);

	bool IsValid() const;

	bool WriteBool(const char* name, bool value);
	bool WriteInt32(const char* name, int32 value);
	bool WritePoint(const char* name, BPoint point);
	bool WriteRect(const char* name, BRect rect);
	bool WriteString(const char* name, const char* string);

	bool ReadBool(const char* name, bool *value, bool *defaultValue = NULL);
	bool ReadInt32(const char* name, int32 *value, int32 *defaultValue = NULL);
	bool ReadPoint(const char* name, BPoint *point, BPoint *defaultPoint = NULL);
	bool ReadRect(const char* name, BRect *rect, BRect *defaultRect = NULL);

	// the string must to be "delete[]"
	bool ReadString(const char* name, char **string, const char *defaultString = NULL);

protected:
	BFile *file;
	virtual bool CreateDefault();
	virtual bool IsWriteable();
};


#endif /* __SAVAGER_SETTINGS_H__ */
