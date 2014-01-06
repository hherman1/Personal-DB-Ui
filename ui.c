#include "myui2.h"
#include "ui.h"

void edit(char *str,int maxLength,Cursor *cursor,char c) {
	if(c == KEY_LEFT){
		if(cursor.x > 0) cursor.x--;
	}
	if (c == KEY_RIGHT){
		cursor.x++;
	}
	if (c==KEY_BACKSPACE) {
		hovered->subject[cursor.x] = hovered->subject[cursor.x--]?' ':'\0';
	}
	//to do: check if c a letter
	if(c >= ' ' && c <= '~') {
		if(cursor.x <= maxLength) {	
			hovered->subject[cursor.x++] = c; 
			if(cursor.x > maxLength) {
				cursor.x = maxLength;
			}
		}
	}

}
