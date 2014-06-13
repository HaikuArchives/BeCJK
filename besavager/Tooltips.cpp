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

#include <string.h>
#include <sys/utsname.h>

#include "Tooltips.h"
#include "StringArray.h"
#include "Window.h"

#include <BeBuild.h>
#include <StringView.h>
#include <Message.h>
#include <Messenger.h>
#include <Bitmap.h>
#include <Screen.h>

class STooltipsWindowView;

class STooltipsWindow : public BWindow {
public:
	STooltipsWindow(STooltips *manager);
	virtual ~STooltipsWindow();

	void ResizeToFitAndShowIt();

	virtual void MessageReceived(BMessage *message);
	virtual bool QuitRequested();

private:
	friend class STooltipsWindowView;

	STooltipsWindowView *view;

	char *tooltips_text;
	BRect tooltips_frame;
	BPoint tooltips_location;
	int32 tooltips_style;

	uint32 tooltips_workspaces;
	bool state;
	bigtime_t start_time;

	STooltips *manager;
	SWindow *orig_window;

	bool is_dano;
};


// do the "Pulse()"
// show the window within 0.4 second
// destroy the window after 5 second
class STooltipsWindowView : public BView {
public:
	STooltipsWindowView(BRect frame);
	virtual ~STooltipsWindowView();

	virtual void Draw(BRect updateRect);

	virtual void Pulse();

	virtual	void GetPreferredSize(float *width, float *height);

	virtual void MouseDown(BPoint where);
};


STooltipsWindowView::STooltipsWindowView(BRect frame)
	: BView(frame, NULL, B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED)
{
}


void
STooltipsWindowView::Pulse()
{
	STooltipsWindow *window = dynamic_cast<STooltipsWindow*>(Window());
	
	if(!window) return;

	if(window->tooltips_text)
	{
		if(window->state == true)
		{
			bigtime_t cur_time = system_time();

			if(cur_time < window->start_time)
				window->start_time = cur_time;
			else
			{
				if(cur_time - window->start_time > 400000)
				{
					BMessage msg(S_TOOLTIPS_CHANGED);
					msg.AddBool("show", true);

					BMessenger msgr(NULL, window);
					msgr.SendMessage(&msg);
				}
			}
		}
		else if(window->state == false && window->start_time != 0)
		{
			bigtime_t cur_time = system_time();

			if(cur_time - window->start_time > 5000000)
			{
				window->PostMessage(B_QUIT_REQUESTED);
			}
		}
	}
}


void
STooltipsWindowView::MouseDown(BPoint where)
{
	Window()->PostMessage(B_QUIT_REQUESTED);
}


STooltipsWindowView::~STooltipsWindowView()
{
}


void
STooltipsWindowView::GetPreferredSize(float *width, float *height)
{
	if(!width && !height) return;

	STooltipsWindow *window = dynamic_cast<STooltipsWindow*>(Window());
	if(!window) return;

	if(width) *width = 0;
	if(height) *height = 0;

	if(!window->tooltips_text) return;

	float string_width = 0, string_height = 0;

	SString string(window->tooltips_text);
	SStringArray *array = string.Split('\n');
	int32 lines = 0;

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;
			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
				string_width = MAX(string_width, StringWidth(data->String()));
		}

		lines = array->CountItems();

		delete array;
	}

	if(lines > 0)
	{
		font_height fheight;
		GetFontHeight(&fheight);
		float true_height = fheight.ascent + fheight.descent + fheight.leading;
		string_height = true_height * (float)lines + (float)(lines - 1) * (true_height / 7.);
	}

	if(width) *width = string_width + 6;
	if(height) *height = string_height + 6;
}


