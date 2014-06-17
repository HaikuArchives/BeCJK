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

#include "BitmapButton.h"
#include "Strings.h"
#include "Tooltips.h"
#include "XPM.h"

#include <BeBuild.h>

#include <InterfaceDefs.h>

#include <Bitmap.h>


void
SBitmapButton::Init(const BBitmap *off, const BBitmap *on, const char *tooltips)
{
	SBitmapButton::tooltips = NULL;
	enable_off_bitmap = NULL;
	enable_on_bitmap = NULL;
	disable_off_bitmap = NULL;
	disable_on_bitmap = NULL;
	waitting = false;
	waitting_is_keyboard = false;
	behavior = S_ONE_STATE_BUTTON;
	mouse_focusing = false;
	SetValue(0);

	if(off) enable_off_bitmap = new BBitmap(off);

	if(on)
		enable_on_bitmap = new BBitmap(on);
	else
		enable_on_bitmap = new BBitmap(off);

	SetTooltips(tooltips);
}


SBitmapButton::SBitmapButton(BRect frame,
							 const char *name,
							 const BBitmap *off,
							 const BBitmap *on,
							 BMessage *message,
							 uint32 resizeMask,
							 uint32 flags)
	: BControl(frame, name, NULL, message, resizeMask, flags)
{
	Init(off, on, NULL);
}


SBitmapButton::SBitmapButton(BRect frame,
							 const char *name,
							 const BBitmap *off,
							 const BBitmap *on,
							 const char* tooltips,
							 BMessage *message,
							 uint32 resizeMask,
							 uint32 flags)
	: BControl(frame, name, NULL, message, resizeMask, flags)
{
	Init(off, on, tooltips);
}


SBitmapButton::SBitmapButton(BRect frame,
							 const char *name,
							 const char** off_xpm_data,
							 const char** on_xpm_data,
							 BMessage *message,
							 uint32 resizeMask,
							 uint32 flags)
	: BControl(frame, name, NULL, message, resizeMask, flags)
{
	SXPM image;
	
	image.SetTo(off_xpm_data);
	BBitmap *off = image.ConvertToBitmap();

	BBitmap *on;

	if(on_xpm_data)
	{
		image.SetTo(on_xpm_data);
		on = image.ConvertToBitmap();
	}
	else
	{
		on = new BBitmap(off);
	}

	Init(off, on, NULL);

	if(off) delete off;
	if(on) delete on;
}


SBitmapButton::SBitmapButton(BRect frame,
							 const char *name,
							 const char** off_xpm_data,
							 const char** on_xpm_data,
							 const char* tooltips,
							 BMessage *message,
							 uint32 resizeMask,
							 uint32 flags)
	: BControl(frame, name, NULL, message, resizeMask, flags)
{
	SXPM image;
	
	image.SetTo(off_xpm_data);
	BBitmap *off = image.ConvertToBitmap();

	BBitmap *on;
	if(on_xpm_data)
	{
		image.SetTo(on_xpm_data);
		on = image.ConvertToBitmap();
	}
	else
	{
		on = new BBitmap(off);
	}

	Init(off, on, tooltips);

	if(off) delete off;
	if(on) delete on;
}


SBitmapButton::~SBitmapButton()
{
	if(tooltips) delete[] tooltips;
	if(enable_off_bitmap) delete enable_off_bitmap;
	if(enable_on_bitmap) delete enable_on_bitmap;
	if(disable_off_bitmap) delete disable_off_bitmap;
	if(disable_on_bitmap) delete disable_on_bitmap;
}


void
SBitmapButton::SetTooltips(const char *tooltips)
{
	BWindow *window = Window();

	if(SBitmapButton::tooltips) delete[] SBitmapButton::tooltips;

	if(tooltips)
		SBitmapButton::tooltips = Sstrdup(tooltips);
	else
		SBitmapButton::tooltips = NULL;

	if(window && tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);

		msg.AddBool("state", false);
	
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}
}


void
SBitmapButton::TooltipsLocation(BMessage *msg)
{
	msg->AddPoint("where", ConvertToScreen(Bounds().LeftBottom() + BPoint(1, 1)));
	msg->AddRect("frame", ConvertToScreen(Bounds()));
	msg->AddInt32("style", S_TOOLTIPS_FOLLOW_MOUSE_X);
}


