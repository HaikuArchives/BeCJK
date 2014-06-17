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
// Button supported Tooltip and Bitmap

#include <string.h>
#include <sys/utsname.h>

#include "Button.h"
#include "Strings.h"
#include "Tooltips.h"
#include "XPM.h"

#include <BeBuild.h>

#include <InterfaceDefs.h>

#include <Bitmap.h>


void
SButton::Init(const BBitmap *bitmap, const char *label, const char *tooltips)
{
	SButton::tooltips = NULL;
	SButton::bitmap = NULL;
	SButton::label = NULL;
	waitting = false;
	waitting_is_keyboard = false;
	focused = false;
	is_default = false;
	is_auto_default = false;
	old_time = 0;
	edge_is_black = true;
	behavior = S_ONE_STATE_BUTTON;
	SetValue(0);

	struct utsname the_os_name;
	uname(&the_os_name);

	if(strcasecmp(the_os_name.release, "5.1") == 0)
		button_style = DANO_STYLE;
	else
		button_style = R5_STYLE;

	if(label) SButton::label = Sstrdup(label);
	if(bitmap) SButton::bitmap = new BBitmap(bitmap);

	SetTooltips(tooltips);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const BBitmap *bitmap,
				 BMessage *message,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(bitmap, NULL, NULL);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char **xpm_data,
				 BMessage *message,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *bm = image.ConvertToBitmap();
	Init(bm, NULL, NULL);
	if(bm) delete bm;
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char *label,
				 BMessage *message,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(NULL, label, NULL);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const BBitmap *bitmap,
				 const char *label,
				 BMessage *message,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(bitmap, label, NULL);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char **xpm_data,
				 const char *label,
				 BMessage *message,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *bm = image.ConvertToBitmap();
	Init(bm, label, NULL);
	if(bm) delete bm;
}


SButton::SButton(BRect frame,
				 const char *name,
				 const BBitmap *bitmap,
				 BMessage *message,
				 const char *tooltips,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(bitmap, NULL, tooltips);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char **xpm_data,
				 BMessage *message,
				 const char *tooltips,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *bm = image.ConvertToBitmap();
	Init(bm, NULL, tooltips);
	if(bm) delete bm;
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char *label,
				 BMessage *message,
				 const char *tooltips,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(NULL, label, tooltips);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const BBitmap *bitmap,
				 const char *label,
				 BMessage *message,
				 const char *tooltips,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	Init(bitmap, label, tooltips);
}


SButton::SButton(BRect frame,
				 const char *name,
				 const char **xpm_data,
				 const char *label,
				 BMessage *message,
				 const char *tooltips,
				 uint32 resizeMask,
				 uint32 flags)
	: BButton(frame, name, label, message, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *bm = image.ConvertToBitmap();
	Init(bm, label, tooltips);
	if(bm) delete bm;
}


SButton::~SButton()
{
	if(tooltips) delete[] tooltips;
	if(label) delete[] label;
	if(bitmap) delete bitmap;
}


void
SButton::SetTooltips(const char *tooltips)
{
	BWindow *window = Window();

	if(SButton::tooltips) delete[] SButton::tooltips;

	if(tooltips)
		SButton::tooltips = Sstrdup(tooltips);
	else
		SButton::tooltips = NULL;

	if(window && tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);

		msg.AddBool("state", false);
	
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}
}


void
SButton::TooltipsLocation(BMessage *msg)
{
	msg->AddPoint("where", ConvertToScreen(Bounds().LeftBottom() + BPoint(1, 1)));
	msg->AddRect("frame", ConvertToScreen(Bounds()));
	msg->AddInt32("style", S_TOOLTIPS_FOLLOW_MOUSE_X);
}


void
SButton::MouseDown(BPoint where)
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

	if(!waitting_is_keyboard)
	{
		waitting = true;
		DrawDefault(Bounds(), true, window->IsActive());
		SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);
		window->UpdateIfNeeded();
	}
}


void
SButton::MouseUp(BPoint where)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();

	if(!waitting) return;

	if(!waitting_is_keyboard)
	{
		waitting = false;

		if(Bounds().Contains(where)) Invoke();
		else
		{
			DrawDefault(Bounds(), waitting, window->IsActive());
			window->UpdateIfNeeded();
		}
	}
}


