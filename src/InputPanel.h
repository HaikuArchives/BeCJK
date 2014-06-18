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
// Input Panel


#ifndef __SINPUT_PANEL_H__
#define __SINPUT_PANEL_H__

#include <SupportDefs.h>

#include <besavager/Tooltips.h>
#include <besavager/Window.h>
#include <besavager/DragBar.h>
#include <besavager/BitmapButton.h>

class SInputAddOn;

class SInputPanel : public SWindow {
public:
	SInputPanel(SInputAddOn *addon);
	virtual ~SInputPanel();

	virtual void MessageReceived(BMessage *message);

	virtual bool SupportNotActivatedTooltips() const;

	virtual	void FrameMoved(BPoint new_position);
	virtual void ScreenChanged(BRect screen_size, color_space depth);

private:
	SInputAddOn *addon;
	SDragBar *dragbar;
	BView *main_view;

	SBitmapButton *btn_cjk_en;
	SBitmapButton *btn_modules;
	SBitmapButton *btn_corner;
	SBitmapButton *btn_punct;
//	SBitmapButton *btn_keyboard;
	SBitmapButton *btn_info;

	BBitmap *default_module_icon;
};

#endif /* __SINPUT_PANEL_H__ */
