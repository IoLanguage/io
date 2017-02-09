//metadoc Curses Steve Dekorte and Edwin Zacharias, 2002
//metadoc Curses license BSD revised

#ifndef IOCURSES_DEFINED
#define IOCURSES_DEFINED 1

#include "IoObject.h"

#define ISCURSES(self) IoObject_hasCloneFunc_(self, (IoTagCloneFunc *)IoCurses_rawClone)

typedef IoObject IoCurses;

typedef struct
{
	int colorOn;
	short fgColor;
	short bgColor;
} IoCursesData;

IoCurses *IoCurses_proto(void *state);
IoCurses *IoCurses_rawClone(IoCurses *self);
IoCurses *IoCurses_new(void *state);
void IoCurses_free(IoCurses *self);

/* ----------------------------------------------------------- */
IoObject *IoCurses_begin(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_end(IoCurses *self, IoObject *locals, IoMessage *m);

IoObject *IoCurses_writeCharacter(IoCurses *self, IoObject *locals, IoMessage *m);

IoObject *IoCurses_nodelay(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_cBreak(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_noCBreak(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_echo(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_noEcho(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_move(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_print(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_insert(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_delete(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_get(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_getCh(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_input(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_erase(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_clear(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_clearToEndOfLine(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_refresh(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_scroll(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_scrollok(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setScrollingRegion(IoCurses *self, IoObject *locals, IoMessage *m);

IoObject *IoCurses_width(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_height(IoCurses *self, IoObject *locals, IoMessage *m);

IoObject *IoCurses_hasColors(IoCurses *self, IoObject *locals, IoMessage *m);

/* ------------------------------------- */
IoObject *IoCurses_setBackgroundBlack(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundBlue(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundGreen(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundCyan(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundRed(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundMagenta(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundYellow(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setBackgroundWhite(IoCurses *self, IoObject *locals, IoMessage *m);

/* ------------------------------------- */
IoObject *IoCurses_setForegroundBlack(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundBlue(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundGreen(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundCyan(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundRed(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundMagenta(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundYellow(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_setForegroundWhite(IoCurses *self, IoObject *locals, IoMessage *m);

IoObject *IoCurses_x(IoCurses *self, IoObject *locals, IoMessage *m);
IoObject *IoCurses_y(IoCurses *self, IoObject *locals, IoMessage *m);

#endif
