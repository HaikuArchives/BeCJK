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
// Input Panel

#include "config.h"
#include "InputAddOn.h"
#include "InputPanel.h"
#include "BeCJKInputModule.h"
#include "Locale.h"
#include "Icons.h"
#include "About.h"
#include "Settings.h"
#include "AllMessages.h"

#include <Button.h>
#include <Roster.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <Screen.h>


#include <besavager/Autolock.h>
#include <besavager/XPM.h>
#include <besavager/Strings.h>
#include <besavager/Separator.h>
#include <besavager/MenuItem.h>


#define BTN_CJK_EN_TOOLTIPS		N_("Switch CJK/English")
#define BTN_CORNER_TOOLTIPS		N_("All/Half Width ASCII Characters")
#define BTN_PUNCT_TOOLTIPS		N_("CJK/English Punctuation")
#define BTN_KEYBOARD_TOOLTIPS	N_("Software Keyboard")
#define BTN_INFO_TOOLTIPS		N_("Information and Settings")
#define BTN_MODULES_TOOLTIPS	N_("Modules")


class SInputPanelMainView : public BView {
public:
	SInputPanelMainView(BRect frame,
						const char *name,
						uint32 resizeMask,
						uint32 flags);

	virtual	void AllAttached();
};


SInputPanelMainView::SInputPanelMainView(BRect frame, const char *name, uint32 resizeMask, uint32 flags)
	: BView(frame, name, resizeMask, flags)
{
}


void
SInputPanelMainView::AllAttached()
{
	BView::AllAttached();

	BWindow *window = Window();

	if(!window) return;
	
	int32 n = CountChildren();

	if(n <= 0) return;

	BRect frame;

	for(int32 i = 0; i < n; i++)
	{
		BView *view = ChildAt(i);
		if(view)
		{
			BRect rect = view->ConvertToParent(view->Bounds());
			
			if(rect.IsValid())
			{
				frame.left = MIN(frame.left, rect.left);
				frame.top = MIN(frame.top, rect.top);
				frame.right = MAX(frame.right, rect.right);
				frame.bottom = MAX(frame.bottom, rect.bottom);
			}
		}
	}

	if(frame.IsValid()) window->ResizeTo(frame.Width() + 5, frame.Height());
}


