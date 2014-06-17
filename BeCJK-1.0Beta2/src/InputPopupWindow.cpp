// BeCJK - Input Server Add-On for CJK Input
// (C) Copyright by Anthony Lee <anthonylee@eLong.com> 2002
//
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//
// Input Popup Window

#include <Input.h>

#include <besavager/Autolock.h>
#include <besavager/Strings.h>

#include "InputAddOn.h"
#include "InputPanel.h"
#include "InputSelector.h"
#include "InputPopupWindow.h"
#include "Settings.h"


// disable some functions
class SInputTextView : public BTextView {
public:
	SInputTextView(BRect frame,
				   const char *name,
				   BRect textRect,
				   uint32 resizeMask,
				   uint32 flags = B_WILL_DRAW | B_PULSE_NEEDED);

	virtual void MakeFocus(bool focusState = true);
	virtual	void MouseDown(BPoint where);
	virtual	void MouseUp(BPoint where);
	virtual	void MouseMoved(BPoint pt, uint32 code, const BMessage *msg);
};

SInputTextView::SInputTextView(BRect frame,
							   const char *name,
							   BRect textRect,
							   uint32 resizeMask,
							   uint32 flags)
	: BTextView(frame, name, textRect, resizeMask, flags)
{
}


void
SInputTextView::MakeFocus(bool focusState)
{
	BView::MakeFocus(focusState);
}


void
SInputTextView::MouseDown(BPoint where)
{
}


void
SInputTextView::MouseUp(BPoint where)
{
}


void
SInputTextView::MouseMoved(BPoint pt, uint32 code, const BMessage *msg)
{
}


SInputPopupWindow::SInputPopupWindow(SInputAddOn *input_addon)
	: BWindow(BRect(0, 0, 400, 14), "BeCJK Input Popup Window", B_BORDERED_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_AVOID_FOCUS, B_ALL_WORKSPACES)
{
	SInputPopupWindow::input_addon = input_addon;

	SetSizeLimits(0, 10000, 0, 10000);

	dragbar = new SDragBar(Bounds(), NULL, S_DRAG_BAR_LEFT, NULL, true);
	AddChild(dragbar);

	text = new SInputTextView(dragbar->ChildBounds(), NULL,
							  dragbar->ChildBounds().OffsetToCopy(B_ORIGIN).InsetByCopy(2, 2), B_FOLLOW_ALL);
	text->SetViewColor(255, 255, 255);
	text->MakeEditable(true);
	text->MakeSelectable(false);
	text->MakeFocus(true);

	AddChild(text);

	RemoveShortcut((uint32)'W', (uint32)B_COMMAND_KEY);
	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);

	SetFontSize(input_addon->font_size);

	Run();
}


SInputPopupWindow::~SInputPopupWindow()
{
}


void
SInputPopupWindow::MessageReceived(BMessage *message)
{
	if(!message) return;
	
	switch(message->what)
	{
		case B_INPUT_METHOD_EVENT:
			{
				if(message->HasBool("savager:popup"))
				{
					int32 opcode;
					if(message->FindInt32("be:opcode", &opcode) == B_OK)
					{
						switch(opcode)
						{
							case B_INPUT_METHOD_STARTED:
								text->SetText("");
								if(IsHidden()) Show();
								Activate(false);
								text->MessageReceived(message);
								break;
							case B_INPUT_METHOD_STOPPED:
								if(!IsHidden()) Hide();
								text->MessageReceived(message);
								break;
							case B_INPUT_METHOD_CHANGED:
								{
									bool confirmed = false;
									message->FindBool("be:confirmed", &confirmed);
									if(confirmed)
									{
										const char *string = NULL;
										message->FindString("be:string", &string);
										if(string)
										{
											if(strlen(string) == 0) string = NULL;
										}

										if(input_addon && string)
										{
											const char *tmp1 = string;
											const char *tmp2 = NULL;

											while(true)
											{
												uint len, i;
												BMessage *msg = new BMessage(B_KEY_DOWN);
												char *tmp_utf8 = NULL;
												tmp2 = __utf8_next_char(tmp1);
												
												if(!*tmp2)
													len = strlen(tmp1);
												else
													len = strlen(tmp1) - strlen(tmp2);
												
												tmp_utf8 = Sstrdup(tmp1, len);
												tmp1 += len;

												if(!tmp_utf8)
												{
													delete msg;
													break;
												}
												
												msg->AddString("bytes", tmp_utf8);
												for(i = 0; i < len; i++)
													msg->AddInt8("byte", tmp_utf8[i]);

												input_addon->EnqueueMessage(msg);

												BMessage *amsg = new BMessage(*msg);
												amsg->what = B_KEY_UP;
												input_addon->EnqueueMessage(amsg);

												delete[] tmp_utf8;
												if(!*tmp2) break;
											}
										}
									}
									else
									{
										const char *string = NULL;
										int32 sel1 = -1, sel2 = -1;
										message->FindString("be:string", &string);
										message->FindInt32("be:selection", 0, &sel1);
										message->FindInt32("be:selection", 1, &sel2);
										if(string && sel1 == sel2 && sel1 > 0 && sel2 > 0)
										{
											if(sel1 == (int32)strlen(string))
											{
												BString bstr;
												bstr << string << "_";
												message->ReplaceString("be:string", bstr);
												message->AddInt32("be:clause_start", 0);
												message->AddInt32("be:clause_end", sel1 + 1);
											}
										}

										text->MessageReceived(message);
									}
								}
								break;
							default:
								text->MessageReceived(message);
						}
					}
				}
				else
					BWindow::MessageReceived(message);
			}
			break;

		default:
			BWindow::MessageReceived(message);
	}
}


void
SInputPopupWindow::SetFontSize(float fsize)
{
	BFont font(be_plain_font);
	font.SetSize(fsize);
	text->SetFontAndColor(&font);

	font_height fheight;
	font.GetHeight(&fheight);
	float h = fheight.ascent + fheight.descent + fheight.leading;

	ResizeTo(Frame().Width(), h + 6);

	text->SetTextRect(text->Bounds().InsetByCopy(2, 2));
}


void
SInputPopupWindow::FrameMoved(BPoint new_position)
{
	if(input_addon)
	{
		SAutolock<SInputAddOn> autolock(input_addon);
		if(input_addon->settings) input_addon->settings->WritePoint("Popup Window Position", new_position);
	}

	BMessage msg(B_INPUT_METHOD_EVENT);
	msg.AddInt32("be:opcode", B_INPUT_METHOD_LOCATION_REQUEST);
	text->MessageReceived(&msg);
}


void
SInputPopupWindow::Show()
{
	BWindow::Show();
}


void
SInputPopupWindow::ScreenChanged(BRect screen_size, color_space depth)
{
	BRect rect = screen_size;
	rect.OffsetTo(B_ORIGIN);
	if(!rect.Contains(Frame()))
	{
		BPoint pt;
		pt.x = rect.right - Frame().Width() - 25;
		pt.y = rect.bottom - Frame().Height() - 25;
		MoveTo(pt);
	}
	
	BWindow::ScreenChanged(screen_size, depth);
}

