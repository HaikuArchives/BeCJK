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

#include "DragBar.h"
#include "Strings.h"
#include "Cursor.h"


#include <Window.h>
#include <Message.h>
#include <Messenger.h>
#include <Screen.h>


SDragBar::SDragBar(BRect window_bounds, const char *name, int32 position, const char *tooltips, bool within_screen)
	: BView(window_bounds, name, 0, B_WILL_DRAW), tracking(false)
{
	SDragBar::within_screen = within_screen;
	SDragBar::window_bounds = window_bounds;

	SDragBar::position = position;
	SDragBar::tooltips = NULL;

	if(tooltips) SDragBar::tooltips = Sstrdup(tooltips);

	BRect frame = BarBounds();
	ResizeTo(frame.Width(), frame.Height());
	MoveTo(frame.LeftTop());

	cursor = new BCursor(S_CURSOR_MOVE);

	switch(position)
	{
		case S_DRAG_BAR_TOP:
			SetResizingMode(B_FOLLOW_ALL &~ B_FOLLOW_BOTTOM);
			break;

		case S_DRAG_BAR_BOTTOM:
			SetResizingMode(B_FOLLOW_ALL &~ B_FOLLOW_TOP);
			break;

		case S_DRAG_BAR_RIGHT:
			SetResizingMode(B_FOLLOW_ALL &~ B_FOLLOW_LEFT);
			break;

		default:
			SetResizingMode(B_FOLLOW_ALL &~ B_FOLLOW_RIGHT);
	}
}


SDragBar::~SDragBar()
{
	if(tooltips) delete[] tooltips;
	if(cursor) delete cursor;
}


BRect
SDragBar::BarBounds()
{
	BWindow *window = Window();
	BRect frame;

	if(window)
		frame = ConvertFromScreen(window->ConvertToScreen(window->Bounds()));
	else
		frame = window_bounds;

	switch(position)
	{
		case S_DRAG_BAR_TOP:
			frame.bottom = frame.top + 4;
			break;

		case S_DRAG_BAR_BOTTOM:
			frame.top = frame.bottom - 4;
			break;

		case S_DRAG_BAR_RIGHT:
			frame.left = frame.right - 4;
			break;

		default:
			frame.right = frame.left + 4;
	}

	return frame;
}


BRect
SDragBar::ChildBounds()
{
	BWindow *window = Window();
	if(!window) return BRect();

	BRect frame;

	frame = window->Bounds();

	switch(position)
	{
		case S_DRAG_BAR_TOP:
			frame.top += 5;
			break;

		case S_DRAG_BAR_BOTTOM:
			frame.bottom -= 5;
			break;

		case S_DRAG_BAR_RIGHT:
			frame.right -= 5;
			break;

		default:
			frame.left += 5;
	}

	return frame;
}


void
SDragBar::DrawBarBounds()
{
	BRect frame = BarBounds();

	SetDrawingMode(B_OP_COPY);

	if(!(Window()->Flags() & B_AVOID_FOCUS))
	{
		if(!Window()->IsActive())
			SetHighColor(200, 148, 0, 255);
		else
			SetHighColor(255, 203, 0, 255);
	}
	else
	{
//		SetHighColor(0, 128, 0, 255);
		SetHighColor(255, 203, 0, 255);
	}
	FillRect(frame);

	PushState();

	SetDrawingMode(B_OP_ALPHA);
	SetHighColor(255, 255, 255, 100);
	StrokeRect(frame);
	frame.left += 1;
	frame.top += 1;
	SetHighColor(0, 0, 0, 100);
	StrokeRect(frame);
	frame.left -= 1;
	frame.top -= 1;

	frame.InsetBy(1, 1);

	PopState();
	FillRect(frame);
}


void
SDragBar::Draw(BRect updateRect)
{
	BRect frame = Bounds();

	SetDrawingMode(B_OP_COPY);
	SetHighColor(ViewColor());
	FillRect(frame);

	DrawBarBounds();
	
	BView::Draw(updateRect);
}


