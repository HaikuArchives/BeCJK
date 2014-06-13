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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Screen.h>
#include <Window.h>
#include <Autolock.h>

#include "ImageUtil.h"
#include "HashTable.h"
#include "Strings.h"


SImage::SImage()
{
	color_index = NULL;

	color_index_array = NULL;
	color_array = NULL;
}


SImage::~SImage()
{
	if(color_index) delete[] color_index;

	if(color_index_array) delete[] color_index_array;
	if(color_array) delete[] color_array;
}


int32
SImage::Width() const
{
	return (color_array != NULL ? width : 0);
}


int32
SImage::Height() const
{
	return (color_array != NULL ? height : 0);
}


int32
SImage::CountColors() const
{
	return (color_index != NULL ? ncolors : 0);
}


const rgb_color*
SImage::ColorIndex() const
{
	return color_index;
}


int32
SImage::ColorIndexAt(int32 x, int32 y) const
{
	if(!color_index) return -1;
	if(x < 0 || y < 0) return -1;
	if(Width() == 0 || Height() == 0) return -1;
	if(x >= Width() || y >= Height()) return -1;

	int32 index = y * Width() + x;
	
	return(color_index_array[index]);
}


const rgb_color*
SImage::ColorAt(int32 x, int32 y) const
{
	if(x < 0 || y < 0) return NULL;
	if(Width() == 0 || Height() == 0) return NULL;
	if(x >= Width() || y >= Height()) return NULL;

	int32 index = y * Width() + x;
	
	return(&(color_array[index]));
}


uchar*
SImage::ConvertToIcon()
{
	uchar *ret;
	uchar *data;
	BScreen screen(B_MAIN_SCREEN_ID);
	
	if(!color_array) return NULL;
	
	ret = new uchar[width * height];
	data = ret;

	for(int i = 0; i < width * height; i++)
	{
		if(color_array[i].alpha == 0)
		{
			*data++ = B_TRANSPARENT_8_BIT;
		}
		else
		{
			*data++ = (uchar)screen.IndexForColor(color_array[i]);
		}
	}

	return ret;
}


char*
SImage::ConvertToCursor(uint8 spot_x, uint8 spot_y)
{
	char* ret;
	char* data;
	char tmp[8];
	
	if(!color_array) return NULL;
	if(!(Width() == 16 && Height() == 16 && CountColors() == 3))
	{
		return NULL;
	}


	ret = new char[68];
	data = ret;

	*data++ = (char)16;
	*data++ = (char)1;
	*data++ = (char)spot_x;
	*data++ = (char)spot_y;

	for(uint i = 0; i < 32; i++)
	{
		for(uint j = 0; j < 8; j++)
		{
			if(color_array[i * 8 + j].alpha == 0 ||
			   (color_array[i * 8 + j].red == 255 &&
			   	color_array[i * 8 + j].green == 255 &&
			   	color_array[i * 8 + j].blue == 255))
			{
				tmp[j] = '0';
			}
			else
			{
				tmp[j] = '1';
			}
		}
		
		*data++ = (char)strtol(tmp, NULL, 2);
	}

	for(uint i = 0; i < 32; i++)
	{
		for(uint j = 0; j < 8; j++)
		{
			if(color_array[i * 8 + j].alpha == 0)
			{
				tmp[j] = '0';
			}
			else
			{
				tmp[j] = '1';
			}
		}
		
		*data++ = (char)strtol(tmp, NULL, 2);
	}

	return ret;
}


char*
SImage::ConvertToCursorForProgram(const char* cursor_name, uint8 spot_x, uint8 spot_y)
{
	char* cursor_data = ConvertToCursor(spot_x, spot_y);
	BString string;
	
	if(!cursor_data) return NULL;

	string << "char " << cursor_name << "[] = {\n"
		   << "16,		/* 16x16 size */\n"
		   << "1,		/* 1 bit */\n"
		   << (int)spot_x << "," << (int)spot_y << ",		/* spot point is (" << (int)spot_x << "," << (int)spot_y << ") */\n"
		   << "\n"
		   << "/* such as 00000001 = 1 */\n"
		   << "/*         00000011 = 3 */\n"
		   << "\n"
		   << "/* cursor data */\n";

	for(uint i = 0; i < 16; i++)
		string << (int)cursor_data[4 + 16 * 0 + i] << ",";

	string << "\n";

	for(uint i = 0; i < 16; i++)
		string << (int)cursor_data[4 + 16 * 1 + i] << ",";

	string << "\n"
		   << "\n"
		   << "/* cursor mask */\n";

	for(uint i = 0; i < 16; i++)
		string << (int)cursor_data[4 + 16 * 2 + i] << ",";

	string << "\n";

	for(uint i = 0; i < 16; i++)
	{
		if(i < 15) string << (int)cursor_data[4 + 16 * 3 + i] << ",";
		else string << (int)cursor_data[4 + 16 * 3 + i];
	}

	string << "}\n";

	delete[] cursor_data;
	
	return(Sstrdup(string.String()));
}


