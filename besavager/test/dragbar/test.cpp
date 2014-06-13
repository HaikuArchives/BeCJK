#include <Application.h>
#include <Window.h>
#include <Button.h>
#include <StringView.h>
#include <Message.h>
#include <besavager/Window.h>
#include <besavager/DragBar.h>
#include <besavager/Button.h>
#include <besavager/Cursor.h>
#include <stdio.h>

class MyWindow : public SWindow {
public:
	MyWindow(BRect frame, const char *title, window_type wtype, uint32 flags, int32 bar_position, bool within_screen = false);
	
	virtual bool QuitRequested();
};


class MyApp : public BApplication {
public:
	MyApp();
	virtual ~MyApp();
};


MyWindow::MyWindow(BRect frame, const char *title, window_type wtype, uint32 flags, int32 bar_position, bool within_screen)
	: SWindow(frame, title, wtype, flags)
{
	SetSizeLimits(0, 10000, 0, 10000);

	SDragBar *dragbar = new SDragBar(Bounds(), NULL, bar_position, "Drag it to move the window", within_screen);
	AddChild(dragbar);

	SButton *button = new SButton(dragbar->ChildBounds(), NULL, title, NULL, B_FOLLOW_ALL);
	AddChild(button);

	Run();
}


bool
MyWindow::QuitRequested()
{
    be_app->PostMessage(B_QUIT_REQUESTED);
	return true;
}


MyApp::MyApp()
	: BApplication("application/x-vnd.anthonylee-test")
{
	MyWindow *window1 = new MyWindow(BRect(200, 200, 600, 270), "TOP & WITHIN_SCREEN", B_BORDERED_WINDOW, B_AVOID_FOCUS, S_DRAG_BAR_TOP, true);
	window1->Lock();
	window1->Show();
	window1->Unlock();

	MyWindow *window2 = new MyWindow(BRect(200, 300, 600, 370), "BOTTOM", B_BORDERED_WINDOW, B_AVOID_FOCUS, S_DRAG_BAR_BOTTOM);
	window2->Lock();
	window2->Show();
	window2->Unlock();

	MyWindow *window3 = new MyWindow(BRect(200, 400, 600, 470), "LEFT", B_BORDERED_WINDOW, 0, S_DRAG_BAR_LEFT);
	window3->Lock();
	window3->Show();
	window3->Unlock();

	MyWindow *window4 = new MyWindow(BRect(200, 500, 600, 570), "RIGHT", B_BORDERED_WINDOW, 0, S_DRAG_BAR_RIGHT);
	window4->Lock();
	window4->Show();
	window4->Unlock();
}


MyApp::~MyApp()
{
}


int main()
{
	MyApp *app = new MyApp();
	app->Run();
	delete app;

	return 0;
}
