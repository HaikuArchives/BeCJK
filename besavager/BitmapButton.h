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
// Bitmap Button supported Tooltip like BPictureButton

#ifndef __SAVAGER_BITMAP_BUTTON_H__
#define __SAVAGER_BITMAP_BUTTON_H__

#include <OS.h>
#include <Bitmap.h>
#include <Control.h>
#include <besavager/Button.h>

class SBitmapButton : public BControl {
public:
	SBitmapButton(BRect frame,
				  const char *name,
				  const BBitmap *off,
				  const BBitmap *on,
				  BMessage *message,
				  uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				  uint32 flags = B_WILL_DRAW);

	SBitmapButton(BRect frame,
				  const char *name,
				  const char **off_xpm_data,
				  const char **on_xpm_data,
				  BMessage *message,
				  uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				  uint32 flags = B_WILL_DRAW);

	SBitmapButton(BRect frame,
				  const char *name,
				  const BBitmap *off,
				  const BBitmap *on,
				  const char *tooltips,
				  BMessage *message,
				  uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				  uint32 flags = B_WILL_DRAW);

	SBitmapButton(BRect frame,
				  const char *name,
				  const char **off_xpm_data,
				  const char **on_xpm_data,
				  const char *tooltips,
				  BMessage *message,
				  uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				  uint32 flags = B_WILL_DRAW);

	virtual ~SBitmapButton();

	void SetTooltips(const char *tooltips);
	virtual void TooltipsLocation(BMessage *message); // custom

	virtual	void Draw(BRect updateRect);

	virtual void SetEnableOn(const BBitmap *bitmap);
	virtual void SetEnableOff(const BBitmap *bitmap);
	virtual void SetDisableOn(const BBitmap *bitmap);
	virtual void SetDisableOff(const BBitmap *bitmap);

	void SetEnableOn(const char **xpm_data);
	void SetEnableOff(const char **xpm_data);
	void SetDisableOn(const char **xpm_data);
	void SetDisableOff(const char **xpm_data);

	virtual	void KeyDown(const char *bytes, int32 numBytes);
	virtual	void KeyUp(const char *bytes, int32 numBytes);

	virtual	void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	virtual	void MouseDown(BPoint where);
	virtual	void MouseUp(BPoint where);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

	virtual void WindowActivated(bool state);

	virtual	void SetBehavior(uint32 behavior);
	uint32	Behavior() const;
	virtual	void SetValue(int32 value);
	virtual	status_t Invoke(BMessage *msg = NULL); // including drawing within Bounds()

	virtual void AttachedToWindow();

	virtual void Show();

private:
	void Init(const BBitmap *off, const BBitmap *on, const char *tooltips);

	char *tooltips;
	BBitmap *enable_off_bitmap;
	BBitmap *enable_on_bitmap;
	BBitmap *disable_off_bitmap;
	BBitmap *disable_on_bitmap;

	void DrawDefault(BRect bounds, bool pushed, bool window_activated = true);

	bool waitting;
	bool waitting_is_keyboard;

	uint32 behavior;
	
	bool mouse_focusing;
};


#endif /* __SAVAGER_BITMAP_BUTTON_H__ */
