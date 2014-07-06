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
// InputServerMethod AddOn

#include <stdlib.h>
#include <syslog.h>

#include "config.h"

#include <BeBuild.h>

#include "Icons.h"
#include "Locale.h"

#define COMPILING_INPUT_ADDON
#include "InputAddOn.h"

#include "InputPanel.h"
#include "BeCJKInputModule.h"
#include "GeneralInputModule.h"
#include "InputSelector.h"
#include "InputPopupWindow.h"
#include "Settings.h"
#include "LoadingModules.h"
#include "AllMessages.h"

#include <Autolock.h>
#include <List.h>
#include <Application.h>
#include <Alert.h>
#include <Roster.h>
#include <MenuItem.h>
#include <image.h>
#include <Screen.h>

#include <besavager/XPM.h>
#include <besavager/Autolock.h>

int32 s_input_font_size_array[] = {10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40};
int32 s_input_font_size_array_num = sizeof(s_input_font_size_array) / sizeof(int32);

class SInputAddOnLooper : public BLooper {
public:
		SInputAddOnLooper(const char *name);

		virtual void MessageReceived(BMessage *message);
		
		void SetInputAddOn(SInputAddOn *addon);

private:
		friend class SInputAddOn;
		SInputAddOn *input_addon;
};


BInputServerMethod* instantiate_input_method()
{
	// export the method funtion
	return(new SInputAddOn());
}


SInputAddOnLooper::SInputAddOnLooper(const char *name)
	: BLooper(name)
{
}


void SInputAddOnLooper::MessageReceived(BMessage *message)
{
	if(!message) return;
	if(!input_addon) return;

	switch(message->what)
	{
		case B_INPUT_METHOD_EVENT:
			{
				int32 opcode;
				if(message->FindInt32("be:opcode", &opcode) == B_OK)
				{
					switch(opcode)
					{
						case B_INPUT_METHOD_STOPPED:
							input_addon->Lock();
							input_addon->MethodStopped();
							input_addon->Unlock();
							break;
						case B_INPUT_METHOD_LOCATION_REQUEST:
							{
								input_addon->Lock();
								
								if(input_addon->module)
								{
									if((input_addon->module->Display() && input_addon->module->AutoHideSelector()) ||
									   !input_addon->module->AutoHideSelector())
									{
										BPoint location(100, 100);
										float height = 16;

										int32 selection_start, selection_end;
										int32 len = 1;

										if(input_addon->module->Display())
										{
											if(input_addon->module->Selection(&selection_start, &selection_end))
												len = (int32)__utf8_strlen(input_addon->module->Display(), selection_end);
											else
												len = (int32)__utf8_strlen(input_addon->module->Display());
										}
										
										if(len < 1) len = 1;

										if(message->FindPoint("be:location_reply", len - 1, &location) == B_OK)
										{
											if(message->FindFloat("be:height_reply", len - 1, &height) == B_OK)
											{
												location += BPoint(0, height);
												input_addon->ShowSelector(&location);
											}
											else
											{
												input_addon->ShowSelector(NULL);
											}
										}
										else
										{
											input_addon->ShowSelector(NULL);
										}
									}
								}

								input_addon->Unlock();
							}
							break;
					}
				}
			}
			break;

		case S_POPUP_STYLE_MSG:
			{
				input_addon->Lock();
				input_addon->SetPopupStyle(!input_addon->popup_style);
				input_addon->Unlock();
			}
			break;

		case S_SWITCH_LANG_MSG:
			{
				input_addon->Lock();
				input_addon->SwitchLanguage();
				input_addon->Unlock();
			}
			break;

		case S_FONT_SIZE_MSG:
			{
				int32 font_size = -1;
				if(message->FindInt32("size", &font_size) != B_OK) break;
				if(font_size <= 0) break;

				input_addon->Lock();
				if(input_addon->font_size != font_size)
				{
					input_addon->font_size = font_size;
					input_addon->settings->WriteInt32("Font Size", input_addon->font_size);

					if(input_addon->popup_window)
					{
						input_addon->MethodStopped();

						// for some reason dead lock, I do this
						while(input_addon->popup_window->LockWithTimeout(1000) != B_OK)
						{
							input_addon->Unlock();
							snooze(1000);
							input_addon->Lock();
						}
						input_addon->popup_window->SetFontSize((float)input_addon->font_size);
						input_addon->popup_window->Unlock();
					}

					// for some reason dead lock, I do this
					while(input_addon->selector->LockWithTimeout(1000) != B_OK)
					{
						input_addon->Unlock();
						snooze(1000);
						input_addon->Lock();
					}
					input_addon->selector->SetFontSize((float)input_addon->font_size);
					input_addon->selector->Unlock();
				}
				input_addon->Unlock();
			}
			break;

		case S_LANG_MSG:
			{
				uint32 lang = 0;
				if(message->FindInt32("lang", (int32*)&lang) != B_OK) break;
				sinput_locale.SetLocale(lang);
			}
			break;
		
		case S_LOCALE_CHANGED:
			{
				input_addon->Lock();
				input_addon->LocaleChanged(message);
				input_addon->Unlock();
			}
			break;

		case S_SWITCH_MODULE_MSG:
			{
				int32 module;
				if(message->FindInt32("module", &module) != B_OK) break;

				input_addon->Lock();
				if(module < 0)
				{
					input_addon->LoadAllModules();
				}
				else if(input_addon->current_module != module)
				{
					if(input_addon->SwitchModule(module))
						input_addon->settings->WriteInt32("Module", input_addon->current_module);
				}
				input_addon->Unlock();
			}
			break;

		case S_MODULE_CHANGED_MSG:
			{
				input_addon->Lock();

				BMessenger msgr1(NULL, input_addon->panel);
				msgr1.SendMessage(message);

				BMessenger msgr2(NULL, input_addon->selector);
				msgr2.SendMessage(message);

				input_addon->Unlock();
			}
			break;

		case S_SWITCH_CORNER_MSG:
			{
				input_addon->Lock();

				input_addon->corner_all = !(input_addon->corner_all);
				input_addon->settings->WriteBool("Full Width", input_addon->corner_all);

				BMessenger msgr1(NULL, input_addon->panel);
				msgr1.SendMessage(S_MODULE_CHANGED_MSG);

				BMessenger msgr2(NULL, input_addon->selector);
				msgr2.SendMessage(S_MODULE_CHANGED_MSG);

				input_addon->Unlock();
			}
			break;
		
		case S_SWITCH_PUNCT_MSG:
			{
				input_addon->Lock();

				input_addon->punct_cjk = !(input_addon->punct_cjk);
				input_addon->settings->WriteBool("CJK Punctuation", input_addon->punct_cjk);

				BMessenger msgr1(NULL, input_addon->panel);
				msgr1.SendMessage(S_MODULE_CHANGED_MSG);

				BMessenger msgr2(NULL, input_addon->selector);
				msgr2.SendMessage(S_MODULE_CHANGED_MSG);

				input_addon->Unlock();
			}
			break;

		case S_MODULE_FROM_MENU_MSG:
			{
				SAutolock<SInputAddOn> autolock(input_addon);

				SInputModule *module = NULL;
				if(message->FindPointer("module", (void**)&module) != B_OK) break;
				
				int32 module_what = 0;
				if(message->FindInt32("module_what", &module_what) != B_OK) break;

				if(module)
				{
					BMessage msg(*message);
					msg.what = module_what;
					module->MessageReceivedFromMenu(&msg);
				}
			}
			break;

		case S_PANEL_SWITCH_STATE_MSG:
			{
				input_addon->Lock();

				input_addon->panel_show_state = !input_addon->panel_show_state;
				input_addon->settings->WriteBool("Panel", input_addon->panel_show_state);

				// for some reason dead lock, I do this
				while(input_addon->panel->LockWithTimeout(1000) != B_OK)
				{
					input_addon->Unlock();
					snooze(1000);
					input_addon->Lock();
				}
				if(input_addon->panel_show_state)
				{
					if(input_addon->panel->IsHidden())
					{
						input_addon->panel->Show();
						input_addon->panel->Activate(false);
					}
				}
				else
				{
					if(!input_addon->panel->IsHidden()) input_addon->panel->Hide();
				}
				input_addon->panel->Unlock();

				input_addon->Unlock();
			}
			break;

		case S_MODULE_CJK_PUNCTUATION_REQUESTED_MSG:
			{
				BMessage reply(B_REPLY);
				reply.AddBool("result", input_addon->punct_cjk);
				message->SendReply(&reply);
			}
			break;

		case S_MODULE_ALL_CORNER_REQUESTED_MSG:
			{
				BMessage reply(B_REPLY);
				reply.AddBool("result", input_addon->corner_all);
				message->SendReply(&reply);
			}
			break;

		default:
			BLooper::MessageReceived(message);
	}
}


