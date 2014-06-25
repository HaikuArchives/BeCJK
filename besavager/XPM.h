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
// Read from XPM
// yanked from GDK-PIXBUF


#ifndef __SAVAGER_XPM_H__
#define __SAVAGER_XPM_H__

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <Bitmap.h>

#include <besavager/ImageUtil.h>

class SXPMScanner;

class SXPM : public SImage {
public:
	SXPM();
	SXPM(const char **xpm_data);
	SXPM(const char *filename);
	virtual ~SXPM();

	bool SetTo(const char **xpm_data);
	bool SetTo(const char *filename);

private:
	bool Generate(SXPMScanner *scanner);
};

#endif /* __SAVAGER_XPM_H__ */
