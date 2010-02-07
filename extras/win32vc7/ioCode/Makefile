IOVMDIR = ..\..\..\vm
IOCODEDIR = $(IOVMDIR)\_ioCode
IOCODE = $(IOCODEDIR)\*.io

CC = cl

all : $(IOVMDIR)\IoVMCode.c

rebuild :
  $(MAKE) /A all

clean :
  del io2c.obj
  del io2c.exe
  del $(IOVMDIR)\IoVMCode.c

io2c.exe : $(IOVMDIR)\_tools\io2c.c
  $(CC) $(IOVMDIR)\_tools\io2c.c

$(IOVMDIR)\IoVMCode.c : io2c.exe $(IOCODE)
  call _build_IoVMCode.cmd