void SInputAddOnLooper::SetInputAddOn(SInputAddOn *addon)
{
	Lock();
	input_addon = addon;
	Unlock();
}


SInputAddOn::SInputAddOn()
	: BInputServerMethod("BeCJK", NULL),
	  input_addon_menu(NULL), input_addon_looper(NULL),
	  locker(NULL), activated(false), cjk(true), is_valid(false),
	  font_size(16), lang(0), panel(NULL), panel_show_state(true),
	  popup_style(false), popup_window(NULL),
	  selector(NULL), module(NULL), settings(NULL),
	  corner_all(false), punct_cjk(false), current_module(-1),
	  loading_modules(NULL)
{
	if(is_computer_on() != 1) return;
	if((locker = new BLocker()) == NULL) return;

	be_app->Lock();

	int32 looper_num = be_app->CountLoopers();

	// check whether BeCJK has been run
	if(looper_num > 0)
		for(int32 i = 0; i < looper_num; i++)
		{
			BLooper *looper = be_app->LooperAt(i);
			if(looper)
			{
				BAutolock autolock(looper);
				if(looper->Name())
				{
					if(strcmp(looper->Name(), S_INPUT_LOOPER_NAME) == 0 &&
						strlen(looper->Name()) == strlen(S_INPUT_LOOPER_NAME)) // find it...
					{
						be_app->Unlock();
						return;
					}
				}
			}
		}

	be_app->Unlock();

	settings = new SInputSettings();
	BPath path;
	find_directory(B_USER_SETTINGS_DIRECTORY, &path);
	path.Append("BeCJK");
	BDirectory bdir;
	bdir.CreateDirectory(path.Path(), NULL);
	if(!settings->SetTo("BeCJK/Settings", B_USER_SETTINGS_DIRECTORY, true)) return;

	is_valid = true;

	settings->ReadBool("CJK", &cjk, &cjk);
	settings->ReadInt32("Font Size", &font_size, &font_size);
	settings->ReadInt32("Language", (int32*)&lang, (int32*)&lang);
	settings->ReadBool("Panel", &panel_show_state, &panel_show_state);
	settings->ReadBool("Popup", &popup_style, &popup_style);
	settings->ReadBool("Full Width", &corner_all, &corner_all);
	settings->ReadBool("CJK Punctuation", &punct_cjk, &punct_cjk);

	sinput_locale_init();

	sinput_locale.Lock();
	sinput_locale.SetLocale(lang);
	sinput_locale.Unlock();

	// generate the icon for the deskbar
	SXPM *image = new SXPM();

	image->SetTo(chb_icon_english);
	icon_english = image->ConvertToIcon();

	image->SetTo(chb_icon_chinese);
	icon_chinese = image->ConvertToIcon();

	image->SetTo(chb_icon_japanese);
	icon_japanese = image->ConvertToIcon();

	image->SetTo(chb_icon_korean);
	icon_korean = image->ConvertToIcon();

	image->SetTo(chb_icon_unknown);
	icon_unknown = image->ConvertToIcon();

	delete image;

	input_addon_looper = new SInputAddOnLooper(S_INPUT_LOOPER_NAME);
	input_addon_looper->SetInputAddOn(this);
	input_addon_looper->Run();
	BMessenger msgr(NULL, input_addon_looper);
	sinput_locale.AddMessenger(msgr);

	CreateMenu();

	Lock();

	panel = new SInputPanel(this);
	selector = new SInputSelector(this);

	BPoint pt(0, 0);
	settings->ReadPoint("Panel Position", &pt, &pt);
	BScreen screen(B_MAIN_SCREEN_ID);
	BRect rect = screen.Frame();
	rect.OffsetTo(B_ORIGIN);
	if(!rect.Contains(pt)) pt = BPoint(0, 0);

	// for some reason dead lock, I do this
	while(panel->LockWithTimeout(1000) != B_OK)
	{
		Unlock();
		snooze(1000);
		Lock();
	}
	panel->MoveTo(pt);
	panel->Unlock();

	LoadAllModules();

	Unlock();
}