void
STooltipsWindowView::Draw(BRect updateRect)
{
	STooltipsWindow *window = dynamic_cast<STooltipsWindow*>(Window());
	if(!window) return;

	if(!window->tooltips_text) return;

	SetPenSize(1);

	BRect frame = Bounds();

	SetDrawingMode(B_OP_COPY);
	if(!window->is_dano)
	{
		SetHighColor(255, 255, 0, 255);
		SetLowColor(255, 255, 0, 255);
	}
	else
	{
#ifdef B_BEOS_VERSION_DANO
		SetHighColor(ui_color(B_TOOLTIP_BACKGROUND_COLOR));
		SetLowColor(ui_color(B_TOOLTIP_BACKGROUND_COLOR));
#else
		SetHighColor(255, 255, 0, 255);
		SetLowColor(255, 255, 0, 255);
#endif
	}
	FillRect(frame);

	if(!window->is_dano)
	{
		// set the text color
		SetHighColor(0, 0, 0, 255);
	}
	else
	{
#ifdef B_BEOS_VERSION_DANO
		SetHighColor(ui_color(B_TOOLTIP_TEXT_COLOR));
#else
		SetHighColor(0, 0, 0, 255);
#endif
	}

	frame.InsetBy(3, 3);

	SString string(window->tooltips_text);
	SStringArray *array = string.Split('\n');

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;

			font_height fheight;
			GetFontHeight(&fheight);
			float true_height = fheight.ascent + fheight.descent + fheight.leading;

			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
			{
				float x = frame.left;
				float y = frame.top + true_height * (float)(i + 1) + (float)i * (true_height / 7.) - fheight.descent;

				DrawString(data->String(), BPoint(x, y));
			}
		}

		delete array;
	}
}


STooltipsWindow::STooltipsWindow(STooltips *manager)
	: BWindow(BRect(-100, -100, -10, -10), "Savager Tooltips Window", B_TITLED_WINDOW, B_NO_WORKSPACE_ACTIVATION | B_AVOID_FOCUS)
{
	SetSizeLimits(0, 10000, 0, 10000);
	tooltips_text = NULL;
	state = false;
	start_time = 0;
	tooltips_workspaces = B_ALL_WORKSPACES;
	tooltips_style = S_TOOLTIPS_FOLLOW_NONE;
	tooltips_location = BPoint(0, 0);

	struct utsname the_os_name;
	uname(&the_os_name);

	if(strcasecmp(the_os_name.release, "5.1") == 0)
		is_dano = true;
	else
		is_dano = false;

	STooltipsWindow::manager = manager;
	if(manager)
		orig_window = manager->orig_window;
	else
		orig_window = NULL;

	view = new STooltipsWindowView(Bounds());
	view->SetFont(be_plain_font);
	AddChild(view);

	SetLook(B_BORDERED_WINDOW_LOOK);
	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);

	SetPulseRate(20000);

	RemoveShortcut((uint32)'W', (uint32)B_COMMAND_KEY);
	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	Run();
}


STooltipsWindow::~STooltipsWindow()
{
}


bool
STooltipsWindow::QuitRequested()
{
	if(manager)
	{
		BMessage msg(S_TOOLTIPS_CHANGED);
		msg.AddBool("closed", true);
		msg.AddPointer("window", (void*)this);
		BMessenger msgr(NULL, manager);
		msgr.SendMessage(&msg);
	}

	return true;
}


void
STooltipsWindow::ResizeToFitAndShowIt()
{
	if(!IsHidden()) Hide();

	float width = 0, height = 0;
	view->GetPreferredSize(&width, &height);

	SetWorkspaces(tooltips_workspaces);
	ResizeTo(width, height);

	BScreen screen(this);

	// Get Mouse Location
	BPoint location(-1000, -1000);
	uint32 buttons;
	view->GetMouse(&location, &buttons, false);
	view->ConvertToScreen(&location);
	if(location.x == -1000 && location.y == -1000) location = tooltips_frame.LeftTop();

	switch(tooltips_style)
	{
		case S_TOOLTIPS_FOLLOW_MOUSE_X:
			location.x -= width / 2;
			location.y = tooltips_location.y;
			if(location.x < 0) location.x = 0;
			else if(location.x > screen.Frame().Width() - width) location.x = screen.Frame().Width() - width;
			break;

		case S_TOOLTIPS_FOLLOW_MOUSE_Y:
			location.x = tooltips_location.x;
			if(location.y < 0) location.y = 0;
			else if(location.y > screen.Frame().Height() - height) location.y = screen.Frame().Height() - height;
			break;

		case S_TOOLTIPS_FOLLOW_MOUSE:
			location.x += 5;
			location.y += 5;
			if(location.x < 0) location.x = 0;
			else if(location.x > screen.Frame().Width() - width) location.x = screen.Frame().Width() - width;
			if(location.y < 0) location.y = 0;
			else if(location.y > screen.Frame().Height() - height) location.y = screen.Frame().Height() - height;
			break;

		case S_TOOLTIPS_FOLLOW_FRAME_CENTER:
			location.x = tooltips_frame.left + tooltips_frame.Width() / 2 - width / 2;
			location.y = tooltips_frame.top + tooltips_frame.Height() / 2 - height / 2;
			break;

		default:
			location.x = tooltips_location.x - width / 2;
			location.y = tooltips_location.y;
	}

	MoveTo(-1000, -1000);

	Show();
	Activate(false);

	MoveTo(location);

	if(orig_window)
	{
		orig_window->Lock();
		orig_window->SendBehind(this);
		orig_window->Unlock();
	}
}


