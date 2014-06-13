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
// MenuItem

#include "MenuItem.h"
#include "Strings.h"
#include "XPM.h"

SMenuItem::SMenuItem(const BBitmap *bitmap,
					 const char *label,
					 BMessage *message,
					 char shortcut,
					 uint32 modifiers)
	: BMenuItem(label, message, shortcut, modifiers)
{
	SMenuItem::bitmap = NULL;
	SMenuItem::label = NULL;

	if(bitmap) SMenuItem::bitmap = new BBitmap(bitmap);
	if(label) SMenuItem::label = Sstrdup(label);
}


SMenuItem::SMenuItem(const char **xpm_data,
					 const char *label,
					 BMessage *message,
					 char shortcut,
					 uint32 modifiers)
	: BMenuItem(label, message, shortcut, modifiers)
{
	bitmap = NULL;
	SMenuItem::label = NULL;

	if(xpm_data)
	{
		SXPM image(xpm_data);
		bitmap = image.ConvertToBitmap();
	}

	if(label) SMenuItem::label = Sstrdup(label);
}


SMenuItem::SMenuItem(const BBitmap *bitmap, BMenu *menu, BMessage *message, const char *label)
	: BMenuItem(menu, message)
{
	SMenuItem::bitmap = NULL;
	SMenuItem::label = NULL;

	if(bitmap)
		SMenuItem::bitmap = new BBitmap(bitmap);

	if(label)
	{
		SMenuItem::label = Sstrdup(label);
	}
	else if(menu->Superitem())
	{
		if(menu->Superitem()->Label())
			SMenuItem::label = Sstrdup(menu->Superitem()->Label());
	}

	SetLabel(SMenuItem::label);
}


SMenuItem::SMenuItem(const char **xpm_data, BMenu *menu, BMessage *message, const char *label)
	: BMenuItem(menu, message)
{
	bitmap = NULL;
	SMenuItem::label = NULL;

	if(xpm_data)
	{
		SXPM image(xpm_data);
		bitmap = image.ConvertToBitmap();
	}

	if(label)
	{
		SMenuItem::label = Sstrdup(label);
	}
	else if(menu->Superitem())
	{
		if(menu->Superitem()->Label())
			SMenuItem::label = Sstrdup(menu->Superitem()->Label());
	}

	SetLabel(SMenuItem::label);
}


SMenuItem::~SMenuItem()
{
	if(bitmap) delete bitmap;
	if(label) delete[] label;
}


void
SMenuItem::GetContentSize(float *width, float *height)
{
	if(!width && !height) return;

	float w = 0, h = 0;
	BMenuItem::GetContentSize(&w, &h);

	if(h < 18) h = 18;

	w += h + 5;

	if(width) *width = w;
	if(height) *height = h;
}


void
SMenuItem::DrawContent()
{
	if(label)
	{
		float w, h;
		GetContentSize(&w, &h);

		font_height fheight;
		Menu()->GetFontHeight(&fheight);
		float font_height = fheight.ascent + fheight.leading + fheight.descent;

		BPoint pt = ContentLocation();
		pt.x += h + 5;
		pt.y += (h - font_height) / 2;
		
		Menu()->MovePenTo(pt);
		BMenuItem::DrawContent();
	}

	DrawBitmap(IsSelected());
}


void
SMenuItem::Highlight(bool on)
{
	BMenuItem::Highlight(on);
	DrawBitmap(on);
}


void
SMenuItem::DrawBitmap(bool highlight)
{
	if(!bitmap) return;

	Menu()->PushState();
	
	float w, h;
	GetContentSize(&w, &h);

	BPoint pt = ContentLocation();
	BRect dest;
	dest.left = pt.x;
	dest.top = pt.y + 1;
	dest.right = pt.x + h - 2;
	dest.bottom = pt.y + h - 2;

	Menu()->SetLowColor(255, 255, 255, 0);

	if(IsEnabled()) Menu()->SetDrawingMode(B_OP_OVER);
	else Menu()->SetDrawingMode(B_OP_BLEND);

	if(highlight && IsEnabled())
	{
		Menu()->SetDrawingMode(B_OP_ERASE);
		Menu()->SetLowColor(50, 50, 50, 255);
		Menu()->DrawBitmapAsync(bitmap, bitmap->Bounds(), dest);
		Menu()->SetDrawingMode(B_OP_ADD);
	}

	Menu()->DrawBitmap(bitmap, bitmap->Bounds(), dest);
	
	Menu()->PopState();
}

