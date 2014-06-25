#include <Application.h>
#include <Window.h>
#include <Button.h>
#include <Message.h>
#include <besavager/Tooltips.h>
#include <besavager/Window.h>
#include <besavager/Button.h>
#include <besavager/BitmapButton.h>
#include <besavager/Separator.h>
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

	virtual void MessageReceived(BMessage *message);

private:
	SButton *btn1;
	SButton *btn2;
	SButton *btn3;

	BButton *btn4;
	BButton *btn5;
	BButton *btn6;

	SBitmapButton *btn7;
	SBitmapButton *btn8;
};


class MyApp : public BApplication {
public:
	MyApp();
	virtual ~MyApp();

private:
	MyWindow *window;
};

#define MSG_BTN1	'mss1'
#define MSG_BTN3	'mss3'

MyWindow::MyWindow()
	: SWindow(BRect(200, 200, 600, 350), "Button Test", B_TITLED_WINDOW, 0)
{
	BView *view = new BView(Bounds(), NULL, B_FOLLOW_ALL, 0);
	AddChild(view);

	rgb_color cl = {232, 232, 232, 255};
	view->SetViewColor(cl);

	btn1 = new SButton(BRect(11, 11, 61, 60), NULL, "SButton1",
					   new BMessage(MSG_BTN1),
					   "AutoDefault and Pixmap Button\nIt is Dano Style...\nSupport Multilines",
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE);

	btn2 = new SButton(BRect(103, 11, 153, 60), NULL, (const char**)xpm_no, "SButton2",
					   NULL,
					   "Navigable & Bitmap & Label Button",
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE);

	btn3 = new SButton(BRect(223, 11, 263, 60), NULL, (const char**)xpm_question,
					   new BMessage(MSG_BTN3),
					   "Bitmap Button",
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);


	btn4 = new BButton(BRect(11, 71, 61, 120), NULL, "BButton1", NULL,
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE);

	btn5 = new BButton(BRect(103, 71, 153, 120), NULL, "BButton2", NULL,
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW | B_NAVIGABLE);

	btn6 = new BButton(BRect(223, 71, 263, 120), NULL, "BButton3", NULL,
					   B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);


	btn7 = new SBitmapButton(BRect(333, 11, 352, 30), NULL,
							 (const char**)xpm_yes, (const char**)xpm_no,
							 "SBitmapButton(S_ONE_STATE_BUTTON)",
							 NULL,
					         B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);

	btn8 = new SBitmapButton(BRect(353, 11, 372, 30), NULL,
							 (const char**)xpm_yes, (const char**)xpm_no,
							 "SBitmapButton(S_TWO_STATE_BUTTON)",
							 NULL,
					         B_FOLLOW_LEFT | B_FOLLOW_TOP, B_FULL_UPDATE_ON_RESIZE | B_WILL_DRAW);

	view->AddChild(btn1);
	view->AddChild(btn2);
	view->AddChild(btn3);

	view->AddChild(btn4);
	view->AddChild(btn5);
	view->AddChild(btn6);

	view->AddChild(btn7);
	view->AddChild(btn8);

	btn1->MakeAutoDefault(true);
//	btn1->SetStyle(DANO_STYLE);
	btn4->MakeDefault(true);

	btn1->ResizeToPreferred();
	btn2->ResizeToPreferred();
	btn3->ResizeToPreferred();
	btn4->ResizeToPreferred();
	btn5->ResizeToPreferred();
	btn6->ResizeToPreferred();
	
//	btn2->SetEnabled(false);
//	btn2->SetStyle(R5_STYLE);
//	btn5->SetEnabled(false);

//	btn3->SetBehavior(S_TWO_STATE_BUTTON);
	btn3->SetValue(1);

	btn8->SetBehavior(S_TWO_STATE_BUTTON);
//	btn8->SetValue(1);
//	btn8->SetEnabled(false);

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
		case MSG_BTN1:
			{
				fprintf(stdout, "****SButton 1****\n");
			}
			break;
		
		case MSG_BTN3:
			{
				fprintf(stdout, "====SButton 3====\n");
			}
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
