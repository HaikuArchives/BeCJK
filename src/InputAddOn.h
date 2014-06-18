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

#ifndef __SINPUT_ADD_ON_H__
#define __SINPUT_ADD_ON_H__

#include <OS.h>
#include <BeBuild.h>
#include <SupportDefs.h>
#include <InterfaceDefs.h>
#include <add-ons/input_server/InputServerMethod.h>
#include <Message.h>
#include <Messenger.h>
#include <View.h>
#include <Input.h>
#include <Looper.h>
#include <String.h>
#include <Locker.h>
#include <List.h>
#include <Menu.h>
#include <storage/Path.h>
#include <storage/Directory.h>

#define S_INPUT_LOOPER_NAME		"BeCJK Event Looper"

// export this for the input_method add-on
extern "C" _EXPORT BInputServerMethod* instantiate_input_method();

class SInputAddOn;
class SInputAddOnLooper;
class SInputPanel;
class SInputModule;
class SInputSelector;
class SInputPopupWindow;
class SInputSettings;
class SLoadingModules;

class SInputAddOn : public BInputServerMethod {
public:
		SInputAddOn();
		virtual ~SInputAddOn();

		virtual status_t InitCheck();
		virtual status_t MethodActivated(bool active);
		virtual filter_result Filter(BMessage *message, BList *outList);
		
		bool Lock();
		void Unlock();
		bool IsLocked() const;

		void SetPopupStyle(bool state);

private: 
		friend class SInputAddOnMessenger;
		friend class SInputAddOnLooper;
		friend class SInputPanel;
		friend class SInputPopupWindow;
		friend class SInputSelector;

		void MethodStopped();
		void SwitchLanguage();
		void ShowSelector(BPoint *where);
		void HideSelector();
		void LocaleChanged(BMessage *message);

		void CreateMenu();

		bool GetKey(int32 modifiers, int32 key_code, char *result);

		void AddMessage(BMessage *msg, BList *outList = NULL, bool delete_msg = false);

		BMenu *input_addon_menu;
		SInputAddOnLooper *input_addon_looper;

		BLocker *locker;

		bool activated;
		bool cjk;
		bool is_valid;
		
		uchar *icon_english;
		uchar *icon_chinese;
		uchar *icon_japanese;
		uchar *icon_korean;
		uchar *icon_unknown;

		int32 font_size;
		uint32 lang;

		SInputPanel *panel;
		bool panel_show_state;

		bool popup_style;
		SInputPopupWindow *popup_window;

		SInputSelector *selector;
		SInputModule *module;
		BList modules_list;

		SInputSettings *settings;

		bool corner_all;
		bool punct_cjk;

		typedef struct _input_module_
		{
			image_id addon_image;
			SInputModule *addon_module;
		} _input_module_;

		int32 current_module;

		SLoadingModules *loading_modules;

		void EmptyModules();
		bool SwitchModule(int32 index = -1);

		bool LoadGeneralModule(const char *addons_filename);
		void LoadGeneralModules(BPath *addons_path);
		void LoadAllGeneralModules();

		bool LoadModule(const char *addons_filename);
		void LoadModules(BPath *addons_path);
		void LoadAllModules();

		void Refresh(BList *outList = NULL);

		int32 CountModules();
};


#ifndef COMPILING_INPUT_ADDON
extern const int32 s_input_font_size_array[];
extern int32 s_input_font_size_array_num;
#endif /* COMPILING_INPUT_ADDON */

#endif /* __SINPUT_ADD_ON_H__ */

