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
// Window supported Tooltip

#ifndef __SAVAGER_WINDOW_H__
#define __SAVAGER_WINDOW_H__

#include <Window.h>
#include <besavager/Tooltips.h>

// support tooltips for children
class SWindow : public BWindow {
public:
	SWindow(BRect frame,
			const char *title,
			window_type type,
			uint32 flags,
			uint32 workspace = B_CURRENT_WORKSPACE);

	SWindow(BRect frame,
			const char *title,
			window_look look,
			window_feel feel,
			uint32 flags,
			uint32 workspace = B_CURRENT_WORKSPACE);

	virtual ~SWindow();

	virtual void MessageReceived(BMessage *message);

	virtual	void FrameMoved(BPoint new_position);
	virtual void WorkspacesChanged(uint32 old_ws, uint32 new_ws);
	virtual	void FrameResized(float new_width, float new_height);
	virtual void Minimize(bool minimize);
	virtual void Zoom(BPoint rec_position, float rec_width, float rec_height);
	virtual	void Hide();

	virtual void WindowActivated(bool state);

	virtual bool SupportNotActivatedTooltips() const;

private:
	STooltips *tooltips;
};


#endif /* __SAVAGER_WINDOW_H__ */

