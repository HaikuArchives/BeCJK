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
// Separator View

#ifndef __SAVAGER_SEPARATOR_H__
#define __SAVAGER_SEPARATOR_H__

#include <View.h>

enum {
	S_SEPARATOR_H = 0, // horizontal
	S_SEPARATOR_V = 1, // vertical
};

class SSeparator : public BView {
public:
	SSeparator(BRect frame,
			   const char *name,
			   int32 separator_style,
			   uint32 resizeMask,
			   uint32 flags = B_WILL_DRAW);

	virtual void Draw(BRect updateRect);
	virtual void AttachedToWindow();
	virtual	void GetPreferredSize(float *width, float *height);

private:
	int32 style;
};

#endif /* __SAVAGER_SEPARATOR_H__ */