status_t SInputAddOn::InitCheck()
{
	if(!is_valid) return B_UNSUPPORTED;
	if(modules_list.IsEmpty() || !module) return B_UNSUPPORTED;

	return B_OK;
}


SInputAddOn::~SInputAddOn()
{
	Lock();

	if(input_addon_looper)
	{
		BMessenger msgr(NULL, input_addon_looper);
		sinput_locale.RemoveMessenger(msgr);

		// for some reason dead lock, I do this
		while(input_addon_looper->LockWithTimeout(1000) != B_OK)
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		input_addon_looper->Quit();
		input_addon_looper = NULL;
	}

	if(panel)
	{
		// for some reason dead lock, I do this
		while(panel->LockWithTimeout(1000) != B_OK)
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		panel->Quit();
		panel = NULL;
	}

	if(selector)
	{
		// for some reason dead lock, I do this
		while(selector->LockWithTimeout(1000) != B_OK)
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		selector->Quit();
		selector = NULL;
	}

	if(popup_window)
	{
		// for some reason dead lock, I do this
		while(popup_window->LockWithTimeout(1000) != B_OK)
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		popup_window->Quit();
		popup_window = NULL;
	}

	if(icon_english) delete[] icon_english;
	if(icon_chinese) delete[] icon_chinese;
	if(icon_japanese) delete[] icon_japanese;
	if(icon_korean) delete[] icon_korean;
	if(icon_unknown) delete[] icon_unknown;
	if(settings) delete settings;
	
	icon_english = NULL;
	icon_chinese = NULL;
	icon_japanese = NULL;
	icon_korean = NULL;
	icon_unknown = NULL;
	settings = NULL;
	
	EmptyModules();

	Unlock();

	if(locker) delete locker;
}


bool SInputAddOn::Lock()
{
	return(locker ? locker->Lock() : false);
}


void SInputAddOn::Unlock()
{
	if(locker) locker->Unlock();
}


bool SInputAddOn::IsLocked() const
{
	return(locker ? locker->IsLocked() : false);
}


void
SInputAddOn::CreateMenu()
{
	BMessenger msgr(NULL, input_addon_looper);

	SetMenu(NULL, msgr);

	if(input_addon_menu) delete input_addon_menu;

	input_addon_menu = new BMenu("BeCJK");

	BMenuItem *menu_item1 = new BMenuItem(_("Switch CJK/English"), new BMessage(S_SWITCH_LANG_MSG));
	BMenuItem *menu_item2 = new BMenuItem(_("Show/Hide panel"), new BMessage(S_PANEL_SWITCH_STATE_MSG));
//	BMenuItem *menu_item3 = new BMenuItem(_("Show/Hide software keyboard"), new BMessage(S_SOFT_KEYBOARD_SWITCH_STATE_MSG));

	input_addon_menu->AddItem(menu_item1);
	input_addon_menu->AddItem(menu_item2);
//	input_addon_menu->AddItem(menu_item3);


	sinput_locale.Lock();

	uint32 lang_count = sinput_locale.CountLocale();

	if(lang_count > 0)
	{
		input_addon_menu->AddSeparatorItem();
		BMenuItem *sub_menu = new BMenuItem(new BMenu("Interface Language"));
	
		for(uint32 i = 0; i < lang_count; i++)
		{
			BMessage *msg = new BMessage(S_LANG_MSG);
			msg->AddInt32("lang", (int32)i);
			BMenuItem *menu_item = new BMenuItem(sinput_locale.GetLocale(i), msg);
			
			if(lang == i) menu_item->SetMarked(true);
			
			sub_menu->Submenu()->AddItem(menu_item);
		}

		input_addon_menu->AddItem(sub_menu);
	}

	sinput_locale.Unlock();
}


status_t SInputAddOn::MethodActivated(bool active)
{
	SAutolock<SInputAddOn> autolock(this);

	if(active)
	{
		if(cjk)
		{
			s_input_encoding encoding = (module ? module->Encoding() : ENCODING_UNKNOWN);
			switch(encoding)
			{
				case ENCODING_SIMPLIFIED_CHINESE:
				case ENCODING_TRADITIONAL_CHINESE:
											SetIcon(icon_chinese); break;
				case ENCODING_JAPANESE:		SetIcon(icon_japanese); break;
				case ENCODING_KOREAN:		SetIcon(icon_korean); break;
				default:					SetIcon(icon_unknown); break;
			}
		}
		else
			SetIcon(icon_english);

		BMessenger msgr(NULL, input_addon_looper);
		SetMenu(input_addon_menu, msgr);
		
		if(panel_show_state)
		{
			// for some reason dead lock, I do this
			while(panel->LockWithTimeout(1000) != B_OK)
			{
				Unlock();
				snooze(1000);
				Lock();
			}
			if(panel->IsHidden())
			{
				panel->Show();
				panel->Activate(false);
			}
			panel->Unlock();
		}
	}
	else
	{
		BMessenger msgr(NULL, input_addon_looper);
		
		BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
		msg->AddString("be:string", "");
		msg->AddBool("be:confirmed", true);
		AddMessage(msg, NULL, true);

		msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
		AddMessage(msg, NULL, true);

		SetMenu(NULL, msgr);

		SetIcon(NULL);

		// for some reason dead lock, I do this
		while(panel->LockWithTimeout(1000) != B_OK)
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		if(!panel->IsHidden()) panel->Hide();
		panel->Unlock();

		MethodStopped();
	}

	return(B_OK);
}


