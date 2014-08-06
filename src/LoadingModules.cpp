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
// Show the status when loading modules

#include <String.h>
#include <Screen.h>
#include <Bitmap.h>
#include <besavager/Autolock.h>

#include "Locale.h"
#include "LoadingModules.h"

class SLoadingModulesViewLight : public BView {
public:
	SLoadingModulesViewLight(BRect frame);
	virtual ~SLoadingModulesViewLight();

	virtual void AttachedToWindow();
	virtual void DetachedFromWindow();

	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);

	virtual void Pulse();
	virtual void Draw(BRect updateRect);

private:
	bool is_red;

	BBitmap *red_bitmap;
	BBitmap *green_bitmap;

	void ReGetBitmap();
};


SLoadingModulesViewLight::SLoadingModulesViewLight(BRect frame)
	: BView(frame, NULL, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS | B_PULSE_NEEDED)
{
	is_red = true;
	red_bitmap = NULL;
	green_bitmap = NULL;
	SetViewColor(220, 220, 220, 255);
}


SLoadingModulesViewLight::~SLoadingModulesViewLight()
{
	if(red_bitmap) delete red_bitmap;
	if(green_bitmap) delete green_bitmap;
}


void
SLoadingModulesViewLight::ReGetBitmap()
{
	BWindow *window = Window();
	if(!window) return;

	bool is_hidden = window->IsHidden();

	if(!is_hidden) window->Hide();

	if(red_bitmap) delete red_bitmap;
	red_bitmap = new BBitmap(Bounds(), B_BITMAP_ACCEPTS_VIEWS, B_RGB32);
	if(red_bitmap)
	{
		SAutolock<BBitmap> autolock(red_bitmap);
		BView *bitmap_view = new BView(red_bitmap->Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);
		red_bitmap->AddChild(bitmap_view);
		bitmap_view->PushState();
		bitmap_view->SetDrawingMode(B_OP_ALPHA);
		bitmap_view->SetHighColor(ViewColor());
		bitmap_view->FillRect(Bounds());
		bitmap_view->SetHighColor(203, 0, 51, 255);
		bitmap_view->FillArc(Bounds(), 0, 360);
		bitmap_view->SetHighColor(20, 20, 20, 150);
		bitmap_view->StrokeArc(Bounds(), 45, 180);
		bitmap_view->SetHighColor(220, 220, 220, 150);
		bitmap_view->StrokeArc(Bounds(), 225, 180);
		bitmap_view->PopState();
		bitmap_view->Sync();
	}

	if(green_bitmap) delete green_bitmap;
	green_bitmap = new BBitmap(Bounds(), B_BITMAP_ACCEPTS_VIEWS, B_RGB32);
	if(green_bitmap)
	{
		SAutolock<BBitmap> autolock(green_bitmap);
		BView *bitmap_view = new BView(green_bitmap->Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);
		green_bitmap->AddChild(bitmap_view);
		bitmap_view->PushState();
		bitmap_view->SetDrawingMode(B_OP_ALPHA);
		bitmap_view->SetHighColor(ViewColor());
		bitmap_view->FillRect(Bounds());
		bitmap_view->SetHighColor(0, 169, 12, 255);
		bitmap_view->FillArc(Bounds(), 0, 360);
		bitmap_view->SetHighColor(20, 20, 20, 150);
		bitmap_view->StrokeArc(Bounds(), 45, 180);
		bitmap_view->SetHighColor(220, 220, 220, 150);
		bitmap_view->StrokeArc(Bounds(), 225, 180);
		bitmap_view->PopState();
		bitmap_view->Sync();
	}

	if(!is_hidden) window->Show();
}


void
SLoadingModulesViewLight::AttachedToWindow()
{
    ReGetBitmap();
}



void
SLoadingModulesViewLight::DetachedFromWindow()
{
	if(red_bitmap) delete red_bitmap;
	if(green_bitmap) delete green_bitmap;
	red_bitmap = NULL;
	green_bitmap = NULL;
	BView::DetachedFromWindow();
}


void
SLoadingModulesViewLight::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}


void
SLoadingModulesViewLight::FrameResized(float new_width, float new_height)
{
	BView::FrameResized(new_width, new_height);
}


