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
// Separator View

#include "Separator.h"

#include <Window.h>

SSeparator::SSeparator(BRect frame,
					   const char *name,
					   int32 separator_style,
					   uint32 resizeMask,
					   uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
	style = separator_style;
}


void
SSeparator::Draw(BRect updateRect)
{
	BRect frame = Bounds();

	SetPenSize(1);
	SetDrawingMode(B_OP_COPY);
	SetHighColor(ViewColor());
	FillRect(frame);

	switch(style)
	{
		case S_SEPARATOR_V:
			{
				BPoint pt1, pt2;
				pt2.x = frame.left + frame.Width() / 2;
				pt2.y = frame.top;
				pt1.x = pt2.x;
				pt1.y = frame.bottom;
				
				SetDrawingMode(B_OP_ALPHA);

				SetHighColor(0, 0, 0, 70);
				StrokeLine(pt1, pt2);

				SetHighColor(250, 250, 250, 150);
				StrokeLine(pt1 + BPoint(1, 0), pt2 + BPoint(1, 0));
			}
			break;
		
		default:
			{
				BPoint pt1, pt2;
				pt1.x = frame.left;
				pt1.y = frame.top + frame.Height() / 2;
				pt2.x = frame.right;
				pt2.y = pt1.y;
				
				SetDrawingMode(B_OP_ALPHA);

				SetHighColor(0, 0, 0, 70);
				StrokeLine(pt1, pt2);

				SetHighColor(250, 250, 250, 150);
				StrokeLine(pt1 + BPoint(0, 1), pt2 + BPoint(0, 1));
			}
	}
}


void
SSeparator::AttachedToWindow()
{
	if(Parent())
		SetViewColor(Parent()->ViewColor());
	BView::AttachedToWindow();
}


void
SSeparator::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	float w, h;

	switch(style)
	{
		case S_SEPARATOR_V:
			w = 4;
			h = Bounds().Height();
			break;
		
		default:
			w = Bounds().Width();
			h = 4;
	}

	if(width) *width = w;
	if(height) *height = h;
}