inline char* get_key_from_keymap(char *chars, int32 offset)
{
	if(!chars) return NULL;

	int size = chars[offset++];

	switch(size)
	{ 
		case 0:
			break;
		default:
			{
				char *str = new char[size + 1];
				strncpy(str, &(chars[offset]), size);
				str[size] = '\000';
				return str;
			}
	}
	
	return NULL;
}

bool SInputAddOn::GetKey(int32 modifiers, int32 key_code, char *result)
{
	if(!result) return false;
	if(!(key_code >= 0 && key_code < 128)) return false;
	
	key_map *keys = NULL;
	char *chars = NULL;
	char *true_key = NULL;

	get_key_map(&keys, &chars);
	if(keys == NULL || chars == NULL)
	{
		if(keys) delete keys;
		if(chars) delete chars;
		return false;
	}

	if((modifiers & B_SHIFT_KEY) && (modifiers & B_CAPS_LOCK))
		true_key = get_key_from_keymap(chars, keys->caps_shift_map[key_code]);
	else if(modifiers & B_SHIFT_KEY)
		true_key = get_key_from_keymap(chars, keys->shift_map[key_code]);
	else if(modifiers & B_CAPS_LOCK)
		true_key = get_key_from_keymap(chars, keys->caps_map[key_code]);
	else
		true_key = get_key_from_keymap(chars, keys->normal_map[key_code]);

	delete keys;
	delete chars;
	
	if(!true_key) return false;
	if(strlen(true_key) != 1) // not a single ascii key
	{
		delete[] true_key;
		return false;
	}
	
	*result = true_key[0];
	delete[] true_key;
	return true;
}


filter_result
SInputAddOn::Filter(BMessage *message, BList *outList)
{
	SAutolock<SInputAddOn> autolock(this);

	if(!message) return B_SKIP_MESSAGE;

	switch(message->what)
	{
		case B_MOUSE_DOWN: // here to deal with the Mouse Right button clicked
			{
				int32 buttons;
				
				if(!activated) return B_DISPATCH_MESSAGE;

				if(message->FindInt32("buttons", &buttons) != B_OK) break;

				if(buttons != B_SECONDARY_MOUSE_BUTTON) break;

				BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
				msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
				msg->AddString("be:string", "");
				msg->AddBool("be:confirmed", true);
				AddMessage(msg, outList, true);

				msg = new BMessage(B_INPUT_METHOD_EVENT);
				msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
				AddMessage(msg, outList, true);

				MethodStopped();

				return B_DISPATCH_MESSAGE;
			}
			break;

		case B_UNMAPPED_KEY_UP:
			{
				int32 modifiers;

				message->FindInt32("modifiers", &modifiers);

				if((modifiers & B_LEFT_SHIFT_KEY) && (modifiers & B_LEFT_CONTROL_KEY)) return B_SKIP_MESSAGE;

				return B_DISPATCH_MESSAGE;
			}
			break;

		case B_UNMAPPED_KEY_DOWN:
			{
				int32 modifiers;

				message->FindInt32("modifiers", &modifiers);

				// Switch Methods
				if((modifiers & B_LEFT_SHIFT_KEY) && (modifiers & B_LEFT_CONTROL_KEY))
				{
					SwitchModule();
					return B_SKIP_MESSAGE;
				}

				return B_DISPATCH_MESSAGE;
			}
			break;

		case B_KEY_UP:
			{
				// TODO
				return(activated == true ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE);
			}
			break;

		case B_KEY_DOWN:
			{
				int32 key_code;
				int32 modifiers;
				char key;

				message->FindInt32("modifiers", &modifiers);
				// if no found for "key", drop the message
				if(message->FindInt32("key", &key_code) != B_OK)
					return(activated == true ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE);
				
				// if no found for analysing the key_code, drop the message
				if(!GetKey(modifiers, key_code, &key))
					return(activated == true ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE);


				if(key == B_SPACE && (modifiers & B_LEFT_CONTROL_KEY))
				{
					SwitchLanguage();
					return B_SKIP_MESSAGE;
				}


				// Function Keys
				if((modifiers & B_LEFT_SHIFT_KEY) && key == B_FUNCTION_KEY && key_code == B_F1_KEY)
				{
					BMessenger msgr(NULL, input_addon_looper);
					msgr.SendMessage(S_SWITCH_CORNER_MSG);
					return B_SKIP_MESSAGE;
				}
				else if((modifiers & B_LEFT_SHIFT_KEY) && key == B_FUNCTION_KEY && key_code == B_F2_KEY)
				{
					BMessenger msgr(NULL, input_addon_looper);
					msgr.SendMessage(S_SWITCH_PUNCT_MSG);
					return B_SKIP_MESSAGE;
				}
				else if((modifiers & B_LEFT_SHIFT_KEY) && key == B_FUNCTION_KEY && key_code == B_F3_KEY)
				{
					// Switch Panel Show State
					panel_show_state = !panel_show_state;
					settings->WriteBool("Panel", panel_show_state);

					// for some reason dead lock, I do this
					while(panel->LockWithTimeout(1000) != B_OK)
					{
						Unlock();
						snooze(1000);
						Lock();
					}
					if(panel_show_state)
					{
						if(panel->IsHidden())
						{
							panel->Show();
							panel->Activate(false);
						}
					}
					else
					{
						if(!panel->IsHidden()) panel->Hide();
					}
					panel->Unlock();

					return B_SKIP_MESSAGE;
				}
				else if((modifiers & B_LEFT_SHIFT_KEY) && key == B_FUNCTION_KEY && key_code == B_F4_KEY)
				{
					// TODO: Switch KeyCursor State
					return B_DISPATCH_MESSAGE;
				}
				else if(key == B_FUNCTION_KEY ||
						key == B_PRINT_KEY ||
						key == B_SCROLL_KEY ||
						key == B_PAUSE_KEY)
				{
					return B_DISPATCH_MESSAGE;
				}

				if(!cjk) return B_DISPATCH_MESSAGE;

				// only be a normal key
				if(!(modifiers & (B_CONTROL_KEY | B_COMMAND_KEY | B_OPTION_KEY | B_MENU_KEY)) && module)
				{
					if(!module->KeyFilter(key, (modifiers & B_LEFT_SHIFT_KEY) || (modifiers & B_RIGHT_SHIFT_KEY)))
						return(activated == true ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE);

					if(!activated) // if not activate yet, start it now
					{
						if(!module->IsEmpty())
						{
							BMessenger msgr(NULL, input_addon_looper);
							BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
							msg->AddInt32("be:opcode", B_INPUT_METHOD_STARTED);
							msg->AddMessenger("be:reply_to", msgr);
							AddMessage(msg, outList, true);
							activated = true;
						}
						else
						{
							return B_DISPATCH_MESSAGE;
						}
					}

					Refresh(outList);

					// ignore the current message, then send the "msg" to the destination
					return(popup_style ? B_SKIP_MESSAGE : B_DISPATCH_MESSAGE);
				}
			}
			break;
	}
	
	return B_DISPATCH_MESSAGE;
}