void
SLoadingModulesViewLight::Pulse()
{
	is_red = !is_red;
	Invalidate();
}


void
SLoadingModulesViewLight::Draw(BRect updateRect)
{
	BBitmap *bitmap = (is_red ? red_bitmap : green_bitmap);

	if(bitmap)
	{
		SAutolock<BBitmap> autolock(bitmap);
		PushState();
		SetDrawingMode(B_OP_COPY);
		DrawBitmapAsync(bitmap, updateRect, updateRect);
		PopState();
	}

	DrawAfterChildren(updateRect);
}


class SLoadingModulesView : public BView {
public:
	SLoadingModulesView(BRect frame);
	virtual ~SLoadingModulesView();

	virtual void AttachedToWindow();
	virtual void DetachedFromWindow();

	virtual void FrameMoved(BPoint new_position);
	virtual void FrameResized(float new_width, float new_height);

	virtual void Draw(BRect updateRect);

	BRect ContainerBounds();

	static float GetHeightByChild(BView *view);

private:
	BBitmap *bitmap;

	void ReGetBitmap();
};


SLoadingModulesView::SLoadingModulesView(BRect frame)
	: BView(frame, NULL, B_FOLLOW_ALL, B_WILL_DRAW | B_FRAME_EVENTS)
{
	bitmap = NULL;
	SetViewColor(220, 220, 220, 255);

	BRect rect = Bounds();
	rect.right = rect.left + Bounds().Height();
	rect.InsetBy(Bounds().Height() / 3, Bounds().Height() / 3);

	SLoadingModulesViewLight *light = new SLoadingModulesViewLight(rect);
	AddChild(light);
}


SLoadingModulesView::~SLoadingModulesView()
{
	if(bitmap) delete bitmap;
}


BRect
SLoadingModulesView::ContainerBounds()
{
	BRect rect = Bounds();
	rect.left += Bounds().Height() / 2;
	rect.InsetBy(0, 5);
	rect.left += Bounds().Height() / 2;
	rect.InsetBy(4, 4);
	return rect;
}


float
SLoadingModulesView::GetHeightByChild(BView *view)
{
	if(!view) return 0;

	float w = 0, h = 0;
	view->GetPreferredSize(&w, &h);

	float height = h + 18;
	
	return height;
}


void
SLoadingModulesView::ReGetBitmap()
{
	BWindow *window = Window();
	if(!window) return;

	bool is_hidden = window->IsHidden();

	if(!is_hidden) window->Hide();

	BScreen screen(window);
	BBitmap *screen_bitmap = NULL;
	BRect rect = screen.Frame().OffsetToCopy(B_ORIGIN);
	if(screen.GetBitmap(&screen_bitmap, false, &rect) == B_OK)
	{
		if(screen_bitmap)
		{
			if(bitmap) delete bitmap;
			rect = ConvertToScreen(Bounds());
			bitmap = new BBitmap(rect.OffsetToCopy(B_ORIGIN), B_BITMAP_ACCEPTS_VIEWS, B_RGB32);
			if(bitmap)
			{
				SAutolock<BBitmap> autolock(bitmap);
				BView *bitmap_view = new BView(bitmap->Bounds(), NULL, B_FOLLOW_NONE, B_WILL_DRAW);
				bitmap->AddChild(bitmap_view);
				bitmap_view->PushState();
				bitmap_view->SetDrawingMode(B_OP_COPY);
				bitmap_view->DrawBitmap(screen_bitmap, rect, rect.OffsetToCopy(B_ORIGIN));
				bitmap_view->SetDrawingMode(B_OP_ALPHA);

				BPoint circle_center;
				float circle_radius = bitmap->Bounds().Height() / 2;
				circle_center.x = bitmap->Bounds().left + circle_radius;
				circle_center.y = bitmap->Bounds().top + circle_radius;
				
				bitmap_view->SetHighColor(ViewColor());
				bitmap_view->FillArc(circle_center, circle_radius, circle_radius, 0, 360);
				bitmap_view->SetHighColor(220, 220, 220, 150);
				bitmap_view->StrokeArc(circle_center, circle_radius, circle_radius, 45, 180);
				bitmap_view->SetHighColor(20, 20, 20, 150);
				bitmap_view->StrokeArc(circle_center, circle_radius, circle_radius, 225, 180);

				BRect rect = bitmap->Bounds();
				rect.left = circle_center.x;
				rect.InsetBy(0, 5);

				bitmap_view->SetHighColor(ViewColor());
				bitmap_view->FillRoundRect(rect, 3, 3);
				bitmap_view->SetHighColor(220, 220, 220, 150);
				bitmap_view->StrokeRoundRect(rect, 3, 3);
				bitmap_view->SetHighColor(20, 20, 20, 150);
				rect.left += 1; rect.top += 1;
				bitmap_view->StrokeRoundRect(rect, 3, 3);
				rect.right -= 1; rect.bottom -= 1;
				bitmap_view->SetHighColor(ViewColor());
				bitmap_view->FillArc(circle_center, circle_radius - 1, circle_radius - 1, 0, 360);
				bitmap_view->FillRoundRect(rect, 3, 3);

				bitmap_view->PopState();
				bitmap_view->Sync();
			}
			delete screen_bitmap;
		}
	}

	if(!is_hidden) window->Show();
}


