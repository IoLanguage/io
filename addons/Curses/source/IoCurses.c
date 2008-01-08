/*#io
Curses ioDoc(
		   docCopyright("Steve Dekorte", 2004)
		   docLicense("BSD revised")
		   docCategory("Server")
		   docDescription("Curses allows writing and reading at arbitrary positions on the terminal. You have to call init to set the terminal to curses mode and end to end curses mode. The terminal is not updated until refresh is called. It is a bad idea to use the standard io's read and write methods when the terminal is in curses mode. The Curses primitive was written by Edwin Zacharias.
<BR><BR>
Here's an example that prints Hello at column 5 and row 7;
<PRE>
Curses init
Curses move(5, 7) print(\"Hello\")
Curses refresh
Curses end
<PRE>

credits: Original version by Edwin Zacharias. getCh method by Scott Dunlop.
")
		   */

#include "IoCurses.h"
#include "IoState.h"
#include "IoNumber.h"
#include "IoSeq.h"

#if defined(__CYGWIN__) || defined(__MINGW32__)
#include <curses.h>
#else
#include <ncurses.h>
#endif

#define IO_CURSES_INPUT_BUFFER_LENGTH 200

#define DATA(self) ((IoCursesData *)IoObject_dataPointer(self))

IoTag *IoCurses_newTag(void *state)
{
	IoTag *tag = IoTag_newWithName_("Curses");
	IoTag_state_(tag, state);
	IoTag_freeFunc_(tag, (IoTagFreeFunc *)IoCurses_free);
	IoTag_cloneFunc_(tag, (IoTagCloneFunc *)IoCurses_rawClone);
	return tag;
}

IoCurses *IoCurses_proto(void *state)
{
	IoObject *self = IoObject_new(state);
	IoObject_tag_(self, IoCurses_newTag(state));

	IoObject_setDataPointer_(self, calloc(1, sizeof(IoCursesData)));
	IoState_registerProtoWithFunc_(state, self, IoCurses_proto);

	{
		IoMethodTable methodTable[] = {
		{"begin", IoCurses_begin},
		{"end",      IoCurses_end},

		//{"nodelay",  IoCurses_nodelay},
		//{"cBreak",   IoCurses_cBreak},
		//{"noCBreak", IoCurses_noCBreak},
		//{"echo",   IoCurses_echo},
		//{"noEcho", IoCurses_noEcho},
		{"move",   IoCurses_move},
		{"write",  IoCurses_print},
		{"writeCharacter", IoCurses_writeCharacter},
		//{"insert", IoCurses_insert},
		//{"delete", IoCurses_delete},
		//{"get",    IoCurses_get},
		{"asyncReadCharacter",  IoCurses_getCh},
		//{"input",  IoCurses_input},
		//{"erase",  IoCurses_erase},
		{"clear",  IoCurses_clear},
		//{"clearToEndOfLine", IoCurses_clearToEndOfLine},
		{"refresh",  IoCurses_refresh},

		{"x", IoCurses_x},
		{"y", IoCurses_y},

		{"width",    IoCurses_width},
		{"height",   IoCurses_height},

		{"hasColors", IoCurses_hasColors},

		{"setBackgroundBlack",   IoCurses_setBackgroundBlack},
		{"setBackgroundBlue",    IoCurses_setBackgroundBlue},
		{"setBackgroundGreen",   IoCurses_setBackgroundGreen},
		{"setBackgroundCyan",    IoCurses_setBackgroundCyan},
		{"setBackgroundRed",     IoCurses_setBackgroundRed},
		{"setBackgroundMagenta", IoCurses_setBackgroundMagenta},
		{"setBackgroundYellow",  IoCurses_setBackgroundYellow},
		{"setBackgroundWhite",   IoCurses_setBackgroundWhite},

		{"setForegroundBlack",   IoCurses_setForegroundBlack},
		{"setForegroundBlue",    IoCurses_setForegroundBlue},
		{"setForegroundGreen",   IoCurses_setForegroundGreen},
		{"setForegroundCyan",    IoCurses_setForegroundCyan},
		{"setForegroundRed",     IoCurses_setForegroundRed},
		{"setForegroundMagenta", IoCurses_setForegroundMagenta},
		{"setForegroundYellow",  IoCurses_setForegroundYellow},
		{"setForegroundWhite",   IoCurses_setForegroundWhite},
		{NULL, NULL},
		};

		IoObject_addMethodTable_(self, methodTable);
	}
	return self;
}