void
SButton::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();
	uint32 btns;
	GetMouse(&pt, &btns, false);

	if(Bounds().Contains(pt) && code == B_ENTERED_VIEW)
	{
		if(!waitting && tooltips)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);
			msg.AddBool("state", true);
			msg.AddString("tooltips", tooltips);
			TooltipsLocation(&msg);
			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}
		else if(waitting && !waitting_is_keyboard)
		{
			DrawDefault(Bounds(), true, true);
			window->UpdateIfNeeded();
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
			
		if(waitting && !waitting_is_keyboard)
		{
			DrawDefault(Bounds(), false, true);
			window->UpdateIfNeeded();
		}
	}
}


void
SButton::DrawDefault(BRect bounds, bool pushed, bool window_activated)
{
	switch(button_style)
	{
		case R5_STYLE:
			draw_r5(bounds, pushed, window_activated);
			break;
		case DANO_STYLE:
			draw_dano(bounds, pushed, window_activated);
			break;
	}
}


void
SButton::Pulse()
{
	bool result = false;

	switch(button_style)
	{
		case R5_STYLE:
			result = flash_r5();
			break;
		case DANO_STYLE:
			result = flash_dano();
			break;
	}

	if(!result)
	{
		edge_is_black = true;
		old_time = 0;
		SetFlags(Flags() & ~B_PULSE_NEEDED);
	}
}


void
SButton::Draw(BRect updateRect)
{
	DrawDefault(Bounds(), waitting, Window()->IsActive());
}


void
SButton::SetBitmap(const BBitmap *bitmap)
{
	if(SButton::bitmap) delete SButton::bitmap;

	if(bitmap) SButton::bitmap = new BBitmap(bitmap);
	else SButton::bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SButton::SetLabel(const char *text)
{
	if(label) delete[] label;

	if(text) label = Sstrdup(text);
	else label = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SButton::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float content_width = 0, content_height = 0;
	float w, h;

	GetContentPreferredSize(&content_width, &content_height);

	w = content_width + 30;
	h = content_height + 10;
	
	if(is_default) h += 1;

	if(width) *width = w;
	if(height) *height = h;
}


void
SButton::ResizeToPreferred()
{
	float width, height;
	GetPreferredSize(&width, &height);

	ResizeBy(width - Bounds().Width(), height - Bounds().Height());
}


void
SButton::WindowActivated(bool state)
{
	if(focused)
	{
		edge_is_black = true;
		DrawDefault(Bounds(), waitting, state);
	}
}


void
SButton::KeyDown(const char *bytes, int32 numBytes)
{
	if(numBytes == 1)
	{
		if(bytes[0] == B_ENTER || bytes[0] == B_RETURN || bytes[0] == B_SPACE)
		{
			BWindow *window = Window();

			BMessage *msg = window->CurrentMessage();

			if(msg)
			{
				if(msg->what == B_KEY_DOWN)
				{
					int32 key_repeat = 0;
					if(msg->FindInt32("be:key_repeat", &key_repeat) == B_OK)
					{
						if(key_repeat > 0) return;
					}
				}
			}

			waitting = true;
			waitting_is_keyboard = true;

			Draw(Bounds());
			window->UpdateIfNeeded();

			return;
		}
	}

	BView::KeyDown(bytes, numBytes);
}


void
SButton::KeyUp(const char *bytes, int32 numBytes)
{
	if(numBytes == 1)
	{
		if(bytes[0] == B_ENTER || bytes[0] == B_RETURN || bytes[0] == B_SPACE)
		{
			BWindow *window = Window();

			BMessage *msg = window->CurrentMessage();

			if(msg)
			{
				if(msg->what == B_KEY_UP)
				{
					int32 key_repeat = 0;
					if(msg->FindInt32("be:key_repeat", &key_repeat) == B_OK)
					{
						if(key_repeat > 0) return;
					}
				}
			}

			waitting = false;
			waitting_is_keyboard = false;

			Invoke();

			return;
		}
	}

	BView::KeyUp(bytes, numBytes);
}


void
SButton::MakeFocus(bool focusState)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();

	if(focused != focusState)
	{
		focused = focusState;
		DrawDefault(Bounds(), waitting, window->IsActive());
		window->UpdateIfNeeded();
	}

	BView::MakeFocus(focusState);

	if(focused)
	{
		if(!(Flags() & B_PULSE_NEEDED))
		{
			edge_is_black = false;
			old_time = system_time();
			SetFlags(Flags() | B_PULSE_NEEDED);
		}
	}
	else
	{
		if(Flags() & B_PULSE_NEEDED)
		{
			edge_is_black = true;
			old_time = 0;
			SetFlags(Flags() & ~B_PULSE_NEEDED);
		}
	}

	DrawDefault(Bounds(), waitting, window->IsActive());
	window->UpdateIfNeeded();

	if(is_auto_default)
	{
		if(focused)
		{
			if(window->DefaultButton() != this)
			{
				MakeDefault(true);
				window->SetDefaultButton(this);
			}
		}
		else
		{
			if(window->DefaultButton() == this)
			{
				MakeDefault(false);
				window->SetDefaultButton(NULL);
			}
		}
	}
}


