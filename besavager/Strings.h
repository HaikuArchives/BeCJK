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

#ifndef __SAVAGER_STRINGS_H__
#define __SAVAGER_STRINGS_H__

#include <SupportDefs.h>

#define UTF8_LENGTH(Char)              \
  ((Char) < 0x80 ? 1 :                 \
   ((Char) < 0x800 ? 2 :               \
    ((Char) < 0x10000 ? 3 :            \
     ((Char) < 0x200000 ? 4 :          \
      ((Char) < 0x4000000 ? 5 : 6)))))

#define UNICODE_VALID(Char)                   \
    ((Char) < 0x110000 &&                     \
     ((Char) < 0xD800 || (Char) >= 0xE000) && \
     (Char) != 0xFFFE && (Char) != 0xFFFF)

#ifdef __cplusplus
extern "C"{
#endif

const char* __utf8_next_char(const char *string);
const char* __utf8_prev_char(const char *string);

uint32 __utf8_strlen(const char *string, uint32 max = (uint32)((int32)-1));

#ifdef __cplusplus
}


#include <String.h>

//Sstrdup(): like strdup(), but the result must be free by "delete[]"
char *Sstrdup(const char* src, uint32 length = (uint32)((int32)-1));


class SStringArray;

class SString : public BString {
public:
	SString();
	SString(const char *);
	SString(const BString &);
	SString(const char *, int32 maxLength);

	// IsNumber: whether it is decimalism number like +1.23,-12,12.21,-.12,+.98
	bool IsNumber() const;

	// IsInteger: whether it is integer in decimalism like +1.000,-2.0,1,2,+1,-2
	bool IsInteger() const;

	// IsDecimal: whether it is decimal in decimalism like +1.2343,-0.23,12.43,-.23,+.23
	bool IsDecimal() const;

	// if IsNumber() is TRUE, it convert the string to double then return TRUE, else do nothing and return FALSE
	bool GetDouble(double *value) const;

	// SPlit: splits a string into a maximum of max_tokens pieces, using the given delimiter.
	//        If max_tokens is reached, the remainder of string is appended to the last token
	// Returns : a newly-allocated array of strings
	SStringArray* Split(const char *delimiter, uint32 max_tokens = (uint32)((int32)-1) - 1) const;
	SStringArray* Split(const char delimiter, uint32 max_tokens = (uint32)((int32)-1) - 1) const;
};

#endif /* __cplusplus */

#endif /* __SAVAGER_STRINGS_H__ */
