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

#include <Messenger.h>
#include <Message.h>
#include <Screen.h>

#include <besavager/Autolock.h>
#include <besavager/Separator.h>

#include "InputAddOn.h"
#include "InputPanel.h"
#include "InputPopupWindow.h"
#include "InputSelector.h"
#include "BeCJKInputModule.h"
#include "Icons.h"
#include "Locale.h"
#include "AllMessages.h"


#define BTN_PAGE_UP_TOOLTIPS			N_("Page Up")
#define BTN_PAGE_DOWN_TOOLTIPS			N_("Page Down")


class SInputSelectorStringView : public SStringView {
public:
	SInputSelectorStringView(BRect bounds,
							 const char *name,
							 const char *text,
							 const s_string_view_color *colors = NULL,
							 int32 ncolors = 0,
							 const char *tooltips = NULL,
							 uint32 resizeFlags = B_FOLLOW_LEFT | B_FOLLOW_TOP,
							 uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
	virtual ~SInputSelectorStringView();


	void EmptyItemRects();
	void InsertItemRect(BRect rect, int32 start, int32 end);

	bool ItemRectsIsEmpty() const;
	int32 CountItemRects() const;
	BRect ItemRectAt(int32 index, int32 *start = NULL, int32 *end = NULL) const;

	void SetSelected(int32 index);

	virtual	void MouseDown(BPoint pt);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

protected:
	virtual void DrawHighlight(BView *view, int32 startOffset, int32 endOffset);


private:
	typedef struct ___item_rect___
	{
		BRect rect;
		int32 start;
		int32 end;
	} ___item_rect___;

	BList list;
	int32 selected;
};


SInputSelectorStringView::SInputSelectorStringView(BRect bounds,
												   const char *name,
												   const char *text,
												   const s_string_view_color *colors,
												   int32 ncolors,
												   const char *tooltips,
												   uint32 resizeFlags,
												   uint32 flags)
	: SStringView(bounds, name, text, colors, ncolors, tooltips, resizeFlags, flags), selected(-1)
{
}


SInputSelectorStringView::~SInputSelectorStringView()
{
	EmptyItemRects();
}


void
SInputSelectorStringView::DrawHighlight(BView *view, int32 startOffset, int32 endOffset)
{
	if(!view) return;

	BRegion region = TextRegion(startOffset, endOffset);
	if(!region.Frame().IsValid()) return;

	view->PushState();
	view->SetHighColor(255, 203, 0, 255);
	view->SetDrawingMode(B_OP_COPY);
	view->FillRoundRect(region.Frame(), 2, 2);
	view->SetHighColor(0, 0, 0, 255);
	view->StrokeRoundRect(region.Frame(), 2, 2);
	view->PopState();

	DrawText(view, startOffset, endOffset);
}


void
SInputSelectorStringView::EmptyItemRects()
{
	if(!list.IsEmpty())
	{
		___item_rect___ *data;
		for(int32 i = 0; (data = (___item_rect___*)list.ItemAt(i)) != NULL; i++) delete data;
		list.MakeEmpty();
	}
}


void
SInputSelectorStringView::InsertItemRect(BRect rect, int32 start, int32 end)
{
	if(!rect.IsValid()) return;

	___item_rect___ *data = new ___item_rect___;

	if(!data) return;

	data->rect = rect;
	data->start = start;
	data->end = end;

	if(!list.AddItem((void*)data)) delete data;
}


bool
SInputSelectorStringView::ItemRectsIsEmpty() const
{
	return list.IsEmpty();
}


int32
SInputSelectorStringView::CountItemRects() const
{
	return list.CountItems();
}


BRect
SInputSelectorStringView::ItemRectAt(int32 index, int32 *start, int32 *end) const
{
	const ___item_rect___ *data = (const ___item_rect___*)list.ItemAt(index);

	if(data)
	{
		if(start) *start = data->start;
		if(end) *end = data->end;
		return data->rect;
	}
	else
		return BRect();
}


void
SInputSelectorStringView::SetSelected(int32 index)
{
	selected = index;
}


void
SInputSelectorStringView::MouseDown(BPoint pt)
{
	SStringView::MouseDown(pt);

	if(!ItemRectsIsEmpty())
	{
		for(int32 i = 0; i < CountItemRects(); i++)
		{
			int32 start = -1, end = -1;
			BRect rect = ItemRectAt(i, &start, &end);
			if(!rect.IsValid()) continue;

			if(rect.Contains(pt))
			{
				BMessenger msgr(NULL, Window());
				BMessage msg(S_INPUT_SELECTOR_SELECTED_MSG);
				msg.AddInt32("sinput:selected", i);
				msg.AddInt32("sinput:start", start);
				msg.AddInt32("sinput:end", end);
				msgr.SendMessage(&msg);
				break;
			}
		}
	}
}


void
SInputSelectorStringView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	SStringView::MouseMoved(pt, code, msg);

