# Thanks to Dru Nelson for help with this makefile

CFLAGS := -O2 -g -I../../../IoVM -I../../../IoVM/base -I../../
CFLAGS += -mwindows
MAKE     = make

CCLIBS := -L../../../IoDesktop -lIoDesktop
CCLIBS += -L../../Image/ -lIoImage
CCLIBS += -L../../../IoServer -lIoServer 
CCLIBS += -L../../../IoVM -lIoVM -lm

CFLAGS += $(ADDONS:%=-DIO_ADDON_%)
CFLAGS += $(ADDONS:%=-I%)

# ========================================================

# Cygwin
CCLIBS += -lopengl32 -lglu32 -lglut32

#CCLIBS += -L../../OpenAL/base/openal/linux/src -lOpenAL
#CCLIBS += -L../../FreeType/freetype-2.1.3rc2/objs/.libs -lfreetype
CCLIBS += -L../../FreeType/freetype/objs/.libs -lfreetype
CCLIBS +=  -L../../Image/base/png -lpng
CCLIBS +=  -L/usr/lib/ -lz

# ========================================================

all:
	make main.o
	$(CC) -o ioPlayer main.o $(CCLIBS) 

clean:
	find . -name "*.[oa]" -print | xargs rm -f
	(if test -f ioPlayer; \
		then rm -f ioPlayer; \
	fi); 