IoCurses *IoCurses_rawClone(IoCurses *proto)
{
	IoCurses *self = IoObject_rawClonePrimitive(proto);
	IoObject_setDataPointer_(self, cpalloc(DATA(proto), sizeof(IoCursesData)));
	return self;
}

/* ----------------------------------------------------------- */

IoCurses *IoCurses_new(void *state)
{
	IoObject *proto = IoState_protoWithInitFunction_(state, IoCurses_proto);
	return IOCLONE(proto);
}

void IoCurses_free(IoCurses *self)
{
	free(IoObject_dataPointer(self));
}

/* ----------------------------------------------------------- */

void IoCurses_suspendCurses(IoCurses *self)
{
	def_prog_mode();
	endwin();
	refresh();
}

void IoCurses_resumeCurses(IoCurses *self)
{
	refresh();
}

IoObject *IoCurses_showError(IoCurses *self, IoMessage *m, const char *name, const char *description)
{
	IoCurses_suspendCurses(self);
	IoCurses_showError(self, m, name, description);
	IoCurses_resumeCurses(self);
	return self;
}

/* in curses y (the column) is listed before x (the row) */

IoObject *IoCurses_begin(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("begin", "Sets the terminal to curses mode. This should be called before any other curses methods. Returns self. ")
	*/

	initscr();
	//start_color();
	//DATA(self)->colorOn = 1;

	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	nodelay(stdscr, TRUE);

	nonl();
	noecho();
	cbreak();

	return self;
}

IoObject *IoCurses_end(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("end", "Ends curses mode. This should be called before standard io's read and write methods are used. Returs self. ")
	*/
	printf("IoCurses_end\n");

	//echo();
	//nocbreak();
	//nl();

	//clear();
	//refresh();
	//reset_shell_mode();
	endwin();
	//reset_shell_mode();
	return self;
}


IoObject *IoCurses_nodelay(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("nodelay(aBoolean)",
		   "Enables or disables block during read. If aNumber is zero, nodelay is set to be false, otherwise it is set to be true. ")
	*/

	int b = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
	nodelay(stdscr, b);
	return self;
}


IoObject *IoCurses_cBreak(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("cBreak(aString)",
		   "Disables line buffering and erase/kill character-processing. cBreak should be on for most purposes. Returns self. ")
	*/

	if (cbreak() == ERR)
	{
		IoCurses_showError(self, m, "Curses.cBreak", "Failed to enable cBreak.");
	}
	return self;
}

IoObject *IoCurses_noCBreak(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("noCBreak",
		   "Allows line buffering and erase/kill character-processing. cBreak should be on for most purposes. Returns self. ")
	*/

	if (nocbreak() == ERR)
	{
		IoCurses_showError(self, m, "Curses.noCBreak", "Failed to disable cBreak.");
	}
	return self;
}

IoObject *IoCurses_echo(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("echo",
		   "Echoes user input to terminal. Returns self. ")
	*/

	if (echo() == ERR)
	{
		IoCurses_showError(self, m, "Curses.echo", "Failed to enable echo.");
	}
	return self;
}

IoObject *IoCurses_noEcho(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("noEcho",
		   "Does not echo user input to terminal. Returns self.")
	*/

	if (noecho() == ERR)
	{
		IoCurses_showError(self, m, "Curses.noecho", "Failed to disable echo.");
	}
	return self;
}

IoObject *IoCurses_move(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("move(x, y)",
		   "Moves the cursor to column y and row x on the terminal. (0, 0) is at the top-left of the terminal. Returns self. ")
	*/

	int x = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
	int y = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 1));

	if (move(y, x) == ERR)
	{
		IoCurses_showError(self, m, "Curses.move", "Failed to move curser.");
	}

	return self;
}

