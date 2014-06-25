#include <Application.h>
#include <Window.h>
#include <MenuBar.h>
#include <Message.h>
#include <besavager/Window.h>
#include <besavager/XPM.h>
#include <besavager/MenuItem.h>
#include <stdio.h>

#include "./yes.xpm"
#include "./no.xpm"
#include "./question.xpm"

class MyWindow : public SWindow {
public:
	MyWindow();
	virtual ~MyWindow();
	
	virtual bool QuitRequested();

	virtual void MessageReceived(BMessage *message);

private:
};


class MyApp : public BApplication {
public:
	MyApp();
	virtual ~MyApp();

private:
	MyWindow *window;
};


#define TEST1_MSG 't1ms'
#define TEST2_MSG 't2ms'


MyWindow::MyWindow()
	: SWindow(BRect(200, 200, 600, 350), "Menu Test", B_TITLED_WINDOW, 0)
{
	SXPM image;
	BBitmap *bitmap;

	BMenuBar *menu_bar = new BMenuBar(Bounds(), "Test");

	image.SetTo((const char**)xpm_question);
	bitmap = image.ConvertToBitmap();
	SMenuItem *sub_menu = new SMenuItem(bitmap, new BMenu(""));
	if(bitmap) delete bitmap;
	
	image.SetTo((const char**)xpm_yes);
	bitmap = image.ConvertToBitmap();
	SMenuItem *menu_item1 = new SMenuItem(bitmap, "Test Label 1", new BMessage(TEST1_MSG));
	SMenuItem *menu_item3 = new SMenuItem(bitmap, "Test Label 3", NULL);
	if(bitmap) delete bitmap;

	image.SetTo((const char**)xpm_no);
	bitmap = image.ConvertToBitmap();
	SMenuItem *menu_item2 = new SMenuItem(bitmap, "Test Label 2", new BMessage(TEST2_MSG));
	SMenuItem *menu_item4 = new SMenuItem(bitmap, "Test Label 4", NULL);
	if(bitmap) delete bitmap;

	SMenuItem *menu_item5 = new SMenuItem(NULL, "Test Label 5", NULL);

	menu_item3->SetEnabled(false);
	menu_item4->SetEnabled(false);

	sub_menu->Submenu()->AddItem(menu_item1);
	sub_menu->Submenu()->AddItem(menu_item2);
	sub_menu->Submenu()->AddItem(menu_item3);
	sub_menu->Submenu()->AddItem(menu_item4);
	sub_menu->Submenu()->AddItem(menu_item5);

	menu_bar->AddItem(sub_menu);
	AddChild(menu_bar);

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
		case TEST1_MSG:
			fprintf(stdout, "Test 1 MenuItem Clicked...\n");
			break;

		case TEST2_MSG:
			fprintf(stdout, "Test 2 MenuItem Clicked...\n");
			break;

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
