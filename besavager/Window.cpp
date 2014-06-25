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

#include "Window.h"
#include "Strings.h"
#include <View.h>
#include <Message.h>
#include <Messenger.h>

SWindow::SWindow(BRect frame,
				 const char *title,
				 window_type type,
				 uint32 flags,
				 uint32 workspace)
	: BWindow(frame, title, type, flags, workspace), tooltips(NULL)
{
 	if(!(Flags() & B_WILL_ACCEPT_FIRST_CLICK))
 		SetFlags(Flags() | B_WILL_ACCEPT_FIRST_CLICK);

	tooltips = new STooltips(this);
}


SWindow::SWindow(BRect frame,
				 const char *title,
				 window_look look,
				 window_feel feel,
				 uint32 flags,
				 uint32 workspace)
	: BWindow(frame, title, look, feel, flags, workspace), tooltips(NULL)
{
 	if(!(Flags() & B_WILL_ACCEPT_FIRST_CLICK))
 		SetFlags(Flags() | B_WILL_ACCEPT_FIRST_CLICK);

	tooltips = new STooltips(this);
}


SWindow::~SWindow()
{
	if(tooltips)
	{
		tooltips->Lock();
		tooltips->Quit();
		tooltips = NULL;
	}
}


void
SWindow::MessageReceived(BMessage *message)
{
	if(!message) return;
	
	switch(message->what)
	{
		case S_TOOLTIPS_CHANGED:
			{
				if(!tooltips) break;
				if(!IsActive() && !SupportNotActivatedTooltips()) break;

				message->AddInt32("workspace", (int32)Workspaces());

				BMessenger msgr(NULL, tooltips);
				msgr.SendMessage(message);
			}
			break;

		default:
			BWindow::MessageReceived(message);
	}
}


void
SWindow::FrameMoved(BPoint new_position)
{
	if(!tooltips) return;
	BMessage msg(S_TOOLTIPS_CHANGED);
	msg.AddBool("state", false);
	BMessenger msgr(NULL, tooltips);
	msgr.SendMessage(&msg);
}


void
SWindow::WorkspacesChanged(uint32 old_ws, uint32 new_ws)
{
	if(!tooltips) return;
	if(old_ws != new_ws)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, tooltips);
		msgr.SendMessage(&msg);
	}
}


void
SWindow::FrameResized(float new_width, float new_height)
{
	if(!tooltips) return;
	BMessage msg(S_TOOLTIPS_CHANGED);
	msg.AddBool("state", false);
	BMessenger msgr(NULL, tooltips);
	msgr.SendMessage(&msg);
}


void
SWindow::Minimize(bool minimize)
{
	if(tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, tooltips);
		msgr.SendMessage(&msg);
	}
	BWindow::Minimize(minimize);
}


void
SWindow::Zoom(BPoint rec_position, float rec_width, float rec_height)
{
	if(tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, tooltips);
		msgr.SendMessage(&msg);
	}
	BWindow::Zoom(rec_position, rec_width, rec_height);
}


void
SWindow::Hide()
{
	if(tooltips)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("state", false);
		BMessenger msgr(NULL, tooltips);
		msgr.SendMessage(&msg);
	}
	BWindow::Hide();
}


void
SWindow::WindowActivated(bool state)
{
	UpdateIfNeeded();
}


bool
SWindow::SupportNotActivatedTooltips() const
{
	return false;
}