void
SLoadingModulesView::AttachedToWindow()
{
    ReGetBitmap();
}



void
SLoadingModulesView::DetachedFromWindow()
{
	if(bitmap) delete bitmap;
	bitmap = NULL;
	BView::DetachedFromWindow();
}


void
SLoadingModulesView::FrameMoved(BPoint new_position)
{
	BView::FrameMoved(new_position);
}


void
SLoadingModulesView::FrameResized(float new_width, float new_height)
{
	BView::FrameResized(new_width, new_height);
}


void
SLoadingModulesView::Draw(BRect updateRect)
{
	if(bitmap)
	{
		SAutolock<BBitmap> autolock(bitmap);
		PushState();
		SetDrawingMode(B_OP_COPY);
		DrawBitmapAsync(bitmap, updateRect, updateRect);
		PopState();
	}

	DrawAfterChildren(updateRect);
}


SLoadingModules::SLoadingModules(int32 modules_count)
	: BWindow(BRect(-100, -100, -10, -10), "BeCJK Loading Moudles", B_BORDERED_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_WILL_ACCEPT_FIRST_CLICK | B_ASYNCHRONOUS_CONTROLS | B_NOT_MINIMIZABLE | B_NOT_RESIZABLE | B_AVOID_FOCUS, B_ALL_WORKSPACES)
{
	count = 0;
	nmodules = modules_count;
	SetPulseRate(500000);
	SetSizeLimits(0, 10000, 0, 10000);

	BString str;
	str << "BeCJK " << _("Loading Modules: ");

	bar = new BStatusBar(BRect(0, 0, 10, 10), NULL, str.String(), NULL);
	rgb_color bar_color = {255, 203, 0, 255};
	bar->SetBarColor(bar_color);
	bar->SetMaxValue(nmodules);
	bar->SetFont(be_plain_font);
	bar->SetFontSize(10);
	bar->SetBarHeight(10);
	float w = 0, h = 0;
	bar->GetPreferredSize(&w, &h);


	BScreen screen(this);
	BRect rect = screen.Frame();
	rect.OffsetTo(B_ORIGIN);
	rect.top = rect.bottom - SLoadingModulesView::GetHeightByChild(bar);
	ResizeTo(rect.Width(), rect.Height());
	MoveTo(rect.LeftTop());

	SLoadingModulesView *main_view = new SLoadingModulesView(Bounds());
	main_view->AddChild(bar);
	bar->ResizeTo(main_view->ContainerBounds().Width(), main_view->ContainerBounds().Height());
	bar->MoveTo(main_view->ContainerBounds().LeftTop());

	AddChild(main_view);

	RemoveShortcut((uint32)'W', (uint32)B_COMMAND_KEY);
	RemoveShortcut((uint32)'Q', (uint32)B_COMMAND_KEY);

	SetFeel(B_FLOATING_ALL_WINDOW_FEEL);
	SetLook(B_NO_BORDER_WINDOW_LOOK);

	Run();
}


SLoadingModules::~SLoadingModules()
{
}


void
SLoadingModules::ModuleChanged(const char *module)
{
	count++;

	BString str;
	if(module) str << "(" << count << "/" << nmodules << ")";

	if(count > 1) bar->Update(1, module, str.String());
}
