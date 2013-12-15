all: mystore myui2

myui2:	myui2.o myui2_util.o keyboard.o xterm_control.o
	gcc myui2.o myui2_util.o keyboard.o xterm_control.o -o myui2
	
myui2.o:	myui2.c myui2.h
	gcc -c myui2.c

myui2_util.o:	myui2_util.c myui2.h
	gcc -c myui2_util.c
	
keyboard.o:	keyboard.c keyboard.h
	gcc -c keyboard.c
	
xterm_control.o:	xterm_control.c xterm_control.h
	gcc -c xterm_control.c
	
mystore:	mystore.o 
	gcc mystore.o -o mystore
	
mystore.o:	mystore.c
	gcc -c mystore.c