BBitmap*
SImage::ConvertToBitmap(const SImage* image)
{
	if(!color_array) return NULL;
	if(Width() == 0 || Height() == 0) return NULL;

	if(image)
	{
		if(!image->color_array) image = NULL;
		else if(image->Width() == 0 || image->Height() == 0) image = NULL;
	}

	int32 w, h, b_w = 0, b_h = 0;

	w = Width();
	h = Height();

	if(image)
	{
		b_w = image->Width();
		b_h = image->Height();
	}

	BBitmap* bitmap = new BBitmap(BRect(0, 0, w - 1, h - 1), B_RGB32);
	if(!bitmap) return NULL;

	uint8 *data, *data_tmp;

	data = new uint8[w * h * 3];
	if(!data) return NULL;
	data_tmp = data;


	for(int32 i = 0; i < h; i++)
	{
		for(int32 j = 0; j < w; j++)
		{
			if(color_array[i * w + j].alpha == 255)
			{
				*data_tmp++ = (uint8)color_array[i * w + j].red;
				*data_tmp++ = (uint8)color_array[i * w + j].green;
				*data_tmp++ = (uint8)color_array[i * w + j].blue;
			}
			else
			{
				if(image == NULL)
				{
					*data_tmp++ = (uint8)B_TRANSPARENT_32_BIT.red;
					*data_tmp++ = (uint8)B_TRANSPARENT_32_BIT.green;
					*data_tmp++ = (uint8)B_TRANSPARENT_32_BIT.blue;
				}
				else
				{
					int32 x, y;
			
					if(b_w >= j + 1)
						x = j;
					else
						x = j%b_w;

					if(b_h >= i + 1)
						y = i;
					else
						y = i%b_h;
			
					*data_tmp++ = (uint8)image->color_array[y * b_w + x].red;
					*data_tmp++ = (uint8)image->color_array[y * b_w + x].green;
					*data_tmp++ = (uint8)image->color_array[y * b_w + x].blue;
				}
			}
		}
	}

	bitmap->SetBits(data, (int32)(w * h * 3), 0, B_RGB32);

	delete[] data;

	return bitmap;
}


BBitmap*
SImage::ConvertToBitmap(BRect rect)
{
	if(!color_array) return NULL;
	if(Width() == 0 || Height() == 0) return NULL;

	if(width <= (int32)rect.right) rect.right = (float)width;
	if(height <= (int32)rect.bottom) rect.bottom = (float)height;
	if(rect.left <= 0) rect.left = 0;
	if(rect.top <= 0) rect.top = 0;

	int32 w, h;
	w = (int32)rect.Width();
	h = (int32)rect.Height();

	BBitmap *bitmap = new BBitmap(BRect(0, 0, w - 1, h - 1), B_RGB32);
	if(!bitmap) return NULL;

	uint8 *data, *data_tmp;

	data = new uint8[w * h * 3];
	if(!data)
	{
		delete bitmap;
		return NULL;
	}
	data_tmp = data;


	for(int32 i = 0; i < h; i++)
	{
		for(int32 j = 0; j < w; j++)
		{
			if(color_array[(i + (int32)rect.top) * width + j + (int32)rect.left].alpha == 255)
			{
				*data_tmp++ = (uint8)color_array[(i + (int32)rect.top) * width + j + (int32)rect.left].red;
				*data_tmp++ = (uint8)color_array[(i + (int32)rect.top) * width + j + (int32)rect.left].green;
				*data_tmp++ = (uint8)color_array[(i + (int32)rect.top) * width + j + (int32)rect.left].blue;
			}
			else
			{
				*data_tmp++ = B_TRANSPARENT_32_BIT.red;
				*data_tmp++ = B_TRANSPARENT_32_BIT.green;
				*data_tmp++ = B_TRANSPARENT_32_BIT.blue;
			}
		}
	}


	bitmap->SetBits(data, (int32)(w * h * 3), 0, B_RGB32);

	delete[] data;

	return bitmap;
}


BBitmap*
SImage::ConvertToBitmapMask(rgb_color f, rgb_color b)
{
	if(!color_array) return NULL;
	if(Width() == 0 || Height() == 0) return NULL;

	BBitmap *bitmap = new BBitmap(BRect(0.0, 0.0, (float)width - 1.0, (float)height - 1.0), B_RGB32);

	if(!bitmap) return NULL;

	uint8 *data, *data_tmp;

	data = new uint8[width * height * 3];
	if(!data)
	{
		delete bitmap;
		return NULL;
	}

	data_tmp = data;

	for(int32 i = 0; i < width * height; i++)
	{
		if(color_array[i].alpha == 255)
		{
			*data_tmp++ = f.red;
			*data_tmp++ = f.green;
			*data_tmp++ = f.blue;
		}
		else
		{
			*data_tmp++ = b.red;
			*data_tmp++ = b.green;
			*data_tmp++ = b.blue;
		}
	}

	bitmap->SetBits(data, (int32)(width * height * 3), 0, B_RGB32);

	delete[] data;

	return bitmap;
}

