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
// Setting

#ifndef __SINPUT_SETTINGS_H__
#define __SINPUT_SETTINGS_H__

#include <besavager/Settings.h>

class SInputSettings : public SSettings {
public:
	SInputSettings();
	SInputSettings(const char* appendString, directory_which which, bool create_default_if_not_exist = true);
	SInputSettings(const char* filename, bool create_default_if_not_exist = true);
	virtual ~SInputSettings();

protected:
	virtual bool CreateDefault();
	virtual bool IsWriteable();
};


#endif /* __SINPUT_SETTINGS_H__ */
