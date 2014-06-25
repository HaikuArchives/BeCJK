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
// Tooltip Manager

#ifndef __SAVAGER_TOOLTIPS_H__
#define __SAVAGER_TOOLTIPS_H__

#include <SupportDefs.h>
#include <OS.h>
#include <Looper.h>
#include <Window.h>


enum {
	S_TOOLTIPS_FOLLOW_NONE			= 0,
	S_TOOLTIPS_FOLLOW_MOUSE_X		= 1,
	S_TOOLTIPS_FOLLOW_MOUSE_Y		= 2,
	S_TOOLTIPS_FOLLOW_MOUSE			= 3,
	S_TOOLTIPS_FOLLOW_FRAME_CENTER	= 4,
};


//
// tooltips message : send a message for show/hide the tooltip
// REGION:
//		"state" -- bool -- whether to show the tooltips
//		"tooltips" -- string -- the tooltips
//		"frame" -- BRect -- the rectangle(frame) region of widget (screen coordinates)
//		"style" -- int32 -- the position style
//		"where" -- BPoint -- the position when the style is S_TOOLTIPS_FOLLOW_NONE (screen coordinates)
#define S_TOOLTIPS_CHANGED		'stt_'


class STooltipsWindow;
class SWindow;

class STooltips : public BLooper {
public:
	STooltips(SWindow *win);
	virtual ~STooltips();

	virtual void MessageReceived(BMessage *message);

	static BRect Measure(const char* tooltips);

private:
	friend class STooltipsWindow;
	STooltipsWindow *tooltips_window;
	SWindow *orig_window;
};

#endif /* __SAVAGER_TOOLTIPS_H__ */
