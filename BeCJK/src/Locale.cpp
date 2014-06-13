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
// Locale Functions for Menu,Message...

#include <SupportDefs.h>
#include <besavager/Locale.h>

#include "locale/English.cpp"
#include "locale/Simplified Chinese.cpp"
#include "locale/Traditional Chinese.cpp"
#include "locale/Janpanese.cpp"
#include "locale/Korean.cpp"

static const s_languages SINPUT_LANGUAGES[] = {
	{"English", cb_lang_english, cb_lang_english_amount},
	{"Simplified Chinese", cb_lang_chinese_sc, cb_lang_chinese_sc_amount},
	{"Traditional Chinese", cb_lang_chinese_tc, cb_lang_chinese_tc_amount},
	{"Janpanese", cb_lang_janpanese, cb_lang_janpanese_amount},
	{"Korean", cb_lang_korean, cb_lang_korean_amount},
};

static const int32 SINPUT_LANGUAGES_AMOUNT = sizeof(SINPUT_LANGUAGES) / sizeof(SINPUT_LANGUAGES[0]);

SLocale sinput_locale;

void sinput_locale_init()
{
	sinput_locale.Lock();
	if(sinput_locale.CountLocale() == 0)
	{
		for(int32 i = 0; i < SINPUT_LANGUAGES_AMOUNT; i++)
			sinput_locale.InsertLocale(SINPUT_LANGUAGES[i]);
	}
	sinput_locale.Unlock();
}
