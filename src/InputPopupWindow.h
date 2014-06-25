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
// Input Popup Window


#ifndef __SINPUT_POPUP_WINDOW_H__
#define __SINPUT_POPUP_WINDOW_H__

#include <SupportDefs.h>
#include <TextView.h>
#include <Window.h>

#include <besavager/DragBar.h>

class SInputAddOn;

class SInputPopupWindow : public BWindow {
public:
	SInputPopupWindow(SInputAddOn *input_addon);
	virtual ~SInputPopupWindow();

	virtual void MessageReceived(BMessage *message);
	virtual void FrameMoved(BPoint new_position);
	virtual void Show();

	void SetFontSize(float fsize);

	virtual void ScreenChanged(BRect screen_size, color_space depth);

private:
	SDragBar *dragbar;
	BTextView *text;

	SInputAddOn *input_addon;
};

#endif /* __SINPUT_POPUP_WINDOW_H__ */
