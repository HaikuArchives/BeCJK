#include <Application.h>
#include <Window.h>
#include <Message.h>
#include <View.h>
#include <besavager/Tooltips.h>
#include <besavager/Window.h>
#include <besavager/StringView.h>


class MyWindow : public SWindow {
public:
	MyWindow();
	virtual ~MyWindow();
	
	virtual bool QuitRequested();

	virtual void MessageReceived(BMessage *message);
};


class MyApp : public BApplication {
public:
	MyApp();
	virtual ~MyApp();

private:
	MyWindow *window;
};


MyWindow::MyWindow()
	: SWindow(BRect(200, 200, 600, 350), "StringView Test", B_TITLED_WINDOW, 0)
{
	SStringView *view = new SStringView(Bounds(), NULL,
										"Test the StringView\nThen you'll see the result\n\nTest the StringView\nThen you'll see the result",
										"It's tooltips for it",
										B_FOLLOW_ALL);
	view->SetAlignment(B_ALIGN_CENTER);
	view->SetViewColor(233, 233, 233);
	view->Highlight(29, 37);
//	view->SetEnabled(false);
	AddChild(view);

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

void
MyWindow::MessageReceived(BMessage *message)
{
	if(!message) return;
	
	switch(message->what)
	{
		default:
			SWindow::MessageReceived(message);
	}
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