void
SInputAddOn::MethodStopped()
{
	if(module) module->Reset();
	activated = false;

	// for some reason dead lock, I do this
	while(selector->LockWithTimeout(1000) != B_OK)
	{
		if(IsLocked())
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		else
		{
			snooze(1000);
		}
	}
	if(!selector->IsHidden()) selector->Hide();
	selector->Unlock();

	if(popup_window)
	{
		// for some reason dead lock, I do this
		while(popup_window->LockWithTimeout(1000) != B_OK)
		{
			if(IsLocked())
			{
				Unlock();
				snooze(1000);
				Lock();
			}
			else
			{
				snooze(1000);
			}
		}
		if(!popup_window->IsHidden()) popup_window->Hide();
		popup_window->Unlock();
	}
}


void
SInputAddOn::LocaleChanged(BMessage *message)
{
	uint32 language = 0;
	if(message->FindInt32("lang", (int32*)&language) != B_OK) return;

	if(lang != language)
	{
		lang = language;
		settings->WriteInt32("Language", lang);
		CreateMenu();

		BMessenger msgr(NULL, input_addon_looper);
		SetMenu(input_addon_menu, msgr);

		if(!modules_list.IsEmpty())
		{
			_input_module_ *data;
			for(int32 i = 0; (data = (_input_module_*)modules_list.ItemAt(i)) != NULL; i++)
			{
				if(!data) continue;
				if((uint32)(data->addon_module->Locale()) != lang) data->addon_module->LocaleChanged((s_input_locale)lang);
			}
		}
	}

	BMessenger msgr1(NULL, panel);
	msgr1.SendMessage(message);

	BMessenger msgr2(NULL, selector);
	msgr2.SendMessage(message);
}


void
SInputAddOn::SwitchLanguage()
{
	cjk = !cjk;
	settings->WriteBool("CJK", cjk);

	if(cjk)
	{
		s_input_encoding encoding = (module ? module->Encoding() : ENCODING_UNKNOWN);
		switch(encoding)
		{
			case ENCODING_SIMPLIFIED_CHINESE:
			case ENCODING_TRADITIONAL_CHINESE:
										SetIcon(icon_chinese); break;
			case ENCODING_JAPANESE:		SetIcon(icon_japanese); break;
			case ENCODING_KOREAN:		SetIcon(icon_korean); break;
			default:					SetIcon(icon_unknown); break;
		}
	}
	else /* turn to english state will close the method event */
	{
		if(activated && module)
		{
			if(module->Display())
			{
				BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
				msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
				msg->AddString("be:string", module->Display() == NULL ? module->Display() : "");
				msg->AddBool("be:confirmed", true);
				AddMessage(msg, NULL, true);
			}

			module->Reset();

			BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
			msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
			AddMessage(msg, NULL, true);
			activated = false;

			// for some reason dead lock, I do this
			while(selector->LockWithTimeout(1000) != B_OK)
			{
				if(IsLocked())
				{
					Unlock();
					snooze(1000);
					Lock();
				}
				else
				{
					snooze(1000);
				}
			}
			if(!selector->IsHidden()) selector->Hide();
			selector->Unlock();
		}
						
		SetIcon(icon_english);
	}

	BMessage msg(S_SWITCH_LANG_MSG);
	BMessenger msgr(NULL, panel);
	msg.AddBool("FromInputAddOnMessenger", true);
	msgr.SendMessage(&msg);
}


void
SInputAddOn::ShowSelector(BPoint *where)
{
	if(!module) return;
	if(!module->SelectItems() && module->AutoHideSelector()) return;

	// for some reason dead lock, I do this
	while(selector->LockWithTimeout(1000) != B_OK)
	{
		if(IsLocked())
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		else
		{
			snooze(1000);
		}
	}


	if(popup_style && where)
	{
		if(popup_window)
		{
			// for some reason dead lock, I do this
			while(popup_window->LockWithTimeout(1000) != B_OK)
			{
				if(IsLocked())
				{
					selector->Unlock();
					Unlock();
					snooze(1000);
					Lock();

					// for some reason dead lock, I do this
					while(selector->LockWithTimeout(1000) != B_OK)
					{
						if(IsLocked())
						{
							Unlock();
							snooze(1000);
							Lock();
						}
						else
						{
							snooze(1000);
						}
					}
				}
				else
				{
					selector->Unlock();
					snooze(1000);

					// for some reason dead lock, I do this
					while(selector->LockWithTimeout(1000) != B_OK)
					{
						if(IsLocked())
						{
							Unlock();
							snooze(1000);
							Lock();
						}
						else
						{
							snooze(1000);
						}
					}
				}
			}
			where->y = popup_window->Frame().bottom + 5;
			popup_window->Unlock();
		}
		else
			where->y += 6;
	}
	else if(where)
	{
		where->y += 10;
	}

	int32 start_item_index = 0;
	int32 select_item_index = 0;

	const SStringArray *array = module->SelectItems(&start_item_index, &select_item_index);
	const char* selkeys = module->SelectKeys();

	if(array)
	{
		int nitem = (selkeys ? (strlen(selkeys) > 0 ? strlen(selkeys) : 10) : 10);

		SStringArray array_tmp;
		SStringArray array_selkeys_tmp;
		for(int32 i = 0; i < MIN(array->CountItems() - start_item_index, nitem); i++)
		{
			const SString *item = array->ItemAt(start_item_index + i);
			if(!item) break;

			SString tmp_selkey;
			if(selkeys) tmp_selkey << selkeys[i];

			array_tmp += item->String();
			array_selkeys_tmp += tmp_selkey;
		}

		selector->SetItems(module->Original(),
						   &array_tmp, &array_selkeys_tmp, select_item_index,
						   (start_item_index >= nitem),
						   (start_item_index < (array->CountItems() / nitem + (array->CountItems() % nitem == 0 ? 0 : 1) - 1) * nitem),
						   where, true);
		selector->ResizeToFit();
		if(where) if(selector->Frame().LeftTop() != *where) selector->MoveTo(*where);
		if(selector->IsHidden()) selector->Show();
		if(selector->IsActive()) selector->Activate(false);
	}
	else if(!module->AutoHideSelector())
	{
		selector->SetItems(module->Original(), NULL, NULL, -1, false, false, where, true);
		selector->ResizeToFit();
		if(where) if(selector->Frame().LeftTop() != *where) selector->MoveTo(*where);
		if(selector->IsHidden()) selector->Show();
		if(selector->IsActive()) selector->Activate(false);
	}

	selector->Unlock();
}


