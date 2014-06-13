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
// Image Utility

#ifndef __SAVAGER_IMAGE_UTIL_H__
#define __SAVAGER_IMAGE_UTIL_H__

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <Bitmap.h>

class SImage {
public:
	SImage();
	virtual ~SImage();

	int32 Width() const;
	int32 Height() const;
	int32 CountColors() const;

	const rgb_color* ColorIndex() const;
	int32 ColorIndexAt(int32 x, int32 y) const; // return -1 when error

	const rgb_color* ColorAt(int32 x, int32 y) const;

	// ConvertToIcon(),ConvertToCursor(),ConvertToCursorForProgram():
	// the return value must to be free by "delete[]"
	uchar*	ConvertToIcon();
	char*	ConvertToCursor(uint8 spot_x, uint8 spot_y);
	char*	ConvertToCursorForProgram(const char* cursor_name, uint8 spot_x, uint8 spot_y);

	// ConvertToBitmap(), ConvertToBitmapMask():
	// the return value must to be free by "delete"
	BBitmap* ConvertToBitmap(const SImage* background = NULL);
	BBitmap* ConvertToBitmap(BRect source_rect);
	BBitmap* ConvertToBitmapMask(rgb_color foreground, rgb_color background = B_TRANSPARENT_32_BIT);

protected:
	int32 width;
	int32 height;
	int32 ncolors;

	rgb_color *color_index;

	int32 *color_index_array;
	rgb_color *color_array;
};

#endif /* __SAVAGER_IMAGE_UTIL_H__ */
