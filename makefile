all: mystore myui4

myui4:	myui4.o ui.o record.o memory.o color.o myui4_util.o keyboard.o xterm_control.o sockets_send.o
	gcc -g -Wall myui4.o ui.o record.o memory.o color.o myui4_util.o keyboard.o xterm_control.o sockets_send.o -o myui4
	
myui4.o:	myui4.c myui4.h record.h memory.h bindings.h color.h ui.h
	gcc -g -Wall -c myui4.c

ui.o:		ui.c myui4.h ui.h color.h memory.h bindings.h record.h
	gcc -g -Wall -c ui.c

record.o:	record.c record.h myui4.h color.h
	gcc -g -Wall -c record.c

memory.o:	memory.c memory.h
	gcc -g -Wall -c memory.c

color.o:	color.c color.h xterm_control.h memory.h
	gcc -g -Wall -c color.c

myui4_util.o:	myui4_util.c myui4.h 
	gcc -g -Wall -c myui4_util.c
	
keyboard.o:	keyboard.c keyboard.h
	gcc -g -Wall -c keyboard.c
	
xterm_control.o:	xterm_control.c xterm_control.h
	gcc -g -Wall -c xterm_control.c
	
mystore:	mystore.o memory.o sockets_send.o
	gcc -g -Wall mystore.o memory.o sockets_send.o -o mystore
	
mystore.o:	mystore.c memory.h
	gcc -g -Wall -c mystore.c

sockets_send.o:		sockets_send.c
	gcc -Wall -c sockets_send.c