void
SBitmapButton::MouseDown(BPoint where)
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
SBitmapButton::MouseUp(BPoint where)
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
SBitmapButton::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
	if(!IsEnabled()) return;

	BWindow *window = Window();
	uint32 btns;
	GetMouse(&pt, &btns, false);

	bool drawed = false;

	bool old_mouse_focusing = mouse_focusing;
	if(Bounds().Contains(pt)) mouse_focusing = true;
	else mouse_focusing = false;

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
			drawed = true;
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
			drawed = true;
		}
	}

	if(old_mouse_focusing != mouse_focusing && !drawed)
	{
		Draw(Bounds());
		window->UpdateIfNeeded();
	}
}


void
SBitmapButton::Draw(BRect updateRect)
{
	DrawDefault(Bounds(), waitting, Window()->IsActive());
}


void
SBitmapButton::SetEnableOn(const BBitmap *bitmap)
{
	if(enable_on_bitmap) delete enable_on_bitmap;

	if(bitmap) enable_on_bitmap = new BBitmap(bitmap);
	else enable_on_bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SBitmapButton::SetEnableOn(const char **xpm_data)
{
	if(!xpm_data)
		SetEnableOn((const BBitmap*)NULL);
	else
	{
		SXPM image(xpm_data);
		BBitmap *the_bitmap = image.ConvertToBitmap();
		SetEnableOn(the_bitmap);
		if(the_bitmap) delete the_bitmap;
	}
}


void
SBitmapButton::SetEnableOff(const BBitmap *bitmap)
{
	if(enable_off_bitmap) delete enable_off_bitmap;

	if(bitmap) enable_off_bitmap = new BBitmap(bitmap);
	else enable_off_bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SBitmapButton::SetEnableOff(const char **xpm_data)
{
	if(!xpm_data)
		SetEnableOff((const BBitmap*)NULL);
	else
	{
		SXPM image(xpm_data);
		BBitmap *the_bitmap = image.ConvertToBitmap();
		SetEnableOff(the_bitmap);
		if(the_bitmap) delete the_bitmap;
	}
}


void
SBitmapButton::SetDisableOn(const BBitmap *bitmap)
{
	if(disable_on_bitmap) delete disable_on_bitmap;

	if(bitmap) disable_on_bitmap = new BBitmap(bitmap);
	else disable_on_bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SBitmapButton::SetDisableOn(const char **xpm_data)
{
	if(!xpm_data)
		SetDisableOn((const BBitmap*)NULL);
	else
	{
		SXPM image(xpm_data);
		BBitmap *the_bitmap = image.ConvertToBitmap();
		SetDisableOn(the_bitmap);
		if(the_bitmap) delete the_bitmap;
	}
}


void
SBitmapButton::SetDisableOff(const BBitmap *bitmap)
{
	if(disable_off_bitmap) delete disable_off_bitmap;

	if(bitmap) disable_off_bitmap = new BBitmap(bitmap);
	else disable_off_bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window)
		DrawDefault(Bounds(), false, window->IsActive());
}


void
SBitmapButton::SetDisableOff(const char **xpm_data)
{
	if(!xpm_data)
		SetDisableOff((const BBitmap*)NULL);
	else
	{
		SXPM image(xpm_data);
		BBitmap *the_bitmap = image.ConvertToBitmap();
		SetDisableOff(the_bitmap);
		if(the_bitmap) delete the_bitmap;
	}
}


void
SBitmapButton::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float w = 0, h = 0;

	if(enable_off_bitmap)
	{
		w = MAX(w, enable_off_bitmap->Bounds().Width());
		h = MAX(h, enable_off_bitmap->Bounds().Height());
	}

	if(enable_on_bitmap)
	{
		w = MAX(w, enable_on_bitmap->Bounds().Width());
		h = MAX(h, enable_on_bitmap->Bounds().Height());
	}

	if(disable_off_bitmap)
	{
		w = MAX(w, disable_off_bitmap->Bounds().Width());
		h = MAX(h, disable_off_bitmap->Bounds().Height());
	}

	if(disable_on_bitmap)
	{
		w = MAX(w, disable_on_bitmap->Bounds().Width());
		h = MAX(h, disable_on_bitmap->Bounds().Height());
	}

	w += 2;
	h += 2;

	if(width) *width = w;
	if(height) *height = h;
}


void
SBitmapButton::ResizeToPreferred()
{
	float width, height;
	GetPreferredSize(&width, &height);

	ResizeBy(width - Bounds().Width(), height - Bounds().Height());
}


void
SBitmapButton::WindowActivated(bool state)
{
	DrawDefault(Bounds(), waitting, state);
}


void
SBitmapButton::KeyDown(const char *bytes, int32 numBytes)
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
SBitmapButton::KeyUp(const char *bytes, int32 numBytes)
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
SBitmapButton::SetBehavior(uint32 behavior)
{
	BWindow *window = Window();
	
	if(SBitmapButton::behavior != behavior)
	{
		SBitmapButton::behavior = behavior;
		if(window)
		{
			DrawDefault(Bounds(), waitting, window->IsActive());
			window->UpdateIfNeeded();
		}
	}
}


uint32
SBitmapButton::Behavior() const
{
	return behavior;
}


void
SBitmapButton::SetValue(int32 value)
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
SBitmapButton::Invoke(BMessage *msg)
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
SBitmapButton::DrawDefault(BRect bounds, bool pushed, bool window_activated)
{
	BRect frame = bounds;

	SetPenSize(1);
	SetDrawingMode(B_OP_COPY);
	SetHighColor(ViewColor());
	SetLowColor(B_TRANSPARENT_32_BIT);
	FillRect(frame);

	if(IsEnabled())
	{
		if((!pushed && Behavior() != S_TWO_STATE_BUTTON) ||
		   (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON))
		{
			if(enable_off_bitmap)
			{
				float x = frame.left + (frame.Width() - enable_off_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - enable_off_bitmap->Bounds().Height()) / 2;

				SetDrawingMode(B_OP_OVER);

				if(mouse_focusing)
				{
					SetDrawingMode(B_OP_ERASE);
					SetLowColor(50, 50, 50, 255);
					DrawBitmap(enable_off_bitmap, BPoint(x, y));
					SetDrawingMode(B_OP_ADD);
				}

				DrawBitmap(enable_off_bitmap, BPoint(x, y));
			}
		}
		else
		{
			if(enable_on_bitmap)
			{
				float x = frame.left + (frame.Width() - enable_on_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - enable_on_bitmap->Bounds().Height()) / 2;

				SetDrawingMode(B_OP_OVER);

				if(mouse_focusing && Behavior() == S_TWO_STATE_BUTTON)
				{
					SetDrawingMode(B_OP_ERASE);
					SetLowColor(50, 50, 50, 255);
					DrawBitmap(enable_on_bitmap, BPoint(x, y));
					SetDrawingMode(B_OP_ADD);
				}

				DrawBitmap(enable_on_bitmap, BPoint(x, y));
			}
		}
	}
	else
	{
		if((!pushed && Behavior() != S_TWO_STATE_BUTTON) ||
		   (Value() == 0 && Behavior() == S_TWO_STATE_BUTTON))
		{
			if(!disable_off_bitmap && enable_off_bitmap)
			{
				SetDrawingMode(B_OP_BLEND);
				float x = frame.left + (frame.Width() - enable_off_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - enable_off_bitmap->Bounds().Height()) / 2;
				DrawBitmap(enable_off_bitmap, BPoint(x, y));
			}
			else if(disable_off_bitmap)
			{
				SetDrawingMode(B_OP_OVER);
				float x = frame.left + (frame.Width() - disable_off_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - disable_off_bitmap->Bounds().Height()) / 2;
				DrawBitmap(disable_off_bitmap, BPoint(x, y));
			}
		}
		else
		{
			if(!disable_on_bitmap && enable_on_bitmap)
			{
				SetDrawingMode(B_OP_BLEND);
				float x = frame.left + (frame.Width() - enable_on_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - enable_on_bitmap->Bounds().Height()) / 2;
				DrawBitmap(enable_on_bitmap, BPoint(x, y));
			}
			else if(disable_on_bitmap)
			{
				SetDrawingMode(B_OP_OVER);
				float x = frame.left + (frame.Width() - disable_on_bitmap->Bounds().Width()) / 2;
				float y = frame.top + (frame.Height() - disable_on_bitmap->Bounds().Height()) / 2;
				DrawBitmap(disable_on_bitmap, BPoint(x, y));
			}
		}
	}
}



void
SBitmapButton::AttachedToWindow()
{
	if(Parent())
		SetViewColor(Parent()->ViewColor());
	BControl::AttachedToWindow();
}


void
SBitmapButton::Show()
{
	mouse_focusing = false;
	BControl::Show();
}
