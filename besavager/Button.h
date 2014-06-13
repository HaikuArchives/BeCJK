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

#ifndef __SAVAGER_BUTTON_H__
#define __SAVAGER_BUTTON_H__

#include <OS.h>
#include <Button.h>
#include <Bitmap.h>

enum {
	S_ONE_STATE_BUTTON		= 0,
	S_TWO_STATE_BUTTON		= 1,
};


typedef enum SButtonStyle {
	R5_STYLE = 0,
	DANO_STYLE = 1,
} SButtonStyle;


class SButton : public BButton {
public:
	SButton(BRect frame,
			const char *name,
			const BBitmap *bitmap,
			BMessage *message,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char **xpm_data,
			BMessage *message,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char *label,
			BMessage *message,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const BBitmap *bitmap,
			const char *label,
			BMessage *message,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char **xpm_data,
			const char *label,
			BMessage *message,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const BBitmap *bitmap,
			BMessage *message,
			const char* tooltips,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char **xpm_data,
			BMessage *message,
			const char* tooltips,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char *label,
			BMessage *message,
			const char* tooltips,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const BBitmap *bitmap,
			const char *label,
			BMessage *message,
			const char* tooltips,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	SButton(BRect frame,
			const char *name,
			const char **xpm_data,
			const char *label,
			BMessage *message,
			const char* tooltips,
			uint32 resizeMask = B_FOLLOW_LEFT | B_FOLLOW_TOP,
			uint32 flags = B_WILL_DRAW | B_NAVIGABLE);

	virtual ~SButton();

	virtual void SetStyle(SButtonStyle style);
	SButtonStyle Style() const;

	void SetTooltips(const char *tooltips);
	virtual void TooltipsLocation(BMessage *message); // custom

	virtual void GetContentPreferredSize(float *width, float *height); // custom
	virtual void DrawContent(BRect content_rect, bool pushed, bool window_activated = true); // custom

	virtual	void Draw(BRect updateRect);

	virtual	void SetLabel(const char *text);
	virtual void SetBitmap(const BBitmap *bitmap);

	const char* Label() const;
	const BBitmap* Bitmap() const;

	virtual	void KeyDown(const char *bytes, int32 numBytes);
	virtual	void KeyUp(const char *bytes, int32 numBytes);
	virtual	void MakeFocus(bool focusState = true);

	virtual	void GetPreferredSize(float *width, float *height);
	virtual void ResizeToPreferred();

	virtual	void MouseDown(BPoint where);
	virtual	void MouseUp(BPoint where);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

	virtual void WindowActivated(bool state);

	virtual void MakeDefault(bool flag);
	bool IsDefault() const;

	virtual void MakeAutoDefault(bool flag);
	bool IsAutoDefault() const;

	virtual void Pulse(); // used to flash the edge when focused

	virtual	void SetBehavior(uint32 behavior);
	uint32	Behavior() const;
	virtual	void SetValue(int32 value);
	virtual	status_t Invoke(BMessage *msg = NULL); // including drawing within Bounds()


	virtual void AttachedToWindow();

private:
	void Init(const BBitmap *bitmap, const char *label, const char *tooltips);

	char *tooltips;
	BBitmap *bitmap;
	char *label;

	void DrawDefault(BRect bounds, bool pushed, bool window_activated = true);
	BRect BitmapRect();

	void draw_r5(BRect bounds, bool pushed, bool window_activated);
	void draw_dano(BRect bounds, bool pushed, bool window_activated);

	bool flash_r5();
	bool flash_dano();

	bool waitting;
	bool waitting_is_keyboard;
	bool focused;
	bool is_default;
	bool is_auto_default;

	bigtime_t old_time;
	bool edge_is_black;

	uint32 behavior;

	SButtonStyle button_style;
};


#endif /* __SAVAGER_BUTTON_H__ */
