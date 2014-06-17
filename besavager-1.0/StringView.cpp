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
// String View

#include <Message.h>
#include <Messenger.h>

#include "Autolock.h"
#include "StringArray.h"
#include "StringView.h"
#include "Tooltips.h"


SStringView::SStringView(BRect bounds,
						 const char *name, 
						 const char *text,
						 const s_string_view_color *colors,
						 int32 ncolors,
						 const char *tooltips,
						 uint32 resizeFlags,
						 uint32 flags)
	: BView(bounds, name, resizeFlags, flags),
	  align(B_ALIGN_LEFT),
	  view_color_custom(false),
	  enabled(true), highlight_start(-1), highlight_end(-1),
	  bitmap(NULL), bitmap_view(NULL),
	  color_list(NULL)
{
	SStringView::tooltips = NULL;
	if(tooltips) SStringView::tooltips = Sstrdup(tooltips);

	SStringView::text = "";
	if(text) SStringView::text = text;

	SetColorList(colors, ncolors);
}


void
SStringView::EmptyColorList()
{
	if(color_list)
	{
		s_string_view_color *color = NULL;
		for(int32 i = 0; (color = (s_string_view_color*)color_list->ItemAt(i)) != NULL; i++) delete color;
		delete color_list;
		color_list = NULL;
	}
}


void
SStringView::SetColorList(const s_string_view_color *colors, int32 ncolors)
{
	EmptyColorList();

	if(colors != NULL && ncolors > 0)
	{
		if(!color_list) color_list = new BList();

		if(color_list)
		{
			for(int32 i = 0; i < ncolors; i++)
			{
				s_string_view_color *color = new s_string_view_color;
				if(!color) // alloc failed!!
				{
					delete color_list;
					color_list = NULL;
					break;
				}

				s_rgb_color_setto(&(color->color), colors[i].color);
				color->start_offset = colors[i].start_offset;
				color->end_offset = colors[i].end_offset;
			
				if(!color_list->AddItem(color)) delete color;
			}
		
			if(color_list->IsEmpty())
			{
				delete color_list;
				color_list = NULL;
			}
		}
	}
}


SStringView::~SStringView()
{
	if(tooltips) delete[] tooltips;
	if(bitmap) delete bitmap;
	EmptyColorList();
}


void
SStringView::SetTooltips(const char *tooltips)
{
	BWindow *window = Window();

	if(SStringView::tooltips) delete[] SStringView::tooltips;

	if(tooltips)
		SStringView::tooltips = Sstrdup(tooltips);
	else
		SStringView::tooltips = NULL;

	if(window && tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);

		msg.AddBool("state", false);
	
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}
}


void
SStringView::SetEnabled(bool on)
{
	if(enabled != on)
	{
		enabled = on;

		RefreshView();
	}
}


bool
SStringView::IsEnabled() const
{
	return enabled;
}


void
SStringView::SetText(const char *text, const s_string_view_color *colors, int32 ncolors)
{
	bool update = false;

	if(!text) text = "";

	if(SStringView::text != text)
	{
		update = true;
		SStringView::text = text;
		highlight_start = highlight_end = -1;
	}

	if(color_list && colors && ncolors > 0)
	{
		if(color_list->CountItems() == ncolors)
		{
			for(int32 i = 0; i < ncolors; i++)
			{
				s_string_view_color *color1 = (s_string_view_color*)color_list->ItemAt(i);
				const s_string_view_color *color2 = &(colors[i]);

				if(!s_rgb_color_is_equal(color1->color, color2->color) ||
				   color1->start_offset != color2->start_offset ||
				   color1->end_offset != color2->end_offset)
				{
					update = true;
					SetColorList(colors, ncolors);
					break;
				}
			}
		}
		else
		{
			update = true;
			SetColorList(colors, ncolors);
		}
	}
	else if(!color_list && colors && ncolors > 0)
	{
		update = true;
		SetColorList(colors, ncolors);
	}

	if(update) RefreshView();
}