void
SButton::MakeDefault(bool flag)
{
	BWindow *window = Window();

	if(is_default != flag)
	{
		is_default = flag;

		if(button_style == DANO_STYLE)
		{
			if(is_default)
			{
				MoveBy(-5, 0);
				ResizeBy(8, 0);
			}
			else
			{
				MoveBy(5, 0);
				ResizeBy(-8, 0);
			}
		}
		else
		{
			if(is_default)
			{
				MoveBy(-4, -4);
				ResizeBy(8, 8);
			}
			else
			{
				MoveBy(4, 4);
				ResizeBy(-8, -8);
			}
		}
	}

	if(window)
	{
		DrawDefault(Bounds(), waitting, window->IsActive());
		window->UpdateIfNeeded();
	}
}


bool
SButton::IsDefault() const
{
	return is_default;
}


void
SButton::MakeAutoDefault(bool flag)
{
	is_auto_default = flag;
}


bool
SButton::IsAutoDefault() const
{
	return is_auto_default;
}


void
SButton::GetContentPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float string_width = 0, string_height = 0;

	if(label)
	{
		string_width = StringWidth(label);

		font_height fheight;
		GetFontHeight(&fheight);
		string_height = fheight.ascent + fheight.descent + fheight.leading;
	}

	if(!bitmap)
	{
		if(width) *width = string_width;
		if(height) *height = string_height;
	}
	else
	{
		if(label) string_width += 3;
		if(width) *width = string_width + bitmap->Bounds().Width();
		if(height) *height = MAX(string_height, bitmap->Bounds().Height());
	}
}


void
SButton::DrawContent(BRect content_rect, bool pushed, bool window_activated)
{
	SetPenSize(1);

	if(label)
	{
		if(IsEnabled())
		{
			if((!pushed && Behavior() != S_TWO_STATE_BUTTON) ||
			   (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON))
			{
				SetDrawingMode(B_OP_COPY);
				if(button_style == DANO_STYLE)
#ifdef B_BEOS_VERSION_DANO
				{
					SetHighColor(ui_color(B_CONTROL_TEXT_COLOR));
					SetLowColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
				}
#else
				{
					SetHighColor(0, 0, 0, 255);
					SetLowColor(245, 245, 245, 255);
				}
#endif
				else
				{
					SetHighColor(0, 0, 0, 255);
					SetLowColor(232, 232, 232, 255);
				}
			}
			else
			{
				if(button_style == DANO_STYLE)
				{
					SetDrawingMode(B_OP_ALPHA);
					SetHighColor(0, 0, 0, 150);

#ifdef B_BEOS_VERSION_DANO
					rgb_color cl = ui_color(B_CONTROL_TEXT_COLOR);
					cl.alpha = 150;
					SetHighColor(cl);
					SetLowColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
#else
					SetHighColor(0, 0, 0, 255);
					SetLowColor(245, 245, 245, 255);
#endif
				}
				else
				{
					SetDrawingMode(B_OP_COPY);
					SetHighColor(255, 255, 255, 255);
					SetLowColor(0, 0, 0, 255);
				}
			}
		}
		else
		{
			SetDrawingMode(B_OP_ALPHA);
			if(button_style == DANO_STYLE)
			{
#ifdef B_BEOS_VERSION_DANO
				rgb_color cl1 = ui_color(B_CONTROL_TEXT_COLOR);
				rgb_color cl2 = ui_color(B_CONTROL_BACKGROUND_COLOR);
				cl1.alpha = 70;
				cl2.alpha = 70;
				SetHighColor(cl1);
				SetLowColor(cl2);
#else
				SetHighColor(0, 0, 0, 70);
				SetLowColor(245, 245, 245, 70);
#endif
			}
			else
			{
				SetHighColor(0, 0, 0, 70);
				SetLowColor(232, 232, 232, 70);
			}
		}

		float width = StringWidth(label);
		float x, y;

		if(bitmap)
			x = content_rect.left +
				(content_rect.Width() - width - bitmap->Bounds().Width() - 5) / 2 +
				bitmap->Bounds().Width() + 3;
		else
			x = content_rect.left + (content_rect.Width() - width) / 2;

		font_height height;
		GetFontHeight(&height);
		y = content_rect.bottom -
			(content_rect.Height() - (height.ascent + height.descent + height.leading)) / 2 -
			height.descent;
		

		DrawString(label, BPoint(x, y));
		
		if(IsEnabled() &&
		   ((!pushed && Behavior() != S_TWO_STATE_BUTTON) || (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON)) &&
		   button_style == R5_STYLE &&
		   focused &&
		   Window()->IsActive() &&
		   !is_default)
		{
			float x1, y1, x2, y2;
			x1 = x - 1;
			y1 = y + height.descent + 1;
			x2 = x + width + 1;
			y2 = y + height.descent + 1;
			SetHighColor(keyboard_navigation_color());
			StrokeLine(BPoint(x1, y1), BPoint(x2, y2));
		}

		if(bitmap)
		{
			if(IsEnabled()) SetDrawingMode(B_OP_OVER);
			else SetDrawingMode(B_OP_BLEND);

			x -= bitmap->Bounds().Width() + 3;
			y = content_rect.top +
				(content_rect.Height() - bitmap->Bounds().Height()) / 2;

			DrawBitmap(bitmap, BPoint(x, y));
		}
	}
	else if(bitmap)
	{
		float x, y;

		x = content_rect.left +	(content_rect.Width() - bitmap->Bounds().Width()) / 2;
		y = content_rect.top + (content_rect.Height() - bitmap->Bounds().Height()) / 2;

		if(IsEnabled()) SetDrawingMode(B_OP_OVER);
		else SetDrawingMode(B_OP_BLEND);

		DrawBitmap(bitmap, BPoint(x, y));
	}
}


