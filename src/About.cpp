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
// About Dialog

#include "config.h"
#include "About.h"
#include "Icons.h"
#include "Locale.h"
#include "InputAddOn.h"

#include <View.h>
#include <Bitmap.h>

#include <besavager/StringArray.h>
#include <besavager/XPM.h>
#include <besavager/StringView.h>
#include <StringView.h>

class SInputAboutDialogView : public BView {
public:
	SInputAboutDialogView(BRect frame);
	virtual ~SInputAboutDialogView();

	virtual void Draw(BRect updateRect);
	virtual void MouseDown(BPoint where);

private:
	BBitmap *pic;

	SStringView *title;

	SStringView *version;
	SStringView *copyright;
	SStringView *license;

	SStringView *introducing;
};


SInputAboutDialogView::SInputAboutDialogView(BRect frame)
	: BView(frame, NULL, B_FOLLOW_NONE, B_WILL_DRAW), pic(NULL), title(NULL), version(NULL), copyright(NULL), introducing(NULL)
{
	SetViewColor(255, 255, 255, 255);
	SetHighColor(0, 0, 0, 255);

	SXPM image(chb_pixmap_about_picture);
	pic = image.ConvertToBitmap();

	BPoint pt(0, 0);

	s_string_view_color title_colors[2];
	s_rgb_color_setto(&(title_colors[0].color), 0, 51, 203, 255);
	title_colors[0].start_offset = 0;
	title_colors[0].end_offset = 1;
	s_rgb_color_setto(&(title_colors[1].color), 203, 0, 51, 255);
	title_colors[1].start_offset = 2;
	title_colors[1].end_offset = 4;
	title = new SStringView(BRect(0, 0, 10, 10), NULL, "BeCJK", title_colors, 2);
	title->SetFont(be_plain_font);
	title->SetFontSize(18);
	AddChild(title);
	title->ResizeToPreferred();
	title->MoveTo(0, pt.y);
	pt.x = MAX(pt.x, title->Bounds().Width());
	pt.y += title->Bounds().Height() + 10;

	BFont font(be_plain_font);
	font.SetSize(12);
	font.SetShear(100.0);

	BString bstr;
	bstr << _("Version") << " " << APP_VERSION;
	version = new SStringView(BRect(0, 0, 10, 10), NULL, bstr.String());
	version->SetFont(&font);
	AddChild(version);
	version->ResizeToPreferred();
	version->MoveTo(0, pt.y);
	pt.x = MAX(pt.x, version->Bounds().Width());
	pt.y += version->Bounds().Height() + 2;

	bstr = "";
	bstr << _("Copyright (C)") << " " << APP_COPYRIGHT << " Anthony Lee";
	copyright = new SStringView(BRect(0, 0, 10, 10), NULL, bstr.String());
	copyright->SetFont(&font);
	AddChild(copyright);
	copyright->ResizeToPreferred();
	copyright->MoveTo(0, pt.y);
	pt.x = MAX(pt.x, copyright->Bounds().Width());
	pt.y += copyright->Bounds().Height() + 2;

	bstr = "";
	bstr << _("License:") << "\n" << _("AddOn") << " - GNU GENERAL PUBLIC LICENSE Version 2\n" << _("Library") << " - GNU LIBRARY GENERAL PUBLIC LICENSE Version 2";
	license = new SStringView(BRect(0, 0, 10, 10), NULL, bstr.String());
	license->SetFont(&font);
	AddChild(license);
	license->ResizeToPreferred();
	license->MoveTo(0, pt.y);
	pt.x = MAX(pt.x, license->Bounds().Width());
	pt.y += license->Bounds().Height() + 30;

	introducing = new SStringView(BRect(0, 0, 10, 10), NULL, _(APP_INTRODUCTION));
	introducing->SetFont(be_bold_font);
	introducing->SetFontSize(12);
	AddChild(introducing);
	introducing->ResizeToPreferred();
	introducing->MoveTo(0, pt.y);
	pt.x = MAX(pt.x, introducing->Bounds().Width());
	pt.y += introducing->Bounds().Height();

	if(pic)
	{
		pt.x += 25 + pic->Bounds().Width();
		pt.y = MAX(pt.y, pic->Bounds().Height());
	}

	ResizeTo(pt.x, pt.y);
}


SInputAboutDialogView::~SInputAboutDialogView()
{
	if(pic) delete pic;
}


void
SInputAboutDialogView::MouseDown(BPoint where)
{
	Window()->Lock();
	Window()->PostMessage(B_QUIT_REQUESTED);
	Window()->Unlock();
}


void
SInputAboutDialogView::Draw(BRect updateRect)
{
	if(pic)
	{
		SetDrawingMode(B_OP_OVER);
		BPoint loc = Bounds().RightTop();
		loc.x -= pic->Bounds().Width();
		DrawBitmapAsync(pic, loc);
	}

	DrawAfterChildren(updateRect);

	Sync();
}


SInputAboutDialog::SInputAboutDialog()
	: BWindow(BRect(-1000, -1000, -10, -10), NULL, B_TITLED_WINDOW, 0, B_ALL_WORKSPACES)
{
	SetSizeLimits(0, 10000, 0, 10000);
	SetLook(B_BORDERED_WINDOW_LOOK);
	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);

	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	SInputAboutDialogView *view = new SInputAboutDialogView(Bounds().InsetByCopy(20, 20));
	AddChild(view);

	Run();

	Lock();
	ResizeTo(view->Bounds().Width() + 40, view->Bounds().Height() + 40);
	Unlock();
}


void
SInputAboutDialog::WindowActivated(bool state)
{
	if(state == false)
	{
		Lock();
		PostMessage(B_QUIT_REQUESTED);
		Unlock();
	}
}