const char*
SStringView::Text() const
{
	return(text.Length() > 0 ? text.String() : NULL);
}


void
SStringView::SetAlignment(alignment flag)
{
	if(align != flag)
	{
		align = flag;

		RefreshView();
	}
}


alignment
SStringView::Alignment() const
{
	return align;
}


void
SStringView::SetViewColor(rgb_color c)
{
	view_color_custom = true;
	BView::SetViewColor(c);
}


void
SStringView::AttachedToWindow()
{
	if(Parent() && !view_color_custom)
		BView::SetViewColor(Parent()->ViewColor());

	GenerateBuffer();

	BView::AttachedToWindow();
}


void
SStringView::Draw(BRect bounds)
{
	if(bitmap)
	{
		SAutolock<BBitmap> autolock(bitmap);
		PushState();
		SetDrawingMode(B_OP_COPY);
		DrawBitmapAsync(bitmap, bounds, bounds);
		PopState();
	}
}


void
SStringView::MouseDown(BPoint pt)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();

	if(!(window->Flags() & B_AVOID_FOCUS)) window->Activate();

	if(tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}
}


void
SStringView::MouseUp(BPoint pt)
{
	if(!IsEnabled()) return;

	// TODO
}


void
SStringView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();
	uint32 btns;
	GetMouse(&pt, &btns, false);

	if(Bounds().Contains(pt) && code == B_ENTERED_VIEW)
	{
		if(tooltips)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);
			msg.AddBool("state", true);
			msg.AddString("tooltips", tooltips);
			msg.AddRect("frame", ConvertToScreen(Bounds()));
			msg.AddInt32("style", S_TOOLTIPS_FOLLOW_FRAME_CENTER);
			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}
	}
	else if(code == B_EXITED_VIEW)
	{
		if(!Bounds().Contains(pt) && tooltips)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);
			msg.AddBool("state", false);
			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}
	}
}


void
SStringView::ResizeToPreferred()
{
	float width, height;
	GetPreferredSize(&width, &height);

	ResizeBy(width - Bounds().Width(), height - Bounds().Height());
}


void
SStringView::GetPreferredSize(float *width, float *height)
{
	GetPreferredSizeByText(text.String(), width, height);
}


void
SStringView::GetPreferredSizeByText(const char *string, float *width, float *height)
{
	if(!string || (!width && !height)) return;

	if(width) *width = 0;
	if(height) *height = 0;

	SString sstr(string);

	if(sstr.Length() < 1) return;

	float w = 0, h = 0;

	SStringArray *array = sstr.Split('\n');
	int32 lines = 0;

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;
			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
				w = MAX(w, StringWidth(data->String()));
		}

		lines = array->CountItems();

		delete array;
	}

	if(lines > 0)
	{
		font_height fheight;
		GetFontHeight(&fheight);
		float true_height = fheight.ascent + fheight.descent + fheight.leading;
		h = true_height * (float)lines + (float)(lines - 1) * (true_height / 8.);
	}

	if(width) *width = w + 2;
	if(height) *height = h + 2;
}


void
SStringView::FrameMoved(BPoint new_position)
{
	if(Window()) Invalidate();
}


void
SStringView::FrameResized(float new_width, float new_height)
{
	RefreshView();
}


void
SStringView::RefreshView(BRect rect)
{
	GenerateBuffer();
	if(Window()) Invalidate(rect);
}


BRegion
SStringView::TextRegion(int32 startOffset, int32 endOffset)
{
	return TextRegionByText(text.String(), startOffset, endOffset);
}


