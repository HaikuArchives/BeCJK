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
// DragBar for Window

#ifndef __SAVAGER_DRAGBAR_H__
#define __SAVAGER_DRAGBAR_H__

#include <View.h>
#include <besavager/Window.h>

enum {
	S_DRAG_BAR_TOP = 1,
	S_DRAG_BAR_BOTTOM = 2,
	S_DRAG_BAR_LEFT = 3,
	S_DRAG_BAR_RIGHT = 4,
};


class SDragBar : public BView {
public:
	SDragBar(BRect window_bounds,
			 const char *name,
			 int32 position = S_DRAG_BAR_LEFT,
			 const char *tooltips = NULL,
			 bool within_screen = false);
	virtual ~SDragBar();

	virtual void SetTooltips(const char *tooltips);

	virtual	void Draw(BRect updateRect);

	// window coordinates
	// if "Window() == NULL" it return the BRect.IsValid() == false
	virtual BRect ChildBounds();

	virtual	void MouseDown(BPoint where);
	virtual	void MouseUp(BPoint where);
	virtual	void MouseMoved(BPoint where, uint32 code, const BMessage *msg);

	virtual void GetPreferredSize(float *width, float *height);
	virtual	void WindowActivated(bool state);

	void SetWithInScreen(bool within);

private:
	BRect BarBounds();
	void DrawBarBounds();

	bool tracking;
	BPoint old_position;

	int32 position;
	
	char *tooltips;
	
	BRect window_bounds;

	BCursor *cursor;

	bool within_screen;
};

#endif /* __SAVAGER_DRAGBAR_H__ */
