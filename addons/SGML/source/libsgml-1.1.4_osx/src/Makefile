# Generated automatically from Makefile.in by configure.
#
# source make file
#

CC=gcc
DEBUG=-g
INCLUDE=-I../include -I.
LIBS=
CFLAGS=${DEBUG} ${INCLUDE} -Wall -g
LFLAGS=${DEBUG} ${INCLUDE} -Wall -g -o
OBJS=DomComment.o DomDocument.o DomElement.o DomNode.o DomNodeList.o DomText.o SgmlExtensionHtml.o SgmlExtensionXml.o SgmlParser.o Variant.o
LIBNAME=../libsgml.a
LIBEXEC=ar rc ${LIBNAME} ${OBJS} ${LIBS} && ranlib ${LIBNAME}
LIBBIN=${CXX} ${DEBUG} -Wall -g -dynamiclib -fPIC -o ../libsgml.dylib ${OBJS}

all: ${OBJS}
	${LIBEXEC}
	${LIBBIN}

*.c.${O}:
	${CC} ${CFLAGS} $*.c

clean:
	rm -f *.o core ${LIBNAME} ../libsgml.dylib

distclean:
	rm -f *.o core ${LIBNAME} ../libsgml.dylib Makefile