BRegion
SStringView::TextRegionByText(const char *string, int32 startOffset, int32 endOffset)
{
	BRegion region;

	SString sstr(string);

	if(sstr.Length() < 1) return region;

	if(startOffset < 0) startOffset = 0;
	if(endOffset < startOffset || endOffset > sstr.Length()) endOffset = sstr.Length() - 1;

	SStringArray *array = sstr.Split('\n');

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;

			font_height fheight;
			GetFontHeight(&fheight);
			float true_height = fheight.ascent + fheight.descent + fheight.leading;

			BRect frame = Bounds().InsetByCopy(1, 1);

			int32 offset = 0;

			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
			{
				if(offset + data->Length() < startOffset)
				{
					offset += data->Length() + 1;
					continue;
				}

				if(offset > endOffset) break;

				if(data->Length() < 1)
				{
					offset += data->Length() + 1;
					continue;
				}

				BPoint line_left_top;
				switch(align)
				{
					case B_ALIGN_RIGHT:
						line_left_top.x = frame.right - StringWidth(data->String());
						break;

					case B_ALIGN_CENTER:
						line_left_top.x = frame.left + (frame.Width() - StringWidth(data->String())) / 2.;
						break;

					default: // B_ALIGN_LEFT
						line_left_top.x = frame.left;
				}

				line_left_top.y = frame.top + true_height * (float)(i) + (float)i * (true_height / 8.);

				float start_x, end_x;

				if(startOffset <= offset)
					start_x = line_left_top.x;
				else
					start_x = line_left_top.x + StringWidth(data->String(), startOffset - offset);

				if(endOffset >= offset + data->Length())
					end_x = line_left_top.x + StringWidth(data->String());
				else
					end_x = line_left_top.x + StringWidth(data->String()) -
							StringWidth(&(data->String()[endOffset - offset + 1]));
				
				BRect rect;
				rect.left = start_x;
				rect.right = end_x;
				rect.top = line_left_top.y - (i > 0 ? true_height / 16. : 0);
				rect.bottom = line_left_top.y + true_height + (i == array->CountItems() - 1 ? 0 : true_height / 16.);

				region.Include(rect);

				offset += data->Length() + 1;
			}
		}

		delete array;
	}

	return region;
}


void
SStringView::DrawText(BView *view, int32 startOffset, int32 endOffset)
{
	if(!view) return;
	if(text.Length() < 1) return;
	
	if(startOffset < 0) startOffset = 0;
	if(endOffset < startOffset || endOffset > text.Length()) endOffset = text.Length() - 1;

	__DrawText(view, startOffset, endOffset, HighColor());
	
	if(color_list)
	{
		s_string_view_color *color = NULL;
		for(int32 i = 0; (color = (s_string_view_color*)color_list->ItemAt(i)) != NULL; i++)
		{
			if(startOffset > color->end_offset || endOffset < color->start_offset) continue;

			int32 start = MAX(startOffset, color->start_offset);
			int32 end = MIN(endOffset, color->end_offset);
			
			__DrawText(view, start, end, color->color);
		}
	}
}


void
SStringView::__DrawText(BView *view, int32 startOffset, int32 endOffset, rgb_color color)
{
	if(!view) return;
	if(text.Length() < 1) return;
	
	if(startOffset < 0) startOffset = 0;
	if(endOffset < startOffset || endOffset > text.Length()) endOffset = text.Length() - 1;

	SStringArray *array = text.Split('\n');

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;

			font_height fheight;
			GetFontHeight(&fheight);
			float true_height = fheight.ascent + fheight.descent + fheight.leading;

			BRect frame = Bounds().InsetByCopy(1, 1);

			int32 offset = 0;

			view->PushState();

			view->SetViewColor(ViewColor());
			view->SetHighColor(color);
			view->SetLowColor(B_TRANSPARENT_32_BIT);

			BFont font;
			GetFont(&font);
			view->SetFont(&font);

			if(IsEnabled()) view->SetDrawingMode(B_OP_ALPHA);
			else view->SetDrawingMode(B_OP_BLEND);

			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
			{
				if(offset + data->Length() < startOffset)
				{
					offset += data->Length() + 1;
					continue;
				}

				if(offset > endOffset) break;

				if(data->Length() < 1)
				{
					offset += data->Length() + 1;
					continue;
				}

				float x, y;
				switch(align)
				{
					case B_ALIGN_RIGHT:
						x = frame.right - StringWidth(data->String());
						break;

					case B_ALIGN_CENTER:
						x = frame.left + (frame.Width() - StringWidth(data->String())) / 2.;
						break;

					default: // B_ALIGN_LEFT
						x = frame.left;
				}

				y = frame.top + true_height * (float)(i + 1) + (float)i * (true_height / 8.) - fheight.descent;

				BPoint loc(x, y);
				if(startOffset > offset) loc.x += StringWidth(data->String(), startOffset - offset);

				if(startOffset <= offset)
					view->DrawString(data->String(),
									 MIN(endOffset, offset + data->Length() - 1) - offset + 1, loc);
				else
					view->DrawString(&(data->String()[startOffset - offset]),
									 MIN(endOffset, offset + data->Length() - 1) - startOffset + 1, loc);

				offset += data->Length() + 1;
			}

			view->PopState();
		}

		delete array;
	}
}