void
SButton::SetBehavior(uint32 behavior)
{
	BWindow *window = Window();
	
	if(SButton::behavior != behavior)
	{
		SButton::behavior = behavior;
		if(window)
		{
			DrawDefault(Bounds(), waitting, window->IsActive());
			window->UpdateIfNeeded();
		}
	}
}


uint32
SButton::Behavior() const
{
	return behavior;
}


void
SButton::SetValue(int32 value)
{
	BWindow *window = Window();
	
	if(value != Value())
	{
		BControl::SetValue(value);
		if(window && Behavior() == S_TWO_STATE_BUTTON)
		{
			DrawDefault(Bounds(), waitting, window->IsActive());
			window->UpdateIfNeeded();
		}
	}
}


status_t
SButton::Invoke(BMessage *msg)
{
	BWindow *window = Window();
	
	if(Behavior() == S_TWO_STATE_BUTTON)
	{
		if(Value() == 0) SetValue(1);
		else SetValue(0);
	}
	else
	{
		DrawDefault(Bounds(), waitting, window->IsActive());
		window->UpdateIfNeeded();
	}

	return BControl::Invoke(msg);
}


void
SButton::SetStyle(SButtonStyle style)
{
	BWindow *window = Window();
	
	if(button_style != style)
	{
		button_style = style;

		if(window)
		{
			DrawDefault(Bounds(), waitting, window->IsActive());
			window->UpdateIfNeeded();
		}
	}
}


SButtonStyle
SButton::Style() const
{
	return button_style;
}


