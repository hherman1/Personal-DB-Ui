#include "myui2.h"
#include "ui.h"

void edit(char *str,int maxLength,Cursor *cursor,char c) {
	if(c == KEY_LEFT){
		if(cursor->x > 0) cursor->x--;
	}
	if (c == KEY_RIGHT){
		if(cursor->x <maxLength)
			cursor->x++;
	}
	if (c==KEY_BACKSPACE) {
		str[cursor->x-1] = (str[cursor->x] && str[cursor->x] != ' ')?' ':'\0';
		cursor->x--;
	}
	//to do: check if c a letter
	if(c >= ' ' && c <= '~') {
		if(cursor->x <= maxLength) {	
			str[cursor->x++] = c; 
			if(cursor->x > maxLength) {
				cursor->x = maxLength;
			}
		}
	}

}