	uint32 btns;
	GetMouse(&pt, &btns, false);

	if(Bounds().Contains(pt))
	{
		if(!ItemRectsIsEmpty())
		{
			for(int32 i = 0; i < CountItemRects(); i++)
			{
				int32 start = -1, end = -1;
				BRect rect = ItemRectAt(i, &start, &end);
				if(!rect.IsValid()) continue;

				if(rect.Contains(pt))
				{
					if(i != selected)
					{
						BMessenger msgr(NULL, Window());
						BMessage msg(S_INPUT_SELECTOR_CHANGED_MSG);
						msg.AddInt32("sinput:selected", i);
						msg.AddInt32("sinput:start", start);
						msg.AddInt32("sinput:end", end);
						msgr.SendMessage(&msg);
					}

					break;
				}
			}
		}
	}
}



SInputSelector::SInputSelector(SInputAddOn *input_addon)
	: SWindow(BRect(0, 0, 400, 50), "BeCJK Input Selector", B_BORDERED_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_AVOID_FOCUS, B_ALL_WORKSPACES)
{
	SInputSelector::input_addon = input_addon;

	SetSizeLimits(0, 10000, 0, 10000);

	BView *main_view = new BView(Bounds(), NULL, B_FOLLOW_ALL, 0);
	main_view->SetViewColor(255, 255, 255, 255);

	btv_cjk = new SBitmapView(BRect(1, 1, 21, 21), NULL, (const BBitmap*)NULL);
	main_view->AddChild(btv_cjk);

	btv_module = new SBitmapView(BRect(23, 1, 43, 21), NULL, (const BBitmap*)NULL);
	main_view->AddChild(btv_module);

	btv_corner = new SBitmapView(BRect(45, 1, 65, 21), NULL, (const BBitmap*)NULL);
	main_view->AddChild(btv_corner);

	btv_punct = new SBitmapView(BRect(67, 1, 87, 21), NULL, (const BBitmap*)NULL);
	main_view->AddChild(btv_punct);

	btn_page_up = new SBitmapButton(BRect(89, 1, 109, 21), NULL, chb_icon_left, NULL,
								   _(BTN_PAGE_UP_TOOLTIPS), new BMessage(S_INPUT_SELECTOR_PAGE_UP_MSG));
	btn_page_up->SetEnabled(false);
	main_view->AddChild(btn_page_up);

	btn_page_down = new SBitmapButton(BRect(111, 1, 131, 21), NULL, chb_icon_right, NULL,
								   _(BTN_PAGE_DOWN_TOOLTIPS), new BMessage(S_INPUT_SELECTOR_PAGE_DOWN_MSG));
	btn_page_down->SetEnabled(false);
	main_view->AddChild(btn_page_down);

	BRect frame;
	frame.left = 140;
	frame.top = 1;
	frame.right = Bounds().right - 1;
	frame.bottom = 21;
	origin_text = new SStringView(frame, NULL, "", NULL, 0, NULL);
	origin_text->SetHighColor(0, 0, 0, 255);
	origin_text->SetFont(be_bold_font);
	origin_text->SetFontSize(12);
	main_view->AddChild(origin_text);


	frame = Bounds().InsetByCopy(1, 1);
	frame.top = 22;
	frame.bottom = 23;
	SSeparator *sep = new SSeparator(frame, NULL, S_SEPARATOR_H, B_FOLLOW_LEFT_RIGHT);
	main_view->AddChild(sep);
	
	frame = Bounds().InsetByCopy(1, 1);
	frame.top += 24;

	text = new SInputSelectorStringView(frame, NULL, NULL, NULL, 0, NULL, B_FOLLOW_NONE);
	main_view->AddChild(text);

	AddChild(main_view);

	RemoveShortcut((uint32)'W', (uint32)B_COMMAND_KEY);
	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);

	SetFontSize(input_addon->font_size);

	Run();
}


