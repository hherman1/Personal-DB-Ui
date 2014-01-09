#include "myui2.h"
#include "ui.h"
#include "record.h"
#include "bindings.h"

int scroll(Record **hovered, struct RecordList **activeBuffer,char c) {
	int redraw = FALSE;
	if(DEBUG && c == 'r') {
		message("refreshed | botom: %i | (*hovered)->prev:%p",(*activeBuffer)->bottom->num,(*hovered)->prev);
		redraw = TRUE; 
	}
	if(DEBUG && c == 'p') {
		DUMP = TRUE;
		redraw = TRUE;
		message("CORE DUMP: ");
	}
	if (c == KEY_ENTER) {
		selectRecord(*(*hovered),*(*activeBuffer));
		redraw = TRUE;
	}
	if (c == KEY_DOWN) {
		if ((*hovered) == (*activeBuffer)->bottom && (*activeBuffer)->lengthfrombot){
			scrollDown((*activeBuffer));
			(*hovered) = (*activeBuffer)->bottom;
		}else if((*hovered)->next) {
			(*hovered) = (*hovered)->next;
			
		}
		redraw = TRUE;
	} 
	if (c == KEY_UP) {
		if((*hovered) == (*activeBuffer)->top && (*hovered)->num - 1) {
			scrollUp((*activeBuffer));
			(*hovered) = (*activeBuffer)->top;
		}else if((*hovered)->prev) {
			(*hovered) = (*hovered)->prev;
		}
		redraw = TRUE;
	}
	return redraw;
}
int modeCheck(int c,int recordSelected,int *cursorLeft,char **cursorArea, char *subject,Cursor *cursor,Area rArea,Record **hovered,struct RecordList **activeBuffer,  struct RecordList *RLBuffer) {
	int redraw = FALSE;
	if(KEY_MODE_RECORDS(c) || KEY_MODE_ESCAPE(c)) {
		redraw = check_record(cursorArea,hovered,activeBuffer,RLBuffer);
	}
	else if (KEY_MODE_ADD(c)) {
		redraw = init_add(cursorArea,cursor);
	}
	else if (KEY_MODE_SEARCH(c)){
		redraw = init_search(subject,cursor,cursorArea);
	}
	else if (KEY_MODE_EDIT(c)) {
		redraw = init_edit(recordSelected,cursorLeft,cursorArea,cursor,rArea,*hovered,*activeBuffer);
	}
	printf("%i\n",redraw);
	return redraw;
}
int check_record(char **cursorArea, Record **hovered,struct RecordList **activeBuffer,struct RecordList *RLBuffer) {
	if(!strcmp(*cursorArea,"record")) {
		*activeBuffer = RLBuffer;
	}
	*hovered = (*activeBuffer)->top;
	*cursorArea = "record";
	return TRUE;
}
int init_add(char **cursorArea, Cursor* cursor) {
	*cursorArea = "addSubject";
	cursor->x = 0;
	return TRUE;
}
int init_search(char *subject,Cursor *cursor,char **cursorArea) {
	fill(subject,MAX_SUBJECT_LEN,'\0');
	*cursorArea = "search";
	cursor->y = 5;
	cursor->x = 0;
	return TRUE;
}
int init_edit(int recordSelected,int *cursorLeft,char **cursorArea, Cursor *cursor,Area rArea,Record *hovered,struct RecordList *activeBuffer) {
	*cursorArea = "editSubject";
	if(hovered->num != recordSelected) {
		selectRecord(*hovered,*activeBuffer);
	}
	*cursorLeft = RECORD_NUM_SPACE +1;
	cursor->x = strlen(hovered->subject);
	cursor->y = getRecordY(hovered,activeBuffer,rArea);
	return TRUE;
}
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
