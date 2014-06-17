#include <iostream.h>
#include <besavager/XPM.h>

#include "cursor-move.xpm"

int main(int argc, char** argv)
{
	SXPM image((const char**)xpm_cursor_move);
//	SXPM image("cursor-move.xpm");

	char *string = image.ConvertToCursorForProgram("S_CURSOR_MOVE", 2, 3);
	
	if(string)
	{
		cout << string;
		delete[] string;
		return 0;
	}
	else
	{
		cout << "Can't not convert the XPM data to Cursor!\n";
		return 1;
	}
}
