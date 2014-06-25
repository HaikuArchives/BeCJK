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

#ifndef __SAVAGER_BITMAP_VIEW_H__
#define __SAVAGER_BITMAP_VIEW_H__

#include <Bitmap.h>
#include <View.h>

class SBitmapView : public BView {
public:
	SBitmapView(BRect frame,
				const char *name,
				const BBitmap *bitmap,
				uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW);

	SBitmapView(BRect frame,
				const char *name,
				const BBitmap *bitmap,
				const char *tooltips,
				uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW);

	SBitmapView(BRect frame,
				const char *name,
				const char **xpm_data,
				uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW);

	SBitmapView(BRect frame,
				const char *name,
				const char **xpm_data,
				const char *tooltips,
				uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW);

	virtual ~SBitmapView();

	void SetTooltips(const char *tooltips);

	virtual	void Draw(BRect updateRect);

	virtual void SetBitmap(const BBitmap *bitmap);
	void SetBitmap(const char **xpm_data);

	virtual	void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	virtual	void MouseDown(BPoint where);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

	virtual void AttachedToWindow();

private:
	void Init(const BBitmap *bitmap, const char *tooltips);

	BBitmap *bitmap;
	char *tooltips;
};


#endif /* __SAVAGER_BITMAP_VIEW_H__ */