IoObject *IoCurses_writeCharacter(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("writeCharacter(aCharacter)",
		   "Prints the aCharacter to the current position on the terminal, overwriting existing text on the terminal. Returns self. ")
	*/


	char c = IoMessage_locals_intArgAt_(m, locals, 0);

	if (addch(c) == ERR)
	{
		/* IoCurses_showError(self, m, "Curses.print", "Failed to print string.");*/
	}
	return self;
}

IoObject *IoCurses_print(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("print(aString)",
		   "Prints the string to the current position on the terminal, overwriting existing text on the terminal. Returns self. ")
	*/


	char *string = IoSeq_asCString(IoMessage_locals_seqArgAt_(m, locals, 0));

	if (addstr(string) == ERR)
	{
		/* IoCurses_showError(self, m, "Curses.print", "Failed to print string.");*/
	}
	return self;
}

IoObject *IoCurses_insert(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("insert(aString)",
		   "Inserts the string at the current position on the terminal, pushing existing text to the right. Returns self. ")
	*/

	char *string = IoSeq_asCString(IoMessage_locals_seqArgAt_(m, locals, 0));

	if (insstr(string) == ERR)
	{
		IoCurses_showError(self, m, "Curses.insert", "Failed to insert string.");
	}
	return self;
}

IoObject *IoCurses_delete(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("delete(n)",
		   "Deletes n characters at the current position. Text to the right is shifted left. n is optional and defaults to 1. Returns self. ")
	*/

	int n = 1;

	if (IoMessage_argCount(m) > 0)
	{
		n = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
	}

	for (n = n; n > 0; n--)
	{
		if (delch() == ERR)
		{
			IoCurses_showError(self, m, "Curses.delete", "Failed to delete string.");
		}
	}
	return self;
}

IoObject *IoCurses_get(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("get(n)",
		   "Returns n characters from the terminal. n is optional and defaults to 1. ")
	*/

	int inputCharacterLimit = IO_CURSES_INPUT_BUFFER_LENGTH;
	char string[inputCharacterLimit];

	if (IoMessage_argCount(m) > 0)
	{
		IoNumber *number = IoMessage_locals_numberArgAt_(m, locals, 0);
		inputCharacterLimit = IoNumber_asInt(number);
	}

	if (getnstr(string, inputCharacterLimit) == ERR)
	{
		return IONIL(self);
	}

	return IoState_symbolWithCString_(IOSTATE, string);
}

IoObject *IoCurses_getCh(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("getCh", "Reads a single-byte character from the terminal associated with the current or specified window. Returns a Number containing the byte.")
	*/

	int key = getch();

	if (key == ERR)
	{
		return IONIL(self);
	}

	return IONUMBER((double)key);
}

IoObject *IoCurses_input(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("input(n)",
		   "Returns user input up to a return, or a maximun of n characters. ")
	*/

	int length = 1;
	char string[length+1];

	if (IoMessage_argCount(m) > 0)
	{
		length = IoNumber_asInt(IoMessage_locals_numberArgAt_(m, locals, 0));
	}

	if (innstr(string, length) == ERR)
	{
		return IONIL(self);
	}

	return IoState_symbolWithCString_(IOSTATE, string);
}

IoObject *IoCurses_clear(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clear",
		   "Clears the terminal. Nicer than erase. Returns self. ")
	*/

	clear();
	return self;
}

IoObject *IoCurses_clearToEndOfLine(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("clearToEndOfLine",
		   "Clears the text from the cursor to the end of the line. Returns self. ")
	*/

	clrtoeol();
	return self;
}

IoObject *IoCurses_refresh(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("refresh",
		   "Copies the current buffer to the screen. This must be called to make changes to the screen. Returns self. ")
	*/

	if (refresh() == ERR)
	{
		IoCurses_showError(self, m, "Curses.refresh", "Failed to refresh screen.");
	}
	return self;
}