SInputSelector::~SInputSelector()
{
}


void
SInputSelector::MessageReceived(BMessage *message)
{
	if(!message) return;

	switch(message->what)
	{
		case S_SWITCH_CORNER_MSG:
		case S_SWITCH_PUNCT_MSG:
			if(!input_addon) break;

			input_addon->Lock();

			if(input_addon->input_addon_looper)
			{
				BMessenger msgr(NULL, (const BLooper*)input_addon->input_addon_looper);
				msgr.SendMessage(message);
			}

			input_addon->Unlock();
			break;

		case S_MODULE_CHANGED_MSG:
			{
				if(!input_addon) break;

				input_addon->Lock();

				if(!input_addon->module)
				{
					btv_cjk->SetBitmap(chb_icon_unknown);

					btv_module->SetBitmap((const BBitmap*)NULL);
					btv_module->SetTooltips(NULL);

					btv_corner->SetBitmap((const BBitmap*)NULL);
					btv_corner->SetTooltips(NULL);

					btv_punct->SetBitmap((const BBitmap*)NULL);
					btv_punct->SetTooltips(NULL);
				}
				else
				{
					s_input_encoding encoding = input_addon->module->Encoding();

					switch(encoding)
					{
						case ENCODING_SIMPLIFIED_CHINESE:
						case LOCALE_TRADITIONAL_CHINESE:
							btv_cjk->SetBitmap(chb_icon_chinese);
							break;

						case ENCODING_JAPANESE:
							btv_cjk->SetBitmap(chb_icon_japanese);
							break;

						case ENCODING_KOREAN:
							btv_cjk->SetBitmap(chb_icon_korean);
							break;
		
						default:
							btv_cjk->SetBitmap(chb_icon_unknown);
					}


					if(input_addon->module->Icon())
						btv_module->SetBitmap(input_addon->module->Icon());
					else
						btv_module->SetBitmap(chb_icon_default_module);

					btv_module->SetTooltips(input_addon->module->Name() ? input_addon->module->Name() : _("Unknown"));

					if(input_addon->corner_all)
					{
						btv_corner->SetBitmap(chb_icon_corner_all);
						btv_corner->SetTooltips(_("Full Width"));
					}
					else
					{
						btv_corner->SetBitmap(chb_icon_corner_half);
						btv_corner->SetTooltips(_("Half Width"));
					}

					if(input_addon->punct_cjk)
					{
						btv_punct->SetBitmap(chb_icon_punct_cjk);
						btv_punct->SetTooltips(_("CJK Punctuation"));
					}
					else
					{
						btv_punct->SetBitmap(chb_icon_punct_en);
						btv_punct->SetTooltips(_("English Punctuation"));
					}
				}

				UpdateIfNeeded();

				input_addon->Unlock();
			}
			break;

		case S_LOCALE_CHANGED:
			{
				btn_page_up->SetTooltips(_(BTN_PAGE_UP_TOOLTIPS));
				btn_page_down->SetTooltips(_(BTN_PAGE_DOWN_TOOLTIPS));

				if(!input_addon) break;

				input_addon->Lock();

				if(!input_addon->module)
				{
					btv_module->SetTooltips(NULL);
					btv_corner->SetTooltips(NULL);
					btv_punct->SetTooltips(NULL);
				}
				else
				{
					btv_module->SetTooltips(input_addon->module->Name() ? input_addon->module->Name() : _("Unknown"));

					if(input_addon->corner_all)
						btv_corner->SetTooltips(_("Full Width"));
					else
						btv_corner->SetTooltips(_("Half Width"));

					if(input_addon->punct_cjk)
						btv_punct->SetTooltips(_("CJK Punctuation"));
					else
						btv_punct->SetTooltips(_("English Punctuation"));
				}

				input_addon->Unlock();
			}
			break;

		case S_INPUT_SELECTOR_CHANGED_MSG:
			{
				if(!input_addon) break;

				SAutolock<SInputAddOn> autolock(input_addon);

				if(!input_addon->module) break;

				int32 selected = -1, start = -1, end = -1;
				message->FindInt32("sinput:selected", &selected);
				message->FindInt32("sinput:start", &start);
				message->FindInt32("sinput:end", &end);
				if(selected < 0 || start < 0 || end < 0) break;
				
				if(input_addon->module->ItemChangedFromSelector(selected))
				{
					text->SetSelected(selected);
					text->Highlight(start, end);
				}
			}
			break;

		case S_INPUT_SELECTOR_SELECTED_MSG:
			{
				if(!input_addon) break;

				SAutolock<SInputAddOn> autolock(input_addon);

				if(!input_addon->module) break;

				int32 selected = -1, start = -1, end = -1;
				message->FindInt32("sinput:selected", &selected);
				message->FindInt32("sinput:start", &start);
				message->FindInt32("sinput:end", &end);
				if(selected < 0 || start < 0 || end < 0) break;

				if(input_addon->module->ItemSelectedFromSelector(selected)) input_addon->Refresh();
			}
			break;

		case S_INPUT_SELECTOR_PAGE_UP_MSG:
			{
				if(!input_addon) break;

				SAutolock<SInputAddOn> autolock(input_addon);

				if(!input_addon->module) break;

				if(input_addon->module->PageUp()) input_addon->Refresh();
			}
			break;

		case S_INPUT_SELECTOR_PAGE_DOWN_MSG:
			{
				if(!input_addon) break;

				SAutolock<SInputAddOn> autolock(input_addon);

				if(!input_addon->module) break;

				if(input_addon->module->PageDown()) input_addon->Refresh();
			}
			break;

		default:
			SWindow::MessageReceived(message);
	}
}


