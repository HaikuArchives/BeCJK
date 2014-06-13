#include <Application.h>
#include <Window.h>
#include <Button.h>
#include <Message.h>
#include <besavager/Tooltips.h>
#include <besavager/Window.h>
#include <besavager/BitmapView.h>
#include <besavager/XPM.h>
#include <stdio.h>

#include "./yes.xpm"
#include "./no.xpm"
#include "./question.xpm"

class MyWindow : public SWindow {
public:
	MyWindow();
	virtual ~MyWindow();
	
	virtual bool QuitRequested();
};


class MyApp : public BApplication {
public:
	MyApp();
	virtual ~MyApp();

private:
	MyWindow *window;
};


MyWindow::MyWindow()
	: SWindow(BRect(200, 200, 600, 350), "Bitmap Test", B_TITLED_WINDOW, 0)
{
	BView *view = new BView(Bounds(), NULL, B_FOLLOW_ALL, 0);
	AddChild(view);

	rgb_color cl = {232, 232, 232, 255};
	view->SetViewColor(cl);

	SBitmapView *view1 = new SBitmapView(BRect(10, 10, 70, 70), "View 1", (const char**)xpm_question, "This is the bitmap view with XPM_QUESTION");
	view->AddChild(view1);

	SBitmapView *view2 = new SBitmapView(BRect(80, 80, 100, 100), "View 2", (const char**)xpm_yes, "XPM_YES");
	view->AddChild(view2);

	SBitmapView *view3 = new SBitmapView(BRect(10, 80, 30, 100), "View 3", (const char**)xpm_no);
	view->AddChild(view3);

	Run();
}


MyWindow::~MyWindow()
{
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
	window = new MyWindow();
	window->Lock();
	window->Show();
	window->Unlock();
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