IoObject *IoCurses_width(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("width",
		   "Returns a Number containing the width of the current screen.")
	*/

	int w, h;
	getmaxyx(stdscr, h, w);
	return IONUMBER(w);
}

IoObject *IoCurses_height(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("height",
		   "Returns a Number containing the height of the current screen.")
	*/

	int w, h;
	getmaxyx(stdscr, h, w);
	return IONUMBER(h);
}

IoObject *IoCurses_hasColors(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("hasColors",
		   "Returns true if the terminal supports color, false otherwise.")
	*/

	return IOBOOL(self, has_colors());
}

void IoCurses_colorSet(IoCurses *self)
{
	if (!DATA(self)->colorOn)
	{
		start_color();
		DATA(self)->colorOn = 1;
		/*pair_content(0, &(DATA(self)->fgColor), &(DATA(self)->bgColor));*/
	}
	init_pair(0, DATA(self)->fgColor, DATA(self)->bgColor);
#if ! defined(__PDCURSES__)
	color_set(0, NULL);
#else
	attrset(COLOR_PAIR(0));
#endif
}

/* --- Background --- */

IoObject *IoCurses_setBackgroundBlack(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundBlack",
		   "Sets the background color to black.")
	*/

	DATA(self)->bgColor = COLOR_BLACK;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundBlue(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundBlue",
		   "Sets the background color to blue.")
	*/

	DATA(self)->bgColor = COLOR_BLUE;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundGreen(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundGreen",
		   "Sets the background color to green.")
	*/

	DATA(self)->bgColor = COLOR_GREEN;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundCyan(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundCyan",
		   "Sets the background color to cyan.")
	*/

	DATA(self)->bgColor = COLOR_CYAN;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundRed(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundRed",
		   "Sets the background color to red.")
	*/

	DATA(self)->bgColor = COLOR_RED;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundMagenta(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundMagenta",
		   "Sets the background color to magenta.")
	*/

	DATA(self)->bgColor = COLOR_MAGENTA;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundYellow(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundYellow",
		   "Sets the background color to yellow.")
	*/

	DATA(self)->bgColor = COLOR_YELLOW;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setBackgroundWhite(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setBackgroundWhite",
		   "Sets the background color to white.")
	*/

	DATA(self)->bgColor = COLOR_WHITE;
	IoCurses_colorSet(self);
	return self;
}

/* --- Foreground --- */

IoObject *IoCurses_setForegroundBlack(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundBlack",
		   "Sets the foreground color to black.")
	*/

	DATA(self)->fgColor = COLOR_BLACK;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundBlue(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundBlue",
		   "Sets the foreground color to blue.")
	*/

	DATA(self)->fgColor = COLOR_BLUE;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundGreen(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundGreen",
		   "Sets the foreground color to green.")
	*/

	DATA(self)->fgColor = COLOR_GREEN;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundCyan(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundCyan",
		   "Sets the foreground color to cyan.")
	*/

	DATA(self)->fgColor = COLOR_CYAN;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundRed(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundRed",
		   "Sets the foreground color to red.")
	*/

	DATA(self)->fgColor = COLOR_RED;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundMagenta(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundMagenta",
		   "Sets the foreground color to magenta.")
	*/

	DATA(self)->fgColor = COLOR_MAGENTA;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundYellow(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundYellow", "Sets the foreground color to yellow.")
	*/

	DATA(self)->fgColor = COLOR_YELLOW;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_setForegroundWhite(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("setForegroundWhite", "Sets the foreground color to white.")
	*/

	DATA(self)->fgColor = COLOR_WHITE;
	IoCurses_colorSet(self);
	return self;
}

IoObject *IoCurses_x(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("x", "Returns the cursor x position.")
	*/
	int x, y;

	getyx(stdscr, y, x);

	return IONUMBER(x);
}

IoObject *IoCurses_y(IoCurses *self, IoObject *locals, IoMessage *m)
{
	/*#io
	docSlot("y", "Returns the cursor y position.")
	*/
	int x, y;

	getyx(stdscr, y, x);

	return IONUMBER(y);
}