void
SInputAddOn::HideSelector()
{
	// for some reason dead lock, I do this
	while(selector->LockWithTimeout(1000) != B_OK)
	{
		if(IsLocked())
		{
			Unlock();
			snooze(1000);
			Lock();
		}
		else
		{
			snooze(1000);
		}
	}
	if(!selector->IsHidden()) selector->Hide();
	selector->Unlock();
}


void
SInputAddOn::AddMessage(BMessage *msg, BList *outList, bool delete_msg)
{
	if(!msg) return;

	if(popup_style)
	{
		if(!popup_window)
		{
			popup_window = new SInputPopupWindow(this);
			
			BPoint pt(0, 50);
			settings->ReadPoint("Popup Window Position", &pt, &pt);
			BScreen screen(B_MAIN_SCREEN_ID);
			BRect rect = screen.Frame();
			rect.OffsetTo(B_ORIGIN);
			if(!rect.Contains(pt)) pt = BPoint(0, 50);

			// for some reason dead lock, I do this
			while(popup_window->LockWithTimeout(1000) != B_OK)
			{
				if(IsLocked())
				{
					Unlock();
					snooze(1000);
					Lock();
				}
				else
				{
					snooze(1000);
				}
			}
			popup_window->MoveTo(pt);
			popup_window->Unlock();
		}

		msg->AddBool("savager:popup", true);

		BMessenger msgr(NULL, popup_window);
		msgr.SendMessage(msg);

		if(delete_msg) delete msg;
	}
	else
	{
		if(outList) outList->AddItem(msg);
		else EnqueueMessage(msg);
	}
}


void
SInputAddOn::SetPopupStyle(bool state)
{
	if(popup_style != state)
	{
		if(popup_style)
		{
			if(popup_window)
			{
				// for some reason dead lock, I do this
				while(popup_window->LockWithTimeout(1000) != B_OK)
				{
					if(IsLocked())
					{
						Unlock();
						snooze(1000);
						Lock();
					}
					else
					{
						snooze(1000);
					}
				}
				popup_window->Quit();
				popup_window = NULL;
				MethodStopped();
			}
		}
		else
		{
			BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
			msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
			msg->AddString("be:string", "");
			msg->AddBool("be:confirmed", true);
			AddMessage(msg, NULL, true);

			msg = new BMessage(B_INPUT_METHOD_EVENT);
			msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
			AddMessage(msg, NULL, true);

			MethodStopped();
		}

		popup_style = state;
		settings->WriteBool("Popup", popup_style);

		CreateMenu();

		BMessenger msgr(NULL, input_addon_looper);
		SetMenu(input_addon_menu, msgr);
	}
}


void
SInputAddOn::EmptyModules()
{
	current_module = -1;
	module = NULL;

	if(!modules_list.IsEmpty())
	{
		_input_module_ *data;
		for(int32 i = 0; (data = (_input_module_*)modules_list.ItemAt(i)) != NULL; i++)
		{
			if(!data) continue;
			delete data->addon_module;
			if(data->addon_image >= 0) unload_add_on(data->addon_image);
		}
		modules_list.MakeEmpty();
	}
}


bool
SInputAddOn::SwitchModule(int32 index)
{
	if(modules_list.IsEmpty() || current_module < 0) return false;

	int32 count = modules_list.CountItems();
	if(count < 1 || index >= count) return false;

	if(index < 0)
	{
		if(current_module == count - 1)
			index = 0;
		else
			index = current_module + 1;
	}

	_input_module_ *data = (_input_module_*)modules_list.ItemAt(index);
	if(!data) return false;

	if(module)
	{
		BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
		msg->AddString("be:string", "");
		msg->AddBool("be:confirmed", true);
		AddMessage(msg, NULL, true);

		msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
		AddMessage(msg, NULL, true);
		
		module->ModuleActivated(false);
	}

	MethodStopped();

	module = data->addon_module;
	current_module = index;

	BMessage msg(S_MODULE_CHANGED_MSG);

	BMessenger msgr1(NULL, panel);
	msgr1.SendMessage(&msg);

	BMessenger msgr2(NULL, selector);
	msgr2.SendMessage(&msg);

	if(cjk)
	{
		s_input_encoding encoding = (module ? module->Encoding() : ENCODING_UNKNOWN);
		switch(encoding)
		{
			case ENCODING_SIMPLIFIED_CHINESE:
			case ENCODING_TRADITIONAL_CHINESE:
										SetIcon(icon_chinese); break;
			case ENCODING_JAPANESE:		SetIcon(icon_japanese); break;
			case ENCODING_KOREAN:		SetIcon(icon_korean); break;
			default:					SetIcon(icon_unknown); break;
		}
	}
	else
	{
		SetIcon(icon_english);
	}


	if(module) module->ModuleActivated(true);

	return true;
}


