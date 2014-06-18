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
// All Messages Define

#ifndef __SINPUT_ALL_MESSAGES_H__
#define __SINPUT_ALL_MESSAGES_H__

#define S_MODULE_CHANGED_MSG						's001'
#define S_MODULE_CJK_PUNCTUATION_REQUESTED_MSG		's002'
#define S_MODULE_ALL_CORNER_REQUESTED_MSG			's003'


#define S_SWITCH_LANG_MSG							's004'
#define S_PANEL_SWITCH_STATE_MSG					's005'
#define S_SOFT_KEYBOARD_SWITCH_STATE_MSG			's006'
#define S_LANG_MSG									's007'
#define S_SWITCH_CORNER_MSG							's008'
#define S_SWITCH_PUNCT_MSG							's009'
#define S_POPUP_STYLE_MSG							's010'
#define S_FONT_SIZE_MSG								's011'
#define S_SWITCH_MODULE_MSG							's012'


#define S_PANEL_INFO_MSG							's013'
#define S_PANEL_ABOUT_MSG							's014'
#define S_PANEL_HOMEPAGE_MSG						's015'
#define S_PANEL_MAIL_MSG							's016'
#define S_PANEL_HELP_MSG							's017'
#define S_PANEL_MODULES_MSG							's018'


/*
 * Used by SInputModule::SetMenu(........)
 * Data:
 *       Pointer	--	"module"		--	SInputModule*
 *       Int32		--	"module_what"	--	message->what
 */
#define S_MODULE_FROM_MENU_MSG						's019'


#define S_INPUT_SELECTOR_SELECTED_MSG				's020'
#define S_INPUT_SELECTOR_CHANGED_MSG				's021'
#define S_INPUT_SELECTOR_PAGE_UP_MSG				's022'
#define S_INPUT_SELECTOR_PAGE_DOWN_MSG				's023'


#define S_GENERAL_INPUT_MODULE_ABOUT_MSG			's024'
#define S_GENERAL_INPUT_MODULE_WORDS_FIRST_MSG		's025'
#define S_GENERAL_INPUT_MODULE_AUTO_SELECT_MSG		's026'
#define S_GENERAL_INPUT_MODULE_AUTO_FIX_UP_MSG		's027'
#define S_GENERAL_INPUT_MODULE_OUTPUT_ORIGINAL_MSG	's028'
#define S_GENERAL_INPUT_MODULE_FREQ_MSG				's029'
#define S_GENERAL_INPUT_MODULE_HELP_MSG				's030'


#endif /* __SINPUT_ALL_MESSAGES_H__ */