void
SDragBar::MouseDown(BPoint where)
{
	BWindow *window = Window();

	uint32 btns;
	GetMouse(&where, &btns, false);

	if(!(window->Flags() & B_AVOID_FOCUS)) window->Activate();

	if(tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}

	if(BarBounds().Contains(where))
	{
		tracking = true;
		old_position = ConvertToScreen(where);
		SetMouseEventMask(B_POINTER_EVENTS, B_SUSPEND_VIEW_FOCUS | B_LOCK_WINDOW_FOCUS);
	}
}


void
SDragBar::MouseUp(BPoint where)
{
	if(tracking)
		tracking = false;
}


void
SDragBar::MouseMoved(BPoint pt, uint32 code, const BMessage *message)
{
	BWindow *window = Window();

	if(cursor) SetViewCursor(cursor);

	uint32 btns;
	GetMouse(&pt, &btns, false);

	if(tracking)
	{
		BPoint new_position = ConvertToScreen(pt);

		if(old_position != new_position)
		{
			BPoint dis = new_position - old_position;
			
			if(within_screen)
			{
				BScreen screen(window);
				BRect rect = screen.Frame();
				rect.OffsetTo(B_ORIGIN);
				
				BRect win_rect = window->Frame();
				win_rect.OffsetBy(dis.x, dis.y);

				float w, h;
				w = win_rect.Width();
				h = win_rect.Height();				

				if(!rect.Contains(win_rect))
				{
					if(win_rect.right > rect.right)
					{
						win_rect.right = rect.right;
						win_rect.left = rect.right - w;
					}

					if(win_rect.left < rect.left)
					{
						win_rect.left = rect.left;
						win_rect.right = rect.left + w;
					}

					if(win_rect.bottom > rect.bottom)
					{
						win_rect.bottom = rect.bottom;
						win_rect.top = rect.bottom - h;
					}

					if(win_rect.top < rect.top)
					{
						win_rect.top = rect.top;
						win_rect.bottom = rect.top + h;
					}
					
					window->MoveTo(win_rect.LeftTop());
				}
				else
				{
					window->MoveBy(dis.x, dis.y);
				}
			}
			else
			{
				window->MoveBy(dis.x, dis.y);
			}
			old_position = new_position;
		}
	}
	else if(tooltips)
	{
		if(BarBounds().Contains(pt) && code == B_ENTERED_VIEW)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);
			msg.AddBool("state", true);
			msg.AddString("tooltips", tooltips);
			msg.AddRect("frame", ConvertToScreen(BarBounds()));
			
			msg.AddPoint("where", ConvertToScreen(BarBounds().LeftBottom() + BPoint(0, 1)));
			msg.AddInt32("style", S_TOOLTIPS_FOLLOW_MOUSE_X);

			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}
		else if(!BarBounds().Contains(pt) && code == B_EXITED_VIEW)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);
			msg.AddBool("state", false);
			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}
	}
}


void
SDragBar::SetTooltips(const char *tooltips)
{
	BWindow *window = Window();

	char *old_tooltips = SDragBar::tooltips;

	if(tooltips)
		SDragBar::tooltips = Sstrdup(tooltips);
	else
		SDragBar::tooltips = NULL;

	if(old_tooltips)
	{
		if(window && tooltips)
		{
			BMessage msg(S_TOOLTIPS_CHANGED);

			msg.AddBool("state", false);
	
			BMessenger msgr(NULL, window);
			msgr.SendMessage(&msg);
		}

		delete[] old_tooltips;
	}
}


void
SDragBar::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float w, h;

	w = Bounds().Width();
	h = Bounds().Height();

	switch(position)
	{
		case S_DRAG_BAR_TOP:
		case S_DRAG_BAR_BOTTOM:
			if(Window() != NULL) w = Window()->Bounds().Width();
			h = 4;
			break;

		default:
			w = 4;
			if(Window() != NULL) h = Window()->Bounds().Height();
	}
	
	if(width) *width = w;
	if(height) *height = h;
}


void
SDragBar::WindowActivated(bool state)
{
	if(!(Window()->Flags() & B_AVOID_FOCUS))
	{
		Draw(Bounds());
		Window()->UpdateIfNeeded();
	}
}


void
SDragBar::SetWithInScreen(bool within)
{
	within_screen = within;
}

