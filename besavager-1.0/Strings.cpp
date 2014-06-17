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
// String Functions
// The Unicode functions were yanked from GLIB


#include <string.h>
#include <bsd_mem.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/param.h>

#include "Strings.h"
#include "StringArray.h"


char* Sstrdup(const char* src, uint32 length)
{
	if(!src) return NULL;

	char* dest;

	uint32 len = 0;	

	if(length == (uint32)((int32)-1))
		len = strlen(src);
	else
		len = MIN(length, strlen(src));

	dest = new char[len + 1];

	if(!dest) return NULL;

	if(len != 0) strncpy(dest, src, sizeof(char) * len);
	dest[len] = 0;

	return dest;
}


static const char __utf8_skip[256] = {
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
	3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,6,6,1,1
};


extern "C" {

const char* __utf8_next_char(const char *string)
{
	if(!string) return NULL;
	else return(((string) + __utf8_skip[*(uchar*)(string)]));
}


const char* __utf8_prev_char(const char *string)
{
	if(!string) return NULL;

	while(TRUE)
	{
		string--;
		if((*string & 0xc0) != 0x80) return string;
	}
}


uint32 __utf8_strlen(const char *string, uint32 max)
{
	uint32 length = 0;
	const char* start = string;

	if(max == (uint32)((int32)-1))
	{
		while(*string)
		{
			string = __utf8_next_char(string);
			length++;
		}
	}
	else
	{
		if(max == 0 || !string) return 0;
      
		string = __utf8_next_char(string);          

		while((uint32)(string - start) < max && *string)
		{
			length++;
			string = __utf8_next_char(string);
		}

		/* only do the last len increment if we got a complete
		 * char (don't count partial chars)
		 */
		if((uint32)(string - start) == max) length++;
	}

	return length;
}

} /* extern "C" */


SString::SString()
	: BString()
{
}


SString::SString(const char *string)
	: BString(string)
{
}


SString::SString(const BString &string)
	: BString(string)
{
}


SString::SString(const char *string, int32 maxLength)
	: BString(string, maxLength)
{
}


bool
SString::IsNumber() const
{
	const char *s = String();
	int32 length = Length();

	if(s == NULL) return false;
	if(length < 1) return false;

	if(s[0] == '-' || s[0] == '+')
	{
		if(length < 2) return false;
		if(strchr(&s[1], '-') != NULL || strchr(&s[1], '+') != NULL) return false; // double '-' or '+'
	}

	const char *tmp = strchr(s, '.');
	if(tmp != NULL)
	{
		tmp++;
		if(strchr(tmp, '.') != NULL) return false; // double '.'
	}

	for(int32 i = 0; i < length; i++)
	{
		if(!(isdigit(s[i]) || s[i] == '-' || s[i] == '.' || s[i] == '+')) return false;
	}

	return true;
}


bool
SString::IsInteger() const
{
	if(!IsNumber()) return false;

	const char *s = strchr(String(), '.');

	if(s == NULL) return true;

	if(strchr(s, '.') != NULL)
	{
		s++;
		int32 length = strlen(s);
		if(length > 0)
		{
			for(int32 i = 0; i < length; i++)
			{
				if(s[i] != '0') return false;
			}
		}
	}

	return true;
}


bool
SString::IsDecimal() const
{
	if(!IsNumber()) return false;
	if(!String()) return false;

	return(!IsInteger());
}


bool
SString::GetDouble(double *value) const
{
	if(!value) return false;
	if(!IsNumber()) return false;

	*value = strtod(String(), NULL);

	return true;
}


SStringArray*
SString::Split(const char *delimiter, uint32 max_tokens) const
{
	if(!delimiter) return NULL;
	if(strlen(delimiter) == 0) return NULL;
	if(max_tokens == (uint32)((int32)-1)) return NULL;

	SStringArray *array = new SStringArray();

	int32 offset = 0;
	int32 found = 0;
	uint32 i = 0;

	for(;;)
	{
		if(offset >= Length() || i >= max_tokens) break;

		found = FindFirst(delimiter, offset);

		if(found == B_ERROR) found = Length();
		i++;

		SString line;
		int32 length;

		if(i == max_tokens)
			length = Length() - 1 - offset;
		else
			length = found - offset;

		if(length > 0) line.SetTo(String() + offset, length);
		
		*array += line;

		offset = found + strlen(delimiter);
	}

	if(array->CountItems() <= 0)
	{
		delete array;
		array = NULL;
	}

	return array;
}


SStringArray*
SString::Split(const char delimiter, uint32 max_tokens) const
{
	char string[2];
	bzero(string, sizeof(string));
	string[0] = delimiter;
	return Split(string, max_tokens);
}

