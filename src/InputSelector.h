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
// Input Selector


#ifndef __SINPUT_SELECTOR_H__
#define __SINPUT_SELECTOR_H__

#include <besavager/Window.h>
#include <besavager/StringArray.h>
#include <besavager/StringView.h>
#include <besavager/BitmapView.h>
#include <besavager/BitmapButton.h>


class SInputAddOn;
class SInputSelectorStringView;

class SInputSelector : public SWindow {
public:
	SInputSelector(SInputAddOn *input_addon);
	virtual ~SInputSelector();

	virtual void MessageReceived(BMessage *message);

	virtual bool SupportNotActivatedTooltips() const;

	void SetItems(const char *origin_buffer,
				  const SStringArray *items,
				  const SStringArray *selkeys,
				  int32 select_item_index,
				  bool can_do_page_up,
				  bool can_do_page_down,
				  BPoint *left_top,
				  bool horizontal);

	void ResizeToFit();

	void SetFontSize(float fsize);

private:
	SInputSelectorStringView *text;

	SStringView *origin_text;

	SBitmapView *btv_cjk;
	SBitmapView *btv_module;
	SBitmapView *btv_corner;
	SBitmapView *btv_punct;

	SBitmapButton *btn_page_up;
	SBitmapButton *btn_page_down;

	SInputAddOn *input_addon;
};

#endif /* __SINPUT_SELECTOR_H__ */
