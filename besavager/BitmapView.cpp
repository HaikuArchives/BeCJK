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
// Bitmap View supported Tooltip

#include <string.h>
#include <InterfaceDefs.h>
#include <Bitmap.h>
#include <Messenger.h>

#include "Autolock.h"
#include "BitmapView.h"
#include "Strings.h"
#include "Tooltips.h"
#include "XPM.h"

void
SBitmapView::Init(const BBitmap *bitmap, const char *tooltips)
{
	SBitmapView::bitmap = NULL;
	SBitmapView::tooltips = NULL;

	if(bitmap) SBitmapView::bitmap = new BBitmap(bitmap);

	SetTooltips(tooltips);
}


SBitmapView::SBitmapView(BRect frame,
						 const char *name,
						 const BBitmap *bitmap,
						 uint32 resizeMask,
						 uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
	Init(bitmap, NULL);
}


SBitmapView::SBitmapView(BRect frame,
						 const char *name,
						 const BBitmap *bitmap,
						 const char *tooltips,
						 uint32 resizeMask,
						 uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
	Init(bitmap, tooltips);
}


SBitmapView::SBitmapView(BRect frame,
						 const char *name,
						 const char **xpm_data,
						 uint32 resizeMask,
						 uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *the_bitmap = image.ConvertToBitmap();

	Init(the_bitmap, NULL);

	if(the_bitmap) delete the_bitmap;
}


SBitmapView::SBitmapView(BRect frame,
						 const char *name,
						 const char **xpm_data,
						 const char *tooltips,
						 uint32 resizeMask,
						 uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
	SXPM image(xpm_data);
	BBitmap *the_bitmap = image.ConvertToBitmap();

	Init(the_bitmap, tooltips);

	if(the_bitmap) delete the_bitmap;
}


SBitmapView::~SBitmapView()
{
	if(tooltips) delete[] tooltips;
	if(bitmap) delete bitmap;
}


void
SBitmapView::SetTooltips(const char *tooltips)
{
	BWindow *window = Window();

	if(SBitmapView::tooltips) delete[] SBitmapView::tooltips;

	if(tooltips)
		SBitmapView::tooltips = Sstrdup(tooltips);
	else
		SBitmapView::tooltips = NULL;

	if(window && tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);

		msg.AddBool("state", false);
	
		BMessenger msgr(NULL, window);
		msgr.SendMessage(&msg);
	}
}


void
SBitmapView::MouseDown(BPoint where)
{
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
SBitmapView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
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
			msg.AddPoint("where", ConvertToScreen(Bounds().LeftBottom() + BPoint(1, 1)));
			msg.AddRect("frame", ConvertToScreen(Bounds()));
			msg.AddInt32("style", S_TOOLTIPS_FOLLOW_MOUSE_X);
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
SBitmapView::Draw(BRect updateRect)
{
	PushState();

	SetPenSize(1);
	SetDrawingMode(B_OP_COPY);
	SetHighColor(ViewColor());
	FillRect(updateRect);

	if(bitmap)
	{
		SAutolock<BBitmap> autolock(bitmap);

		BRect frame;
		frame.left = Bounds().left + (Bounds().Width() - bitmap->Bounds().Width()) / 2;
		frame.top = Bounds().top + (Bounds().Height() - bitmap->Bounds().Height()) / 2;
		frame.right = frame.left + bitmap->Bounds().Width();
		frame.bottom = frame.top + bitmap->Bounds().Height();

		BRect rect = frame & updateRect;
		
		if(rect.IsValid())
		{
			BRect r = rect.OffsetByCopy(0 - frame.left, 0 - frame.top);
			SetDrawingMode(B_OP_OVER);
			DrawBitmapAsync(bitmap, r, rect);
		}
	}

	PopState();
}


void
SBitmapView::SetBitmap(const BBitmap *bitmap)
{
	if(SBitmapView::bitmap) delete SBitmapView::bitmap;

	if(bitmap) SBitmapView::bitmap = new BBitmap(bitmap);
	else SBitmapView::bitmap = NULL;
	
	BWindow *window = Window();
	
	if(window) Draw(Bounds());
}


void
SBitmapView::SetBitmap(const char **xpm_data)
{
	SXPM image(xpm_data);
	BBitmap *the_bitmap = image.ConvertToBitmap();

	SetBitmap(the_bitmap);

	if(the_bitmap) delete the_bitmap;
}


void
SBitmapView::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float w = 0, h = 0;

	if(bitmap)
	{
		w = MAX(w, bitmap->Bounds().Width());
		h = MAX(h, bitmap->Bounds().Height());
	}

	w += 2;
	h += 2;

	if(width) *width = w;
	if(height) *height = h;
}


void
SBitmapView::ResizeToPreferred()
{
	float width, height;
	GetPreferredSize(&width, &height);

	ResizeBy(width - Bounds().Width(), height - Bounds().Height());
}


void
SBitmapView::AttachedToWindow()
{
	if(Parent()) SetViewColor(Parent()->ViewColor());
	BView::AttachedToWindow();
}