bool
SInputAddOn::LoadGeneralModule(const char *addons_filename)
{
	if(!addons_filename) return false;

	if(loading_modules)
	{
		loading_modules->Lock();
		loading_modules->ModuleChanged(addons_filename);
		loading_modules->UpdateIfNeeded();
		loading_modules->Unlock();
	}

	SInputModule *addon_module = new SGeneralInputModule(addons_filename);
	if(!addon_module) return false;

	addon_module->locker = locker;
	addon_module->messenger = new BMessenger(NULL, input_addon_looper);

	if(!addon_module->InitCheck())
	{
		delete addon_module;
		return false;
	}

	if((uint32)(addon_module->Locale()) != lang) addon_module->LocaleChanged((s_input_locale)lang);

	_input_module_ *data = new _input_module_;
	data->addon_image = -1;
	data->addon_module = addon_module;
	if(!modules_list.AddItem((void*)data))
	{
		delete addon_module;
		delete data;
		return false;
	}

	return true;
}


void
SInputAddOn::LoadGeneralModules(BPath *addons_path)
{
	if(!addons_path) return;

	BDirectory dir(addons_path->Path());

	BPath path;
	status_t err = B_NO_ERROR; 
	BEntry entry;

	//load modules in the path
	while(err == B_NO_ERROR)
	{
		err = dir.GetNextEntry((BEntry*)&entry, TRUE);
		if(entry.InitCheck() != B_NO_ERROR) break;
		if(entry.GetPath(&path) != B_NO_ERROR ) continue;

		if(!LoadGeneralModule(path.Path()))
			syslog(LOG_WARNING, "%s:Can't Load Dictionary %s\n", APP_NAME, path.Path());
		
		snooze(1000);
	}
}


void
SInputAddOn::LoadAllGeneralModules()
{
	BPath path;

	if(find_directory( B_SYSTEM_DATA_DIRECTORY, &path) == B_OK)
	{
		path.Append("BeCJK/dicts");
		LoadGeneralModules(&path);
	}
    //add nonpackaged data directory for Haiku with PM
    if(find_directory( B_SYSTEM_NONPACKAGED_DATA_DIRECTORY, &path) == B_OK)
    {
        path.Append("BeCJK/dicts");
        LoadGeneralModules(&path);
    }

	char * env_safemode = getenv("SAFEMODE");
	if(!env_safemode || strcmp(env_safemode, "yes") != 0)
	{
		if(find_directory(B_USER_DATA_DIRECTORY, &path) == B_OK)
		{
			path.Append("BeCJK/dicts");
			LoadGeneralModules(&path);
		}
	}
}


bool
SInputAddOn::LoadModule(const char *addons_filename)
{
	if(!addons_filename) return false;

	if(loading_modules)
	{
		loading_modules->Lock();
		loading_modules->ModuleChanged(addons_filename);
		loading_modules->UpdateIfNeeded();
		loading_modules->Unlock();
	}

	image_id addon_image = -1;
	SInputModule *addon_module = NULL;

	if((addon_image = load_add_on(addons_filename)) >= 0)
	{
		SInputModule* (*instantiate_input_module)();
		if(get_image_symbol(addon_image, "instantiate_input_module",
							B_SYMBOL_TYPE_TEXT, (void**)&instantiate_input_module) == B_OK)
		{
			addon_module = (*instantiate_input_module)();
		}
	}

	if(addon_module)
	{
		addon_module->locker = locker;
		addon_module->messenger = new BMessenger(NULL, input_addon_looper);

		if(!addon_module->InitCheck())
		{
			delete addon_module;
			addon_module = NULL;
		}
	}

	if(!addon_module)
	{
		syslog(LOG_WARNING, "%s: Can't load module(%s)\n", APP_NAME, addons_filename);
		if(addon_image >= 0) unload_add_on(addon_image);
		return false;
	}

	if((uint32)(addon_module->Locale()) != lang) addon_module->LocaleChanged((s_input_locale)lang);

	_input_module_ *data = new _input_module_;
	data->addon_image = addon_image;
	data->addon_module = addon_module;
	if(!modules_list.AddItem((void*)data))
	{
		delete addon_module;
		unload_add_on(addon_image);
		delete data;
		return false;
	}
	
	return true;
}


void
SInputAddOn::LoadModules(BPath *addons_path)
{
	if(!addons_path) return;

	BDirectory dir(addons_path->Path());

	BPath path;
	status_t err = B_NO_ERROR; 
	BEntry entry;

	//load modules in the path
	while(err == B_NO_ERROR)
	{
		err = dir.GetNextEntry((BEntry*)&entry, TRUE);
		if(entry.InitCheck() != B_NO_ERROR) break;
		if(entry.GetPath(&path) != B_NO_ERROR ) continue;

		if(!LoadModule(path.Path()))
			syslog(LOG_WARNING, "%s:Can't Load Addon %s\n", APP_NAME, path.Path());

		snooze(1000);
	}
}


void
SInputAddOn::LoadAllModules()
{
	if(module)
	{
		BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);
		msg->AddString("be:string", "");
		msg->AddBool("be:confirmed", true);
		AddMessage(msg, NULL, true);

		msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
		AddMessage(msg, NULL, true);
	}

	MethodStopped();

	EmptyModules();

	loading_modules = new SLoadingModules(CountModules());
	loading_modules->Lock();
	loading_modules->Show();
	loading_modules->Unlock();

	LoadAllGeneralModules();

	BPath path;

	if(find_directory(B_SYSTEM_ADDONS_DIRECTORY, &path) == B_OK)
	{
		path.Append("BeCJK");
		LoadModules(&path);
	}

	char * env_safemode = getenv("SAFEMODE");
	if(!env_safemode || strcmp(env_safemode, "yes") != 0)
	{
		if(find_directory(B_USER_ADDONS_DIRECTORY, &path) == B_OK)
		{
			path.Append("BeCJK");
			LoadModules(&path);
		}
	}

	if(loading_modules)
	{
		loading_modules->Lock();
		loading_modules->ModuleChanged(NULL);
		loading_modules->Unlock();
		snooze(500000);
		loading_modules->Lock();
		loading_modules->Quit();
		loading_modules = NULL;
	}

	if(!modules_list.IsEmpty())
	{
		settings->ReadInt32("Module", &current_module, &current_module);
		if(current_module >= modules_list.CountItems() || current_module < 0) current_module = 0;
		SwitchModule(current_module);
	}

	if(!module)
	{
		BMessenger msgr(NULL, input_addon_looper);
		msgr.SendMessage(S_MODULE_CHANGED_MSG);
	}
}


