#include "myui2.h"
#include "ui.h"
#include "record.h"
#include "bindings.h"

int scroll(Record **hovered, struct RecordList **activeBuffer,char c) {

	int redraw = FALSE;
	if(*hovered) {
		if(DEBUG && c == 'r') {
			message("refreshed | botom: %i | (*hovered)->prev:%p",(*activeBuffer)->bottom->num,(*hovered)->prev);
			redraw = TRUE; 
		}
		if(DEBUG && c == 'p') {
			DUMP = TRUE;
			redraw = TRUE;
		}
		if(DUMP) {
			message("%p<-[[%p]]->%p",(*activeBuffer)->top->prev,(*activeBuffer)->top,(*activeBuffer)->top->next);
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
	}
	return redraw;
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

int modeCheck(int c,int recordSelected,int *cursorLeft,int *cursorArea, int *colorScheme,char *subject,Cursor *cursor,Area rArea,Record **hovered,struct RecordList **activeBuffer,  struct RecordList *RLBuffer) {
	int redraw = FALSE;
	if(commandMode(*cursorArea)) {
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
		} else if(KEY_MODE_DELETE(c)) {
			redraw = init_delete(recordSelected,cursorArea,colorScheme,*hovered,*activeBuffer);
		}
	}
	return redraw;
}
int check_record(int *cursorArea, Record **hovered,struct RecordList **activeBuffer,struct RecordList *RLBuffer) {
	if(*cursorArea == UI_AREA_RECORDS) {
		*activeBuffer = RLBuffer;
	}
	*hovered = (*activeBuffer)->top;
	*cursorArea = UI_AREA_RECORDS;
	return TRUE;
}
int init_add(int *cursorArea, Cursor* cursor) {
	*cursorArea = UI_AREA_ADD_SUBJECT;
	cursor->x = 0;
	return TRUE;
}
int init_search(char *subject,Cursor *cursor,int *cursorArea) {
	fill(subject,MAX_SUBJECT_LEN,'\0');
	*cursorArea = UI_AREA_SEARCH;
	cursor->y = 5;
	cursor->x = 0;
	return TRUE;
}
int init_edit(int recordSelected,int *cursorLeft,int *cursorArea, Cursor *cursor,Area rArea,Record *hovered,struct RecordList *activeBuffer) {
	*cursorArea = UI_AREA_EDIT_SUBJECT;
	if(hovered->num != recordSelected) {
		selectRecord(*hovered,*activeBuffer);
	}
	*cursorLeft = RECORD_NUM_SPACE +1;
	cursor->x = strlen(hovered->subject);
	cursor->y = getRecordY(hovered,activeBuffer,rArea);
	return TRUE;
}
int init_delete(int recordSelected,int *cursorArea,int *colorScheme,Record *hovered,struct RecordList *activeBuffer) {
	*cursorArea = UI_AREA_DELETE;
	*colorScheme = R_COLOR_SCHEME_DELETE;
	if(hovered->num != recordSelected) {
		selectRecord(*hovered,*activeBuffer);
	}
	message(UI_DELETE_CONFIRM(hovered));
	return TRUE;
}
int commandMode(int cursorArea) {
	return UI_COMMANDABLE(cursorArea);
}