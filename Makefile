CC      = gcc
# CFLAGS  = -O2 -DLSB_FIRST -DDEBUG
CFLAGS  = -O2 -DMSB_FIRST -DDEBUG -g -Wall
OBJ	= Z80.o Debug.o x07.o T6834.o video.o lit_ini.o
inc_X11=-I/usr/local/X11/include/
lib_X11=-L/usr/local/X11/lib

dasm:	dasm.c
	gcc -o dasm dasm.c

x07:$(OBJ)
	gcc $(CFLAGS) -o x07 -O $(OBJ) $(lib_X11) -lX11

Z80.o:	Z80.c Z80.h Codes.h CodesED.h CodesCB.h CodesXX.h Tables.h CodesXCB.h
	gcc $(CFLAGS) -c -O Z80.c

Debug.o:Debug.c Z80.h
	gcc $(CFLAGS) -c -O Debug.c

x07.o:x07.c Z80.h Tables.h var.h struct.h const.h proto.h
	gcc $(CFLAGS) -c -O x07.c

T6834.o:T6834.c Z80.h Tables.h var.h struct.h const.h proto.h
	gcc $(CFLAGS) -c -O T6834.c

video.o:T6834.c Z80.h Tables.h var.h struct.h const.h proto.h
	gcc $(CFLAGS) -c -O video.c $(inc_X11)

lit_ini.o:lit_ini.c
	gcc $(CFLAGS) -c -O lit_ini.c