void
SInputAddOn::Refresh(BList *outList)
{
	if(!module) return;

	BMessage *msg = new BMessage(B_INPUT_METHOD_EVENT);
	msg->AddInt32("be:opcode", B_INPUT_METHOD_CHANGED);

	if(module->Confirmed() == CONFIRMED_ORIGINAL)
	{
		msg->AddString("be:string", (module->Original() != NULL ? module->Original() : ""));

		msg->AddInt32("be:selection", (module->Original() != NULL ? strlen(module->Original()) : 0));
		msg->AddInt32("be:selection", (module->Original() != NULL ? strlen(module->Original()) : 0));
	}
	else
	{
		msg->AddString("be:string", (module->Display() != NULL ? module->Display() : ""));

		int32 selection_start = -1;
		int32 selection_end = -1;
		if(module->Selection(&selection_start, &selection_end) &&
		   module->Display() &&
		   module->Confirmed() == CONFIRMED_NONE)
		{
			if(selection_start > 0)
			{
				msg->AddInt32("be:clause_start", 0);
				msg->AddInt32("be:clause_end", selection_start);
			}

			msg->AddInt32("be:clause_start", selection_start);
			msg->AddInt32("be:clause_end", selection_end);
			msg->AddInt32("be:selection", selection_start);
			msg->AddInt32("be:selection", selection_end);

			if(selection_end < (int32)strlen(module->Display()))
			{
				msg->AddInt32("be:clause_start", selection_end);
				msg->AddInt32("be:clause_end", strlen(module->Display()));
			}
		}
		else
		{
			if(module->Confirmed() == CONFIRMED_NONE)
			{
				msg->AddInt32("be:clause_start", 0);
				msg->AddInt32("be:clause_end", (module->Display() != NULL ? strlen(module->Display()) : 0));
			}

			msg->AddInt32("be:selection", (module->Display() != NULL ? strlen(module->Display()) : 0));
			msg->AddInt32("be:selection", (module->Display() != NULL ? strlen(module->Display()) : 0));
		}
	}

	msg->AddBool("be:confirmed", (module->Confirmed() != CONFIRMED_NONE));
	AddMessage(msg, outList, true);

	if(module->Confirmed() || module->IsEmpty())
	{
		module->Reset();

		msg = new BMessage(B_INPUT_METHOD_EVENT);
		msg->AddInt32("be:opcode", B_INPUT_METHOD_STOPPED);
		AddMessage(msg, outList, true);

		HideSelector();

		activated = false;
	}
	else
	{
		if(!module->SelectItems() && module->AutoHideSelector())
		{
			HideSelector();
		}
		else if(module->Display())
		{
			msg = new BMessage(B_INPUT_METHOD_EVENT);
			msg->AddInt32("be:opcode", B_INPUT_METHOD_LOCATION_REQUEST);
			AddMessage(msg, outList, true);
		}
		else if(!module->AutoHideSelector())
		{
			ShowSelector(NULL);
		}
	}
}


int32
SInputAddOn::CountModules()
{
	int32 count = 0;

	BPath path;

	if(find_directory(B_SYSTEM_DATA_DIRECTORY, &path) == B_OK)
	{
		path.Append("BeCJK/dicts");
		BDirectory dir(path.Path());

		status_t err = B_NO_ERROR; 
		BEntry entry;

		while(err == B_NO_ERROR)
		{
			err = dir.GetNextEntry((BEntry*)&entry, TRUE);
			if(entry.InitCheck() != B_NO_ERROR) break;
			if(entry.GetPath(&path) != B_NO_ERROR ) continue;
			count++;
		}
	}
    // add non-packaged directory for Haiku with PM
    if(find_directory(B_SYSTEM_NONPACKAGED_DATA_DIRECTORY, &path) == B_OK)
    {
        path.Append("BeCJK/dicts");
        BDirectory dir(path.Path());

        status_t err = B_NO_ERROR;
        BEntry entry;

        while(err == B_NO_ERROR)
        {
            err = dir.GetNextEntry((BEntry*)&entry, TRUE);
            if(entry.InitCheck() != B_NO_ERROR) break;
            if(entry.GetPath(&path) != B_NO_ERROR ) continue;
            count++;
        }
    }

	if(find_directory(B_SYSTEM_ADDONS_DIRECTORY, &path) == B_OK)
	{
		path.Append("BeCJK");
		BDirectory dir(path.Path());

		status_t err = B_NO_ERROR; 
		BEntry entry;

		while(err == B_NO_ERROR)
		{
			err = dir.GetNextEntry((BEntry*)&entry, TRUE);
			if(entry.InitCheck() != B_NO_ERROR) break;
			if(entry.GetPath(&path) != B_NO_ERROR ) continue;
			count++;
		}
	}

	char * env_safemode = getenv("SAFEMODE");
	if(!env_safemode || strcmp(env_safemode, "yes") != 0)
	{
		if(find_directory(B_USER_DATA_DIRECTORY, &path) == B_OK)
		{
			path.Append("BeCJK/dicts");
			BDirectory dir(path.Path());

			status_t err = B_NO_ERROR; 
			BEntry entry;

			while(err == B_NO_ERROR)
			{
				err = dir.GetNextEntry((BEntry*)&entry, TRUE);
				if(entry.InitCheck() != B_NO_ERROR) break;
				if(entry.GetPath(&path) != B_NO_ERROR ) continue;
				count++;
			}
		}

		if(find_directory(B_USER_ADDONS_DIRECTORY, &path) == B_OK)
		{
			path.Append("BeCJK");
			BDirectory dir(path.Path());

			status_t err = B_NO_ERROR; 
			BEntry entry;

			while(err == B_NO_ERROR)
			{
				err = dir.GetNextEntry((BEntry*)&entry, TRUE);
				if(entry.InitCheck() != B_NO_ERROR) break;
				if(entry.GetPath(&path) != B_NO_ERROR ) continue;
				count++;
			}
		}
	}

	return count;
}
