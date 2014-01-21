all: mystore myui2

myui2:	myui2.o ui.o record.o color.o myui2_util.o keyboard.o xterm_control.o
	gcc -g -Wall myui2.o ui.o record.o color.o myui2_util.o keyboard.o xterm_control.o -o myui2
	
myui2.o:	myui2.c myui2.h record.h bindings.h color.h
	gcc -g -Wall -c myui2.c

ui.o:		ui.c myui2.h ui.h color.h
	gcc -g -Wall -c ui.c

record.o:	record.c record.h myui2.h color.h
	gcc -g -Wall -c record.c

color.o:	color.c color.h xterm_control.h
	gcc -g -Wall -c color.c

myui2_util.o:	myui2_util.c myui2.h
	gcc -g -Wall -c myui2_util.c
	
keyboard.o:	keyboard.c keyboard.h
	gcc -g -Wall -c keyboard.c
	
xterm_control.o:	xterm_control.c xterm_control.h
	gcc -g -Wall -c xterm_control.c
	
mystore:	mystore.o 
	gcc -g -Wall mystore.o -o mystore
	
mystore.o:	mystore.c
	gcc -g -Wall -c mystore.c
