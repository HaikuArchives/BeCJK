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

#include <string.h>
#include <bsd_mem.h>

#include <storage/Path.h>
#include <storage/Directory.h>

#include "Strings.h"
#include "Settings.h"

SSettings::SSettings()
{
	file = NULL;
}


SSettings::SSettings(const char* appendString, directory_which which, bool create_default_if_not_exist)
{
	file = NULL;
	SetTo(appendString, which, create_default_if_not_exist);
}


SSettings::SSettings(const char* filename, bool create_default_if_not_exist)
{
	file = NULL;
	SetTo(filename, create_default_if_not_exist);
}


SSettings::~SSettings()
{
	if(file) delete file;
}


bool
SSettings::SetTo(const char* appendString, directory_which which, bool create_default_if_not_exist)
{
	if(!appendString) return false;

	BPath path;
	find_directory(which, &path);
	path.Append(appendString);
	return SetTo(path.Path(), create_default_if_not_exist);
}


bool
SSettings::SetTo(const char* filename, bool create_default_if_not_exist)
{
	if(!filename) return false;

	bool needed_to_create_default = false;

	BFile *bfile = new BFile();
	if(!bfile) return false;

	if(bfile->SetTo(filename, B_READ_WRITE) != B_OK)
	{
		BDirectory bdir;
		if(bdir.CreateFile(filename, bfile, true) != B_OK)
		{
			delete bfile;
			return false;
		}

		if(bfile->SetTo(filename, B_READ_WRITE) != B_OK)
		{
			delete bfile;
			return false;
		}

		needed_to_create_default = true;
	}

	if(needed_to_create_default)
	{
		if(!CreateDefault())
		{
			delete bfile;
			return false;
		}
	}
	
	if(file) delete file;
	file = bfile;

	return true;
}


bool
SSettings::IsValid() const
{
	return(file != NULL);
}


bool
SSettings::CreateDefault()
{
	return true;
}


bool
SSettings::WriteBool(const char* name, bool value)
{
	if(!file || !name || !IsWriteable()) return false;

	if(file->WriteAttr(name, B_BOOL_TYPE, 0, &value, sizeof(bool)) < 0) return false;
	return true;
}


bool
SSettings::WriteInt32(const char* name, int32 value)
{
	if(!file || !name || !IsWriteable()) return false;

	if(file->WriteAttr(name, B_INT32_TYPE, 0, &value, sizeof(int32)) < 0) return false;
	return true;
}


bool
SSettings::WritePoint(const char* name, BPoint point)
{
	if(!file || !name || !IsWriteable()) return false;

	if(file->WriteAttr(name, B_POINT_TYPE, 0, &point, sizeof(BPoint)) < 0) return false;
	return true;
}


bool
SSettings::WriteRect(const char* name, BRect rect)
{
	if(!file || !name || !IsWriteable()) return false;

	if(file->WriteAttr(name, B_RECT_TYPE, 0, &rect, sizeof(BRect)) < 0) return false;
	return true;
}


bool
SSettings::WriteString(const char* name, const char* string)
{
	if(!file || !name || !string || !IsWriteable()) return false;
	if(strlen(string) > S_SETTINGS_MAX_STRING_LENGTH) return false;

	char buf[S_SETTINGS_MAX_STRING_LENGTH + 1];
	strcpy(buf, string);
	buf[strlen(string)] = 0;

	if(file->WriteAttr(name, B_STRING_TYPE, 0, buf, sizeof(char) * (strlen(buf) + 1)) < 0) return false;
	return true;
}


bool
SSettings::ReadBool(const char* name, bool *value, bool *defaultValue)
{
	if(!file || !name || !value) return false;

	status_t err = file->ReadAttr(name, B_BOOL_TYPE, 0, value, sizeof(bool));
	if(err == B_ENTRY_NOT_FOUND)
	{
		if(!IsWriteable()) return false;
		if(!defaultValue) return false;

		*value = *defaultValue;

		if(file->WriteAttr(name, B_BOOL_TYPE, 0, defaultValue, sizeof(bool)) < 0)
			return false;
	}
	else if(err < 0)
	{
		return false;
	}

	return true;
}


bool
SSettings::ReadInt32(const char* name, int32 *value, int32 *defaultValue)
{
	if(!file || !name || !value) return false;

	status_t err = file->ReadAttr(name, B_INT32_TYPE, 0, value, sizeof(int32));
	if(err == B_ENTRY_NOT_FOUND)
	{
		if(!IsWriteable()) return false;
		if(!defaultValue) return false;

		*value = *defaultValue;

		if(file->WriteAttr(name, B_INT32_TYPE, 0, defaultValue, sizeof(int32)) < 0)
			return false;
	}
	else if(err < 0)
	{
		return false;
	}

	return true;
}


bool
SSettings::ReadPoint(const char* name, BPoint *point, BPoint *defaultPoint)
{
	if(!file || !name || !point) return false;

	status_t err = file->ReadAttr(name, B_POINT_TYPE, 0, point, sizeof(BPoint));
	if(err == B_ENTRY_NOT_FOUND)
	{
		if(!IsWriteable()) return false;
		if(!defaultPoint) return false;

		*point = *defaultPoint;

		if(file->WriteAttr(name, B_POINT_TYPE, 0, defaultPoint, sizeof(BPoint)) < 0)
			return false;
	}
	else if(err < 0)
	{
		return false;
	}

	return true;
}


bool
SSettings::ReadRect(const char* name, BRect *rect, BRect *defaultRect)
{
	if(!file || !name || !rect) return false;

	status_t err = file->ReadAttr(name, B_RECT_TYPE, 0, rect, sizeof(BRect));
	if(err == B_ENTRY_NOT_FOUND)
	{
		if(!IsWriteable()) return false;
		if(!defaultRect) return false;

		*rect = *defaultRect;

		if(file->WriteAttr(name, B_RECT_TYPE, 0, defaultRect, sizeof(BRect)) < 0)
			return false;
	}
	else if(err < 0)
	{
		return false;
	}

	return true;
}


bool
SSettings::ReadString(const char* name, char **string, const char *defaultString)
{
	if(!file || !name || !string) return false;

	char buf[S_SETTINGS_MAX_STRING_LENGTH + 1];
	bzero(buf, sizeof(buf));

	status_t err = file->ReadAttr(name, B_STRING_TYPE, 0, buf, sizeof(buf));
	if(err == B_ENTRY_NOT_FOUND)
	{
		if(!IsWriteable()) return false;
		if(!defaultString || strlen(defaultString) > S_SETTINGS_MAX_STRING_LENGTH) return false;

		*string = Sstrdup(defaultString);

		strcpy(buf, defaultString);
		buf[strlen(defaultString)] = 0;

		if(file->WriteAttr(name, B_STRING_TYPE, 0, buf, sizeof(char) * (strlen(buf) + 1)) < 0)
			return false;
	}
	else if(err < 0)
	{
		return false;
	}

	*string = Sstrdup(buf);

	return true;
}


bool
SSettings::IsWriteable()
{
	return true;
}