void
SStringView::DrawOldHighlightToNormal(BView *view, int32 startOffset, int32 endOffset)
{
	if(!view) return;

	BRegion region = TextRegion(startOffset, endOffset);
	if(!region.Frame().IsValid()) return;

	view->PushState();
	view->SetDrawingMode(B_OP_COPY);
	view->SetHighColor(ViewColor());
	view->FillRegion(&region);
	view->PopState();

	DrawText(view, startOffset, endOffset);
}


void
SStringView::DrawHighlight(BView *view, int32 startOffset, int32 endOffset)
{
	if(!view) return;

	BRegion region = TextRegion(startOffset, endOffset);
	if(!region.Frame().IsValid()) return;

	view->PushState();
	view->SetHighColor(B_TRANSPARENT_32_BIT);
	view->SetLowColor(B_TRANSPARENT_32_BIT);
	view->SetDrawingMode(B_OP_INVERT);
	view->FillRegion(&region);
	view->PopState();
}


void
SStringView::Highlight(int32 startOffset, int32 endOffset)
{
	if(startOffset == highlight_start && endOffset == highlight_end) return;
	
	BRegion region1 = TextRegion(highlight_start, highlight_end);

	int32 old_highlight_start = highlight_start;
	int32 old_highlight_end = highlight_end;

	highlight_start = startOffset;
	highlight_end = endOffset;

	BRegion region2 = TextRegion(highlight_start, highlight_end);

	region1.Include(&region2);

	if(bitmap)
	{
		bitmap->Lock();
		DrawOldHighlightToNormal(bitmap_view, old_highlight_start, old_highlight_end);
		DrawHighlight(bitmap_view, highlight_start, highlight_end);
		bitmap_view->Sync();
		bitmap->Unlock();
	}

	if(Window()) Invalidate(region1.Frame());
}


BBitmap*
SStringView::BufferBitmap()
{
	return bitmap;
}


BView*
SStringView::BufferBitmapView()
{
	return bitmap_view;
}


void
SStringView::GenerateBuffer()
{
	if(bitmap)
	{
		delete bitmap;
		bitmap = NULL;
		bitmap_view = NULL;
	}

	bitmap = new BBitmap(Bounds(), B_BITMAP_ACCEPTS_VIEWS, B_RGB32);

	SAutolock<BBitmap> autolock(bitmap);

	bitmap_view = new BView(Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);
	bitmap->AddChild(bitmap_view);

	bitmap_view->PushState();
	bitmap_view->SetDrawingMode(B_OP_COPY);
	bitmap_view->SetHighColor(ViewColor());
	bitmap_view->FillRect(Bounds());
	bitmap_view->PopState();

	if(text.Length() < 1) return;

	DrawText(bitmap_view, 0, -1);
	if(!(highlight_start < 0 && highlight_end < 0)) DrawHighlight(bitmap_view, highlight_start, highlight_end);

	bitmap_view->Sync();
}