SInputPanel::SInputPanel(SInputAddOn *addon)
	: SWindow(BRect(0, 0, 200, 24), "BeCJK Input Panel", B_BORDERED_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_AVOID_FOCUS, B_ALL_WORKSPACES)
{
	SXPM image;

	image.SetTo(chb_icon_default_module);
	default_module_icon = image.ConvertToBitmap();

	SetSizeLimits(0, 10000, 0, 10000);

	SInputPanel::addon = addon;

	dragbar = new SDragBar(Bounds(), NULL, S_DRAG_BAR_LEFT, NULL, true);
	AddChild(dragbar);

	main_view = new SInputPanelMainView(dragbar->ChildBounds(), NULL, B_FOLLOW_ALL, 0);
	main_view->SetViewColor(219, 219, 219, 255);

	BRect frame = main_view->Bounds();

	frame.right = frame.left + frame.Height();
	btn_cjk_en = new SBitmapButton(frame, NULL, chb_icon_unknown, chb_icon_english,
								  _(BTN_CJK_EN_TOOLTIPS), new BMessage(S_SWITCH_LANG_MSG));
	btn_cjk_en->SetBehavior(S_TWO_STATE_BUTTON);
	if(addon) btn_cjk_en->SetValue(addon->cjk ? 0 : 1);
	main_view->AddChild(btn_cjk_en);


	frame.OffsetBy(frame.Height() + 1, 0);
	frame.right = frame.left + 4;
	SSeparator *sep1 = new SSeparator(frame.InsetByCopy(0, 2), NULL, S_SEPARATOR_V, 0);
	main_view->AddChild(sep1);


	frame.OffsetBy(5, 0);
	frame.right = frame.left + frame.Height();
	btn_modules = new SBitmapButton(frame, NULL, chb_icon_default_module, NULL,
									_(BTN_MODULES_TOOLTIPS), new BMessage(S_PANEL_MODULES_MSG));
	main_view->AddChild(btn_modules);


	frame.OffsetBy(frame.Height() + 1, 0);
	frame.right = frame.left + 4;
	SSeparator *sep2 = new SSeparator(frame.InsetByCopy(0, 2), NULL, S_SEPARATOR_V, 0);
	main_view->AddChild(sep2);


	frame.OffsetBy(5, 0);
	frame.right = frame.left + frame.Height();
	btn_corner = new SBitmapButton(frame, NULL, chb_icon_corner_half, NULL,
								   _(BTN_CORNER_TOOLTIPS), new BMessage(S_SWITCH_CORNER_MSG));
	btn_corner->SetEnabled(false);
	main_view->AddChild(btn_corner);


	frame.OffsetBy(frame.Height() + 1, 0);
	frame.right = frame.left + frame.Height();
	btn_punct = new SBitmapButton(frame, NULL, chb_icon_punct_en, NULL,
								  _(BTN_PUNCT_TOOLTIPS), new BMessage(S_SWITCH_PUNCT_MSG));
	btn_punct->SetEnabled(false);
	main_view->AddChild(btn_punct);


//	frame.OffsetBy(frame.Height() + 1, 0);
//	frame.right = frame.left + 4;
//	SSeparator *sep3 = new SSeparator(frame.InsetByCopy(0, 2), NULL, S_SEPARATOR_V, 0);
//	main_view->AddChild(sep3);


//	frame.OffsetBy(5, 0);
//	frame.right = frame.left + frame.Height();
//	btn_keyboard = new SBitmapButton(frame, NULL, chb_icon_keyboard, NULL,
//									 _(BTN_KEYBOARD_TOOLTIPS), new BMessage(S_SOFT_KEYBOARD_SWITCH_STATE_MSG));
//	main_view->AddChild(btn_keyboard);


	frame.OffsetBy(frame.Height() + 1, 0);
	frame.right = frame.left + 4;
	SSeparator *sep4 = new SSeparator(frame.InsetByCopy(0, 2), NULL, S_SEPARATOR_V, 0);
	main_view->AddChild(sep4);


	frame.OffsetBy(5, 0);
	frame.right = frame.left + frame.Height();
	btn_info = new SBitmapButton(frame, NULL, chb_icon_info, NULL,
								 _(BTN_INFO_TOOLTIPS), new BMessage(S_PANEL_INFO_MSG));
	main_view->AddChild(btn_info);

	AddChild(main_view);

	RemoveShortcut((uint32)'W', (uint32)B_COMMAND_KEY);
	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);

	Run();
}


SInputPanel::~SInputPanel()
{
	if(default_module_icon) delete default_module_icon;
}


inline BPopUpMenu* __popup_menu_by_steal_items_form_menu(BMenu *menu)
{
	BPopUpMenu *popup = new BPopUpMenu("");

	if(menu)
	{
		if(menu->CountItems() > 0)
		{
			while(true)
			{
				BMenuItem *menu_item = menu->RemoveItem((int32)0);
				if(!menu_item) break;
				
				popup->AddItem(menu_item);
			}
		}
	}
	
	return popup;
}