void
SButton::draw_dano(BRect bounds, bool pushed, bool window_activated)
{
	BRect frame = bounds;

	SetPenSize(1);
	SetDrawingMode(B_OP_COPY);
	SetHighColor(ViewColor());
	FillRect(frame);

	if(is_default)
	{
		BRect rect1, rect2;

		rect1 = frame;
		rect1.right = rect1.left + 18;
		
		rect2 = frame;
		rect2.left = rect2.right - 15;

#ifdef B_BEOS_VERSION_DANO
		SetHighColor(ui_color(B_CONTROL_HIGHLIGHT_COLOR));
#else
		SetHighColor(115, 120, 184, 255);
#endif
		FillRoundRect(rect1, 3, 3);
		FillRoundRect(rect2, 3, 3);

#ifdef B_BEOS_VERSION_DANO
		SetHighColor(ui_color(B_CONTROL_BORDER_COLOR));
#else
		SetHighColor(0, 0, 0, 255);
#endif
		StrokeRoundRect(rect1, 3, 3);
		StrokeRoundRect(rect2, 3, 3);

		frame.left += 5;
		frame.right -= 3;
	}

	if(IsEnabled())
	{
		if((!pushed && Behavior() != S_TWO_STATE_BUTTON) ||
		   (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON))
		{
			// draw shadow
			SetDrawingMode(B_OP_ALPHA);
#ifdef B_BEOS_VERSION_DANO
			rgb_color shadow_cl = ui_color(B_SHADOW_COLOR);
#else
			rgb_color shadow_cl = {0, 0, 0, 255};
#endif
			frame.left += 2;
			frame.top += 4;
			shadow_cl.alpha = 50;
			SetHighColor(shadow_cl);
			FillRoundRect(frame, 3, 3);
			frame.right -= 1;
			frame.bottom -= 1;
			shadow_cl.alpha = 70;
			SetHighColor(shadow_cl);
			FillRoundRect(frame, 3, 3);
			frame.right += 1;
			frame.bottom += 1;

			frame.OffsetBy(-2, -2);

			// draw background
			SetDrawingMode(B_OP_COPY);
#ifdef B_BEOS_VERSION_DANO
			SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
#else
			SetHighColor(245, 245, 245, 255);
#endif
			FillRoundRect(frame, 3, 3);

			// draw border
			if(!edge_is_black && window_activated)
			{
				SetHighColor(keyboard_navigation_color());
			}
			else
			{
#ifdef B_BEOS_VERSION_DANO
				if(!focused) SetHighColor(ui_color(B_CONTROL_BORDER_COLOR));
				else SetHighColor(ui_color(B_NAVIGATION_PULSE_COLOR));
#else
				SetHighColor(0, 0, 0, 255);
#endif
			}

			StrokeRoundRect(frame, 3, 3);
		}
		else
		{
			SetDrawingMode(B_OP_COPY);

			// draw background
			frame.top += 2;
			frame.right -= 2;
			frame.bottom -= 2;

#ifdef B_BEOS_VERSION_DANO
			SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
#else
			SetHighColor(245, 245, 245, 255);
#endif
			FillRoundRect(frame, 3, 3);

			SetDrawingMode(B_OP_ALPHA);
			SetHighColor(0, 0, 0, 5);
			FillRoundRect(frame, 3, 3);

			// draw shadow
#ifdef B_BEOS_VERSION_DANO
			rgb_color shadow_cl = ui_color(B_SHADOW_COLOR);
#else
			rgb_color shadow_cl = {0, 0, 0, 255};
#endif
			BRect rect = frame;
			rect.left += 1;
			rect.top += 1;
			shadow_cl.alpha = 70;
			SetHighColor(shadow_cl);
			StrokeRoundRect(rect, 2, 2);

			rect.left += 1;
			rect.top += 1;
			shadow_cl.alpha = 50;
			SetHighColor(shadow_cl);
			StrokeRoundRect(rect, 2, 2);

			StrokeLine(rect.LeftTop(), rect.LeftTop());

			// draw border
			SetDrawingMode(B_OP_COPY);

			if(!edge_is_black && window_activated)
			{
				SetHighColor(keyboard_navigation_color());
			}
			else
			{
#ifdef B_BEOS_VERSION_DANO
				if(!focused) SetHighColor(ui_color(B_CONTROL_BORDER_COLOR));
				else SetHighColor(ui_color(B_NAVIGATION_PULSE_COLOR));
#else
				SetHighColor(0, 0, 0, 255);
#endif
			}

			StrokeRoundRect(frame, 3, 3);
		}
	}
	else
	{
		frame.top += 2;
		frame.right -= 2;
		frame.bottom -= 2;
	
		SetDrawingMode(B_OP_COPY);

#ifdef B_BEOS_VERSION_DANO
		SetHighColor(ui_color(B_CONTROL_BACKGROUND_COLOR));
#else
		SetHighColor(245, 245, 245, 255);
#endif
		FillRoundRect(frame, 3, 3);

		SetDrawingMode(B_OP_ALPHA);
		rgb_color view_cl = ViewColor();
		view_cl.alpha = 200;
		SetHighColor(view_cl);
		FillRoundRect(frame, 3, 3);

#ifdef B_BEOS_VERSION_DANO
		rgb_color cl = ui_color(B_CONTROL_BORDER_COLOR);
		cl.alpha = 70;
		SetHighColor(cl);
#else
		SetHighColor(0, 0, 0, 70);
#endif
		StrokeRoundRect(frame, 3, 3);
	}

	frame.InsetBy(2, 2);

	DrawContent(frame, pushed, window_activated);
}


