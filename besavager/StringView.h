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

#ifndef __SAVAGER_STRING_VIEW_H__
#define __SAVAGER_STRING_VIEW_H__

#include <Region.h>
#include <View.h>
#include <List.h>
#include <Bitmap.h>
#include <besavager/Strings.h>

inline void s_rgb_color_setto(rgb_color *dest, rgb_color src)
{
	if(!dest) return;

	dest->red = src.red;
	dest->green = src.green;
	dest->blue = src.blue;
	dest->alpha = src.alpha;
}


inline void s_rgb_color_setto(rgb_color *dest, uint8 red, uint8 green, uint8 blue, uint8 alpha = 255)
{
	if(!dest) return;

	dest->red = red;
	dest->green = green;
	dest->blue = blue;
	dest->alpha = alpha;
}


inline bool s_rgb_color_is_equal(rgb_color a, rgb_color b)
{
	return (*((uint32*)&a)) == (*((uint32*)&b));
}


typedef struct s_string_view_color {
	rgb_color color;
	int32 start_offset;
	int32 end_offset;
} s_string_view_color;


class SStringView : public BView {
public:
	SStringView(BRect bounds,
				const char *name,
				const char *text,
				const s_string_view_color *colors = NULL,
				int32 ncolors = 0,
				const char *tooltips = NULL,
				uint32 resizeFlags = B_FOLLOW_LEFT | B_FOLLOW_TOP,
				uint32 flags = B_WILL_DRAW | B_FRAME_EVENTS);
	virtual ~SStringView();

	void		SetTooltips(const char *tooltips);
	
	virtual void SetEnabled(bool on);
	bool IsEnabled() const;

	void		SetText(const char *text, const s_string_view_color *colors = NULL, int32 ncolors = 0);
	const char	*Text() const;

	void		SetAlignment(alignment flag);
	alignment	Alignment() const;

	virtual	void SetViewColor(rgb_color c);
	void SetViewColor(uchar r, uchar g, uchar b, uchar a = 255) {BView::SetViewColor(r, g, b, a);};

	virtual	void AttachedToWindow();
	virtual	void Draw(BRect bounds);

	virtual	void MouseDown(BPoint pt);
	virtual	void MouseUp(BPoint pt);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);

	virtual void ResizeToPreferred();
	virtual void GetPreferredSize(float *width, float *height);

	void GetPreferredSizeByText(const char *string, float *width, float *height);

	virtual	void FrameMoved(BPoint new_position);
	virtual	void FrameResized(float new_width, float new_height);

	void Highlight(int32 startOffset, int32 endOffset);

	void RefreshView(BRect rect);
	void RefreshView();

	BRegion TextRegionByText(const char *string, int32 startOffset, int32 endOffset);

protected:
	BBitmap *BufferBitmap();
	BView *BufferBitmapView();
	void GenerateBuffer();

	BRegion TextRegion(int32 startOffset, int32 endOffset);
	void DrawText(BView* view, int32 startOffset, int32 endOffset);

	virtual void DrawHighlight(BView *view, int32 startOffset, int32 endOffset);

private:
	SString text;
	alignment align;
	char *tooltips;

	bool view_color_custom;
	bool enabled;

	int32 highlight_start;
	int32 highlight_end;

	BBitmap *bitmap;
	BView *bitmap_view;

	BList *color_list;

	void DrawOldHighlightToNormal(BView *view, int32 startOffset, int32 endOffset);
	void EmptyColorList();
	void SetColorList(const s_string_view_color *colors, int32 ncolors);
	void __DrawText(BView* view, int32 startOffset, int32 endOffset, rgb_color color);
};


inline void
SStringView::RefreshView()
{
	RefreshView(Bounds());
}


#endif /* __SAVAGER_STRING_VIEW_H__ */