void
SInputPanel::MessageReceived(BMessage *message)
{
	if(!message) return;
	
	switch(message->what)
	{
		case S_SWITCH_LANG_MSG:
			if(!addon) break;

			addon->Lock();
			if(!message->HasBool("FromInputAddOnMessenger"))
			{
				if(addon->input_addon_looper)
				{
					BMessenger msgr(NULL, (const BLooper*)addon->input_addon_looper);
					msgr.SendMessage(message);
				}
			}
			else
			{
				if(addon->cjk != (btn_cjk_en->Value() == 0))
					btn_cjk_en->SetValue(addon->cjk ? 0 : 1);
			}
			addon->Unlock();
			break;


		case S_LOCALE_CHANGED:
			btn_cjk_en->SetTooltips(_(BTN_CJK_EN_TOOLTIPS));
			btn_corner->SetTooltips(_(BTN_CORNER_TOOLTIPS));
			btn_punct->SetTooltips(_(BTN_PUNCT_TOOLTIPS));
//			btn_keyboard->SetTooltips(_(BTN_KEYBOARD_TOOLTIPS));
			btn_info->SetTooltips(_(BTN_INFO_TOOLTIPS));
			btn_modules->SetTooltips(_(BTN_MODULES_TOOLTIPS));
			break;

		case S_SWITCH_CORNER_MSG:
		case S_SWITCH_PUNCT_MSG:
		case S_SOFT_KEYBOARD_SWITCH_STATE_MSG:
		case S_PANEL_SWITCH_STATE_MSG:
		case S_LANG_MSG:
		case S_POPUP_STYLE_MSG:
		case S_FONT_SIZE_MSG:
		case S_SWITCH_MODULE_MSG:
		case S_MODULE_FROM_MENU_MSG:
			if(!addon) break;

			addon->Lock();

			if(addon->input_addon_looper)
			{
				BMessenger msgr(NULL, (const BLooper*)addon->input_addon_looper);
				msgr.SendMessage(message);
			}

			addon->Unlock();
			break;


		case S_MODULE_CHANGED_MSG:
			{
				if(!addon) break;

				addon->Lock();

				s_input_encoding encoding = (addon->module ? addon->module->Encoding() : ENCODING_UNKNOWN);

				switch(encoding)
				{
					case ENCODING_SIMPLIFIED_CHINESE:
					case LOCALE_TRADITIONAL_CHINESE:
						btn_cjk_en->SetEnableOff(chb_icon_chinese);
						break;

					case ENCODING_JAPANESE:
						btn_cjk_en->SetEnableOff(chb_icon_japanese);
						break;

					case ENCODING_KOREAN:
						btn_cjk_en->SetEnableOff(chb_icon_korean);
						break;
		
					default:
						btn_cjk_en->SetEnableOff(chb_icon_unknown);
				}

				const BBitmap *module_icon = (addon->module ? (addon->module->Icon() ? addon->module->Icon() : default_module_icon) : default_module_icon);
				btn_modules->SetEnableOff(module_icon);
				btn_modules->SetEnableOn(module_icon);

				if(!addon->module)
				{
					if(btn_corner->IsEnabled()) btn_corner->SetEnabled(false);
					if(btn_punct->IsEnabled()) btn_punct->SetEnabled(false);
				}
				else
				{
					if(addon->corner_all)
					{
						btn_corner->SetEnableOff(chb_icon_corner_all);
						btn_corner->SetEnableOn(chb_icon_corner_all);
					}
					else
					{
						btn_corner->SetEnableOff(chb_icon_corner_half);
						btn_corner->SetEnableOn(chb_icon_corner_half);
					}

					if(addon->punct_cjk)
					{
						btn_punct->SetEnableOff(chb_icon_punct_cjk);
						btn_punct->SetEnableOn(chb_icon_punct_cjk);
					}
					else
					{
						btn_punct->SetEnableOff(chb_icon_punct_en);
						btn_punct->SetEnableOn(chb_icon_punct_en);
					}

					if(!btn_corner->IsEnabled()) btn_corner->SetEnabled(true);
					if(!btn_punct->IsEnabled()) btn_punct->SetEnabled(true);
				}

				UpdateIfNeeded();

				addon->Unlock();
			}
			break;


		case S_PANEL_INFO_MSG:
			{
				if(!addon) break;

				BRect btn_rect = btn_info->ConvertToScreen(btn_info->Bounds());
				BPoint where(btn_rect.left, btn_rect.bottom + 1);

				SXPM image;
				BBitmap *bitmap;

				BPopUpMenu *menu = new BPopUpMenu(_("Information and Settings"));
				menu->SetTriggersEnabled(false);

				image.SetTo(chb_icon_about);
				bitmap = image.ConvertToBitmap();
				SMenuItem *menu_item1 = new SMenuItem(bitmap, _("About BeCJK..."), new BMessage(S_PANEL_ABOUT_MSG));
				if(bitmap) delete bitmap;

				image.SetTo(chb_icon_homepage);
				bitmap = image.ConvertToBitmap();
				SMenuItem *menu_item2 = new SMenuItem(bitmap, _("Homepage..."), new BMessage(S_PANEL_HOMEPAGE_MSG));
				if(bitmap) delete bitmap;

				image.SetTo(chb_icon_mail);
				bitmap = image.ConvertToBitmap();
				SMenuItem *menu_item3 = new SMenuItem(bitmap, _("Report Bugs..."), new BMessage(S_PANEL_MAIL_MSG));
				if(bitmap) delete bitmap;

				image.SetTo(chb_icon_help);
				bitmap = image.ConvertToBitmap();
				SMenuItem *menu_item4 = new SMenuItem(bitmap, _("Help..."), new BMessage(S_PANEL_HELP_MSG));
				if(bitmap) delete bitmap;

				image.SetTo(chb_icon_hide);
				bitmap = image.ConvertToBitmap();
				SMenuItem *menu_item5 = new SMenuItem(bitmap, _("Hide the panel"), new BMessage(S_PANEL_SWITCH_STATE_MSG));
				if(bitmap) delete bitmap;

				SMenuItem *sub_menu_lang = NULL, *sub_menu_font_size = NULL, *sub_menu_popup = NULL;

				addon->Lock();

				sub_menu_popup = new SMenuItem((const BBitmap*)NULL, _("Bottom-line Style"), new BMessage(S_POPUP_STYLE_MSG));
				if(addon->popup_style) sub_menu_popup->SetMarked(true);

				if(s_input_font_size_array_num > 0)
				{
					sub_menu_font_size = new SMenuItem((const BBitmap*)NULL, new BMenu(_("Font Size")));
					sub_menu_font_size->Submenu()->SetTriggersEnabled(false);

					for(int32 i = 0; i < s_input_font_size_array_num; i++)
					{
						BMessage *msg = new BMessage(S_FONT_SIZE_MSG);
						msg->AddInt32("size", s_input_font_size_array[i]);
						BString str; str << s_input_font_size_array[i];
						BMenuItem *menu_item = new BMenuItem(str.String(), msg);
						if(addon->font_size == s_input_font_size_array[i]) menu_item->SetMarked(true);
						sub_menu_font_size->Submenu()->AddItem(menu_item);
					}
				}

// shown in the DeskBar, so we disable it
#if 0
				sinput_locale.Lock();
				uint32 lang_count = sinput_locale.CountLocale();
				if(lang_count > 0)
				{
					sub_menu_lang = new SMenuItem(NULL, new BMenu("Interface Language"));
					sub_menu_lang->Submenu()->SetTriggersEnabled(false);
	
					for(uint32 i = 0; i < lang_count; i++)
					{
						BMessage *msg = new BMessage(S_LANG_MSG);
						msg->AddInt32("lang", (int32)i);
						BMenuItem *menu_item = new BMenuItem(sinput_locale.GetLocale(i), msg);
			
						if(addon->lang == i) menu_item->SetMarked(true);
			
						sub_menu_lang->Submenu()->AddItem(menu_item);
					}
				}
				sinput_locale.Unlock();
#endif

				addon->Unlock();

				// Don't show HOMEPAGE link when tracker hadn't been runned for safty
				if(!(be_roster->IsRunning("application/x-vnd.Be-TRAK")))
				{
					menu_item2->SetEnabled(false);
					menu_item3->SetEnabled(false);
					menu_item4->SetEnabled(false);
				}

				menu->AddItem(menu_item1);
				menu->AddItem(menu_item2);
				menu->AddItem(menu_item3);
				menu->AddItem(menu_item4);
				menu->AddSeparatorItem();
				if(sub_menu_popup != NULL)
				{
					menu->AddItem(sub_menu_popup);
				}
				if(sub_menu_font_size != NULL)
				{
					menu->AddItem(sub_menu_font_size);
					menu->AddSeparatorItem();
				}
				else if(sub_menu_popup != NULL)
				{
					menu->AddSeparatorItem();
				}
				if(sub_menu_lang != NULL)
				{
					menu->AddItem(sub_menu_lang);
					menu->AddSeparatorItem();
				}
				menu->AddItem(menu_item5);

				BMenuItem *selected;
				selected = menu->Go(where, false, true);
				
				if(selected && selected->Message())
					PostMessage(selected->Message());

				delete menu;
			}
			break;


		case S_PANEL_MODULES_MSG:
			{
				if(!addon) break;

				BRect btn_rect = btn_modules->ConvertToScreen(btn_modules->Bounds());
				BPoint where(btn_rect.left, btn_rect.bottom + 1);

				BMenu *menu_sc = NULL, *menu_tc = NULL, *menu_jp = NULL, *menu_ko = NULL, *menu_ukn = NULL;

				addon->Lock();

				s_input_encoding encoding = (addon->module ? addon->module->Encoding() : ENCODING_UNKNOWN);

				if(!addon->modules_list.IsEmpty())
				{
					SInputAddOn::_input_module_ *data;
					for(int32 i = 0; (data = (SInputAddOn::_input_module_*)addon->modules_list.ItemAt(i)) != NULL; i++)
					{
						if(data->addon_module)
						{
							BMessage *msg = new BMessage(S_SWITCH_MODULE_MSG);
							msg->AddInt32("module", i);
							const char *module_name = (data->addon_module->Name() ? data->addon_module->Name() : _("Unknown"));
							const BBitmap *module_icon = (data->addon_module->Icon() ? data->addon_module->Icon() : default_module_icon);
							BMenu *module_menu = data->addon_module->Menu();
							SMenuItem *menu_item = NULL;
							if(module_menu)
							{
								menu_item = new SMenuItem(module_icon, module_menu, msg);
								menu_item->SetLabel(module_name);
							}
							else
							{
								menu_item = new SMenuItem(module_icon, module_name, msg);
							}

							BMenu *menu = NULL;							

							switch(data->addon_module->Encoding())
							{
								case ENCODING_SIMPLIFIED_CHINESE:
									if(!menu_sc)
									{
										menu_sc = new BMenu(_("Simplified Chinese"));
										menu_sc->SetTriggersEnabled(false);
									}
									menu = menu_sc;
									break;

								case LOCALE_TRADITIONAL_CHINESE:
									if(!menu_tc)
									{
										menu_tc = new BMenu(_("Traditional Chinese"));
										menu_tc->SetTriggersEnabled(false);
									}
									menu = menu_tc;
									break;

								case ENCODING_JAPANESE:
									if(!menu_jp)
									{
										menu_jp = new BMenu(_("Japanese"));
										menu_jp->SetTriggersEnabled(false);
									}
									menu = menu_jp;
									break;

								case ENCODING_KOREAN:
									if(!menu_ko)
									{
										menu_ko = new BMenu(_("Korean"));
										menu_ko->SetTriggersEnabled(false);
									}
									menu = menu_ko;
									break;
		
								default:
									if(!menu_ukn)
									{
										menu_ukn = new BMenu(_("Unknown"));
										menu_ukn->SetTriggersEnabled(false);
									}
									menu = menu_ukn;
							}

							if(addon->current_module == i) menu_item->SetMarked(true);
							menu->AddItem(menu_item);
						}
					}
				}

				addon->Unlock();

				BPopUpMenu *menu = NULL;
				
				if(menu_sc && !menu_tc && !menu_jp && !menu_ko && !menu_ukn)
				{
					menu = __popup_menu_by_steal_items_form_menu(menu_sc);
					delete menu_sc;
				}
				else if(!menu_sc && menu_tc && !menu_jp && !menu_ko && !menu_ukn)
				{
					menu = __popup_menu_by_steal_items_form_menu(menu_tc);
					delete menu_tc;
				}
				else if(!menu_sc && !menu_tc && menu_jp && !menu_ko && !menu_ukn)
				{
					menu = __popup_menu_by_steal_items_form_menu(menu_jp);
					delete menu_jp;
				}
				else if(!menu_sc && !menu_tc && !menu_jp && menu_ko && !menu_ukn)
				{
					menu = __popup_menu_by_steal_items_form_menu(menu_ko);
					delete menu_ko;
				}
				else if(!menu_sc && !menu_tc && !menu_jp && !menu_ko && menu_ukn)
				{
					menu = __popup_menu_by_steal_items_form_menu(menu_ukn);
					delete menu_ukn;
				}
				else
				{
					menu = new BPopUpMenu(_("Modules"));
					menu->SetTriggersEnabled(false);

					if(menu_sc)
					{
						SMenuItem *menu_item = new SMenuItem(chb_icon_chinese, menu_sc);
						if(encoding == ENCODING_SIMPLIFIED_CHINESE) menu_item->SetMarked(true);
						menu->AddItem(menu_item);
					}

					if(menu_tc)
					{
						SMenuItem *menu_item = new SMenuItem(chb_icon_chinese, menu_tc);
						if(encoding == ENCODING_TRADITIONAL_CHINESE) menu_item->SetMarked(true);
						menu->AddItem(menu_item);
					}

					if(menu_jp)
					{
						SMenuItem *menu_item = new SMenuItem(chb_icon_japanese, menu_jp);
						if(encoding == ENCODING_JAPANESE) menu_item->SetMarked(true);
						menu->AddItem(menu_item);
					}

					if(menu_ko)
					{
						SMenuItem *menu_item = new SMenuItem(chb_icon_korean, menu_ko);
						if(encoding == ENCODING_KOREAN) menu_item->SetMarked(true);
						menu->AddItem(menu_item);
					}

					if(menu_ukn)
					{
						SMenuItem *menu_item = new SMenuItem(chb_icon_unknown, menu_ukn);
						if(encoding == ENCODING_UNKNOWN) menu_item->SetMarked(true);
						menu->AddItem(menu_item);
					}
				}

				BMessage *msg = new BMessage(S_SWITCH_MODULE_MSG);
				msg->AddInt32("module", -1);
				if(menu->CountItems() > 0)
				{
					menu->AddSeparatorItem();
					SMenuItem *menu_item = new SMenuItem((const BBitmap*)NULL, _("Reload all modules"), msg);
					menu->AddItem(menu_item);
				}
				else
				{
					BMenuItem *menu_item = new BMenuItem(_("Reload all modules"), msg);
					menu->AddItem(menu_item);
				}

				BMenuItem *selected;
				selected = menu->Go(where, false, true);

				if(selected && selected->Message()) PostMessage(selected->Message());

				delete menu;
			}
			break;

		
		case S_PANEL_HOMEPAGE_MSG:
			{
				const char *string = APP_HOMEPAGE;
				be_roster->Launch("text/html", 1, (char**)&string);
			}
			break;


		case S_PANEL_HELP_MSG:
			{
				const char *string = _(APP_HELP_FILE);
				be_roster->Launch("text/html", 1, (char**)&string);
			}
			break;


		case S_PANEL_MAIL_MSG:
			{
				const char *strings[5];

				strings[0] = APP_AUTHOR_MAIL;

				strings[1] = "-subject";
				BString bstr1, bstr2;
				bstr1 << "(BUGS!)BeCJK version " << APP_VERSION << " !";
				strings[2] = bstr1.String();

				strings[3] = "-body";
				bstr2 << "Hi, Lee:\n\n"
					  << "************************************************************\n"
					  << "Replace the lines by tell of the issue.\n"
					  << "Please write it in ENGLISH or CHINESE so that I can read it.\n"
					  << "************************************************************\n";
				strings[4] = bstr2.String();

				BEntry entry("/boot/apps/BeMail");
				entry_ref *file = new entry_ref();
				entry.GetRef(file);

				be_roster->Launch(file, 5, (char**)strings);

				delete file;
			}
			break;

		case S_PANEL_ABOUT_MSG:
			{
				BScreen screen(this);
				SInputAboutDialog *dialog = new SInputAboutDialog();

				dialog->Lock();
				BPoint location;
				location.x = (screen.Frame().Width() - dialog->Bounds().Width()) / 2;
				location.y = (screen.Frame().Height() - dialog->Bounds().Height()) / 2;
				dialog->MoveTo(location);
				dialog->Show();
				dialog->Unlock();
			}
			break;

		default:
			SWindow::MessageReceived(message);
	}
}


bool
SInputPanel::SupportNotActivatedTooltips() const
{
	return true;
}


void
SInputPanel::FrameMoved(BPoint new_position)
{
	if(addon)
	{
		SAutolock<SInputAddOn> autolock(addon);
		if(addon->settings) addon->settings->WritePoint("Panel Position", new_position);
	}
}


void
SInputPanel::ScreenChanged(BRect screen_size, color_space depth)
{
	BRect rect = screen_size;
	rect.OffsetTo(B_ORIGIN);
	if(!rect.Contains(Frame()))
	{
		BPoint pt;
		pt.x = rect.right - Frame().Width() - 25;
		pt.y = rect.bottom - Frame().Height() - 65;
		MoveTo(pt);
	}
	
	SWindow::ScreenChanged(screen_size, depth);
}