bool
SButton::flash_dano()
{
	if(!focused) return true;

	if(!Window()->IsActive()) return true;

	if(system_time() - old_time > 50000)
	{
		BRect frame = Bounds();

		if(is_default)
		{
			frame.left += 5;
			frame.right -= 3;
		}

		frame.top += 2;
		frame.right -= 2;
		frame.bottom -= 2;
		
		edge_is_black = !edge_is_black;

		SetDrawingMode(B_OP_COPY);
		SetPenSize(1);

		if(!edge_is_black)
		{
			SetHighColor(keyboard_navigation_color());
		}
		else
		{
#ifdef B_BEOS_VERSION_DANO
			if(!focused) SetHighColor(ui_color(B_CONTROL_BORDER_COLOR));
			else SetHighColor(ui_color(B_NAVIGATION_PULSE_COLOR));
#else
			SetHighColor(0, 0, 0, 255);
#endif
		}

		StrokeRoundRect(frame, 3, 3);
		
		Window()->UpdateIfNeeded();

		old_time = system_time();
	}
	
	return true;
}


void
SButton::draw_r5(BRect bounds, bool pushed, bool window_activated)
{
	BRect frame = bounds;

	SetPenSize(1);
	if(IsEnabled()) SetDrawingMode(B_OP_COPY);
	else SetDrawingMode(B_OP_BLEND);

	SetHighColor(ViewColor());
	FillRect(frame);

	if(is_default)
	{
		SetHighColor(232, 232, 232, 255);
		FillRoundRect(frame, 2, 2);

		SetHighColor(96, 96, 96, 255);
		StrokeRoundRect(frame, 2, 2);

		frame.InsetBy(1, 1);
		SetHighColor(184, 184, 184, 255);
		StrokeRect(frame);

		frame.InsetBy(3, 3);
	}
	else
	{
		SetHighColor(232, 232, 232, 255);
		FillRoundRect(frame, 4, 4);
	}

	if((((!pushed && Behavior() != S_TWO_STATE_BUTTON) ||
	     (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON)) && IsEnabled()) || !IsEnabled())
	{
		BRect rect = frame;

		frame.InsetBy(1, 1);

		frame.left += 1;
		frame.top += 1;

		SetHighColor(152, 152, 152, 255);
		StrokeRect(frame);

		frame.InsetBy(1, 1);
		SetHighColor(216, 216, 216, 255);
		StrokeRect(frame);

		SetDrawingMode(B_OP_COPY);

		SetHighColor(255, 255, 255, 255);
		StrokeLine(frame.LeftTop(), BPoint(frame.right - 1, frame.top));
		StrokeLine(frame.LeftTop(), BPoint(frame.left, frame.bottom - 1));

		frame.InsetBy(-1, -1);
		StrokeLine(frame.LeftTop(), BPoint(frame.right - 1, frame.top));
		StrokeLine(frame.LeftTop(), BPoint(frame.left, frame.bottom - 1));

		frame.InsetBy(2, 2);

		if(IsEnabled()) SetDrawingMode(B_OP_COPY);
		else SetDrawingMode(B_OP_BLEND);

		SetHighColor(96, 96, 96, 255);
		StrokeRoundRect(rect, 2, 2);
	}
	else
	{
		SetDrawingMode(B_OP_COPY);

		BRect rect = frame;

		SetHighColor(23, 23, 23, 255);
		FillRoundRect(frame, 2, 2);
			
		frame.InsetBy(1, 1);
		SetHighColor(96, 96, 96, 255);
		StrokeLine(frame.RightBottom(), BPoint(frame.right, frame.top + 1));
		StrokeLine(frame.RightBottom(), BPoint(frame.left - 1, frame.bottom));

		frame.left += 1;
		frame.top += 1;

		frame.InsetBy(2, 2);

		SetHighColor(96, 96, 96, 255);
		StrokeRoundRect(rect, 2, 2);
	}

	frame.InsetBy(2, 2);

	DrawContent(frame, pushed, window_activated);
}


bool
SButton::flash_r5()
{
	if(Window()->IsActive())
	{
		Draw(Bounds());
		Window()->UpdateIfNeeded();
	}

	return false;
}


const char*
SButton::Label() const
{
	return label;
}


const BBitmap*
SButton::Bitmap() const
{
	return bitmap;
}


void
SButton::AttachedToWindow()
{
	if(Parent())
		SetViewColor(Parent()->ViewColor());
	BControl::AttachedToWindow();
}

