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
// MenuItem

#ifndef __SAVAGER_MENU_ITEM_H__
#define __SAVAGER_MENU_ITEM_H__

#include <Bitmap.h>
#include <MenuItem.h>

class SMenuItem : public BMenuItem {
public:
	SMenuItem(const BBitmap *bitmap,
			  const char *label,
			  BMessage *message,
			  char shortcut = 0,
			  uint32 modifiers = 0);
	SMenuItem(const char **xpm_data,
			  const char *label,
			  BMessage *message,
			  char shortcut = 0,
			  uint32 modifiers = 0);

	SMenuItem(const BBitmap *bitmap, BMenu *menu, BMessage *message = NULL, const char *label = NULL);
	SMenuItem(const char **xpm_data, BMenu *menu, BMessage *message = NULL, const char *label = NULL);


	virtual ~SMenuItem();

	virtual	void DrawContent();
	virtual	void Highlight(bool on);
	virtual	void GetContentSize(float *width, float *height);

private:
	BBitmap *bitmap;
	char *label;

	void DrawBitmap(bool highlight);
};


#endif /* __SAVAGER_MENU_ITEM_H__ */