void
SInputSelector::SetItems(const char *origin_buffer,
						 const SStringArray *items,
						 const SStringArray *selkeys,
						 int32 select_item_index,
						 bool can_do_page_up,
						 bool can_do_page_down,
						 BPoint *left_top,
						 bool horizontal)
{
	origin_text->SetText(origin_buffer);

	text->EmptyItemRects();
	text->SetSelected(select_item_index);

	if(can_do_page_up)
	{
		if(btn_page_up->IsHidden()) btn_page_up->Show();
		if(!btn_page_up->IsEnabled()) btn_page_up->SetEnabled(true);
	}
	else
	{
		if(btn_page_up->IsEnabled()) btn_page_up->SetEnabled(false);
		if(!btn_page_up->IsHidden()) btn_page_up->Hide();
	}

	if(can_do_page_down)
	{
		if(btn_page_down->IsHidden()) btn_page_down->Show();
		if(!btn_page_down->IsEnabled()) btn_page_down->SetEnabled(true);
	}
	else
	{
		if(btn_page_down->IsEnabled()) btn_page_down->SetEnabled(false);
		if(!btn_page_down->IsHidden()) btn_page_down->Hide();
	}


	if(items)
	{
		int32 start = -1, end = -1;

		s_string_view_color *colors = NULL;
		int32 ncolors = 0;

		if(items->CountItems() > 0 && selkeys)
		{
			if(items->CountItems() == selkeys->CountItems())
				colors = new s_string_view_color[items->CountItems() * 2];
		}

		SString tmp(" ");
		for(int32 i = 0; i < items->CountItems(); i++)
		{
			const SString *item = items->ItemAt(i);
			const SString *item_selkey = (selkeys ? (items->CountItems() == selkeys->CountItems() ? selkeys->ItemAt(i) : NULL) : NULL);
			if(!item) break;

			int32 item_len = (item_selkey ? item_selkey->Length() + 1 + item->Length() : item->Length());

			int32 startOffset = (tmp.Length() > 0 ? tmp.Length() - 1 : 0);
			int32 endOffset = startOffset + item_len + 1;

			if(i == select_item_index)
			{
				start = startOffset;
				end = endOffset;
			}

			if(colors && item_selkey)
			{
				// highlight the select key with red
				s_rgb_color_setto(&(colors[ncolors].color), 203, 0, 51, 255);
				colors[ncolors].start_offset = tmp.Length() > 0 ? tmp.Length() - 1: 0;
				colors[ncolors].end_offset = colors[ncolors].start_offset + item_selkey->Length() + 1;
				ncolors++;
				
				tmp << item_selkey->String() << " ";

				SStringArray *array_tmp = item->Split('\t');
				if(array_tmp)
				{
					if(array_tmp->CountItems() > 1)
					{
						s_rgb_color_setto(&(colors[ncolors].color), 0, 128, 0, 255);
						colors[ncolors].start_offset = colors[ncolors - 1].end_offset + array_tmp->ItemAt(0)->Length() + 2;
						colors[ncolors].end_offset = colors[ncolors].start_offset + array_tmp->ItemAt(1)->Length() + 1;
						ncolors++;
						
						tmp << array_tmp->ItemAt(0)->String() << " " << array_tmp->ItemAt(1)->String();
					}
					else
					{
						tmp << item->String();
					}

					delete array_tmp;
				}
				else
				{
					tmp << item->String();
				}
			}
			else
			{
				tmp << item->String();
			}

			if(horizontal)
				tmp << " ";
			else
				tmp << " \n";

			if(i != items->CountItems() - 1) tmp << " ";

			text->InsertItemRect(text->TextRegionByText(tmp.String(), startOffset, endOffset).Frame(), startOffset, endOffset);
		}

		float w1 = 0, h1 = 0, w2 = 0, h2 = 0;
		text->GetPreferredSizeByText(tmp.String(), &w1, &h1);
		origin_text->GetPreferredSize(&w2, &h2);

		float w = w1 + 2;
		float h = h1 + 26;

		w = MAX(w, 140 + w2 + 10);
		h = (text->Text() != NULL ? h : 22);

		BScreen screen(this);
		if(w > screen.Frame().Width() && horizontal)
		{
			if(colors) delete[] colors;
			SetItems(origin_buffer, items, selkeys, select_item_index, can_do_page_up, can_do_page_down, left_top, false);
			return;
		}
		else
		{
			text->SetText(tmp.String(), colors, ncolors);
			if(start >= 0 && end >= 0) text->Highlight(start, end);
			if(colors) delete[] colors;
		}
	}
	else
	{
		text->SetText("");
	}

	if(left_top)
	{
		float w1 = 0, h1 = 0, w2 = 0, h2 = 0;
		text->GetPreferredSize(&w1, &h1);
		origin_text->GetPreferredSize(&w2, &h2);

		float w = w1 + 2;
		float h = h1 + 26;

		w = MAX(w, 140 + w2 + 10);
		h = (text->Text() != NULL ? h : 22);

		BScreen screen(this);

		if(left_top->y + h > screen.Frame().OffsetToCopy(B_ORIGIN).bottom)
			left_top->y = screen.Frame().OffsetToCopy(B_ORIGIN).bottom - h;
		if(left_top->y < 0)
			left_top->y = 0;

		if(left_top->x + w > screen.Frame().OffsetToCopy(B_ORIGIN).right)
			left_top->x = screen.Frame().OffsetToCopy(B_ORIGIN).right - w;
		if(left_top->x < 0)
			left_top->x = 0;
	}
}


void
SInputSelector::ResizeToFit()
{
	text->ResizeToPreferred();
	origin_text->ResizeToPreferred();

	float w = text->Bounds().Width() + 2;
	float h = text->Bounds().Height() + 26;

	w = MAX(w, 140 + origin_text->Bounds().Width() + 10);
	h = (text->Text() != NULL ? h : 22);

	if(Frame().Width() != w || Frame().Height() != h) ResizeTo(w, h);
}


void
SInputSelector::SetFontSize(float fsize)
{
	text->SetFontSize(fsize);
	text->RefreshView();
	ResizeToFit();
}


bool
SInputSelector::SupportNotActivatedTooltips() const
{
	return true;
}