void
STooltipsWindow::MessageReceived(BMessage *message)
{
	if(!message) return;

	switch(message->what)
	{
		case S_TOOLTIPS_CHANGED:
			{
				bool the_state = false;
				bool show_state = false;

				if(message->FindBool("show", &show_state) == B_OK)
				{
					if(show_state && start_time != 0 && state)
					{
						if(tooltips_text) ResizeToFitAndShowIt();

						state = false;
						break;
					}
				}

				if(message->FindBool("state", &the_state) != B_OK) break;

				if(the_state)
				{
					uint32 workspace = B_ALL_WORKSPACES;
					const char *string = NULL;
					BPoint where(-1000, -1000);
					BRect frame;
					int32 style;
					
					if(message->FindString("tooltips", &string) != B_OK) break;
					if(!string) break;

					if(message->FindRect("frame", &frame) != B_OK) break;
					if(!frame.IsValid()) break;

					if(message->FindInt32("workspace", (int32*)&workspace) != B_OK) break;

					if(message->FindInt32("style", &style) != B_OK)
						style = S_TOOLTIPS_FOLLOW_NONE;

					if(message->FindPoint("where", &where) != B_OK)
					{
						if(style == S_TOOLTIPS_FOLLOW_NONE ||
						   style == S_TOOLTIPS_FOLLOW_MOUSE_X ||
						   style == S_TOOLTIPS_FOLLOW_MOUSE_Y) break;
					}

					if(tooltips_text) delete[] tooltips_text;
					tooltips_text = Sstrdup(string);

					tooltips_workspaces = workspace;
					tooltips_location = where;
					tooltips_frame = frame;
					tooltips_style = style;

					state = true;
					start_time = system_time();
					MoveTo(-1000, -1000);
					if(IsHidden()) Show();
				}
				else
				{
					start_time = 0;
					state = false;

					if(tooltips_text)
					{
						delete[] tooltips_text;
						tooltips_text = NULL;
					}

					if(!IsHidden()) Hide();

					Quit();
				}
			}
			break;
		
		default:
			BWindow::MessageReceived(message);
	}
}


STooltips::STooltips(SWindow *win)
	: BLooper("Savager Tooltips Manager Looper"), tooltips_window(NULL)
{
	orig_window = win;
	Run();
}


STooltips::~STooltips()
{
	if(tooltips_window)
	{
		tooltips_window->Lock();
		tooltips_window->Quit();
		tooltips_window = NULL;
	}
}


void
STooltips::MessageReceived(BMessage *message)
{
	if(!message) return;

	switch(message->what)
	{
		case S_TOOLTIPS_CHANGED:
			{
				if(message->HasBool("closed"))
				{
					void *w = NULL;
					message->FindPointer("window", &w);
					if(w == (void*)tooltips_window) tooltips_window = NULL;
					break;
				}

				bool the_state = false;

				if(message->FindBool("state", &the_state) != B_OK) break;
				
				if(tooltips_window)
				{
					tooltips_window->Lock();
					tooltips_window->Quit();
					tooltips_window = NULL;
				}

				if(the_state)
				{
					tooltips_window = new STooltipsWindow(this);

					BMessenger msgr(NULL, tooltips_window);
					msgr.SendMessage(message);
				}
			}
			break;
		
		default:
			BLooper::MessageReceived(message);
	}
}


BRect
STooltips::Measure(const char* tooltips)
{
	BFont font(be_plain_font);

	float string_width = 0, string_height = 0;

	SString string(tooltips);
	SStringArray *array = string.Split('\n');
	int32 lines = 0;

	if(array)
	{
		if(!array->IsEmpty())
		{
			const SString *data = NULL;
			for(int32 i = 0; (data = array->ItemAt(i)) != NULL; i++)
				string_width = MAX(string_width, font.StringWidth(data->String()));
		}

		lines = array->CountItems();

		delete array;
	}

	if(lines > 0)
	{
		font_height fheight;
		font.GetHeight(&fheight);
		float true_height = fheight.ascent + fheight.descent + fheight.leading;
		string_height = true_height * (float)lines + (float)(lines - 1) * (true_height / 7.);
	}

	return BRect(0, 0, string_width + 9, string_height + 9);
}

