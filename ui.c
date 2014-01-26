#include "myui2.h"
#include "ui.h"
#include "record.h"
#include "bindings.h"
#include "color.h"
#include "memory.h"

extern struct RecordColor classicModeBar;
extern char subject[30];
extern char body[140];
//display functions
int displayUIElement(Area window,struct displayText ui, ...) {
	int row = 0;
	int col = 0;
	int len = strlen(ui.text);
	va_list args;
	va_start(args,ui);

	row = verticalMargin(ui.verticalMargin,window);
	if(ui.verticalMargin > 0) {
		row = window.top + ui.verticalMargin;
	} else if (ui.verticalMargin < 0) {
		row = window.bot + ui.verticalMargin;
	}

	if(ui.alignment == TEXT_ALIGN_LEFT) {
		col = window.left;
	} else if (ui.alignment == TEXT_ALIGN_CENTER) {
		col = (window.right - window.left - len) / 2;
	} else if (ui.alignment == TEXT_ALIGN_RIGHT) {
		col = window.right - len;
	}

	xt_par2(XT_SET_ROW_COL_POS,row,col);
	xt_par0(ui.color);
	vprintf(ui.text,args);
	va_end(args);
	return row;
}

void displayModeBar(int cursorArea,Area window,struct modeBar modeBar){
	int i;
	int totalLength = modeBar.nModes * UI_MODE_LABEL_WIDTH;
	char *next = "%s%s";
	char *format = "%s%-*s";
	char *formatSelected = "[%s]";
	int col = window.top;
	int row = window.left;
	char *output = '\0';
	for(i = 0; i < modeBar.nModes;i++) {
		char *input = modeBar.modes[i].text;

		char * color = getColor(classicModeBar.subject);
		if(isMode(cursorArea,modeBar.modes[i])) {
			color = getColor(classicModeBar.body);
			input = saveFormatted(formatSelected,input);
		}
		input = saveFormatted(format,color,UI_MODE_LABEL_WIDTH,input);
		if(output) {
			output = saveFormatted(next,output,input);
		} else {
			output = saveFormatted(input);
		}
		//message(output);
	}
	/*for(i = 0; i < modeBar.nModes;i++) {
		totalLength += strlen(modeBar.modes[i].text);
		totalLength += 2;
	}*/
	col = (window.right - totalLength) / 2;
	if(col < window.left) {
		col = window.left;
	}
	row = verticalMargin(modeBar.verticalMargin,window);

	xt_par2(XT_SET_ROW_COL_POS,row,col);
	printf(output);
	flushPool();

}
// display utilities
int isMode(int mode,struct modeText mt) {
	int ans = 0;
	int i = 0;
	for(i = 0;i < mt.nModes;i++) {
		if(mt.mode[i] == mode) {
			ans = 1;
		}
	}
	return ans;
}
int verticalMargin(int marg, Area ar) {
	return margin(marg,ar.top,ar.bot);
}
int horizontalMargin(int marg, Area ar) {
	return margin(marg,ar.left,ar.right);
}
int margin(int marg, int low, int high) {
	if(marg < 0) {
		return high + marg;
	} else {
		return low + marg;
	}
}

//Mode Functions
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
			selectRecord((*hovered)->num);
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

void shiftLeft(char *str) {
	while(*str && *(str+1)) {
		*str = *(str+1);
		str++;
	}
	*str = '\0';
} 
void shiftRight(char newChar,char *s) {
	char nextChar;
	while(*s) {
		nextChar = *s;
		*(s++) = newChar;
		newChar = nextChar;
	}
	*s = newChar;
	*(s+1) = '\0';
}

void edit(char *str,int maxLength,Cursor *cursor,char c) {
	int strLen = strlen(str);
	if(cursor->x >strLen) {
		cursor->x = strLen;
	}
	if(c == KEY_LEFT){
		if(cursor->x > 0) cursor->x--;
	}
	if (c == KEY_RIGHT){
		if(cursor->x < maxLength && cursor->x < strLen)
			cursor->x++;
	}
	if (c==KEY_BACKSPACE) {
		//str[cursor->x-1] = (str[cursor->x] && str[cursor->x] != ' ')?' ':'\0';
		//cursor->x--;
		printf("%c",'\b');
		if(cursor->x > 0) {
			(cursor->x)--;
			shiftLeft(str + cursor->x);
		}
	}
	//to do: check if c a letter
	if(c >= ' ' && c <= '~') {
		if(cursor->x <= maxLength) {	
			//str[cursor->x++] = c; 
			shiftRight(c,str + cursor->x++);
			if(cursor->x > maxLength) {
				cursor->x = maxLength;
			}
		}
	}

}

int modeCheck(int c,int recordSelected,int *cursorLeft,int *cursorArea, char *subject,Cursor *cursor,Area rArea,Record **hovered,struct RecordList **activeBuffer,  struct RecordList *RLBuffer) {
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
			if(*hovered) redraw = init_edit(recordSelected,cursorLeft,cursorArea,cursor,rArea,*hovered,*activeBuffer);
		} else if(KEY_MODE_DELETE(c)) {
			if(*hovered) redraw = init_delete(recordSelected,cursorArea,*hovered,*activeBuffer);
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
	fill(subject,30,'\0');
	fill(body,140,'\0');

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
		selectRecord(hovered->num);
	}
	*cursorLeft = RECORD_NUM_SPACE +1;
	fill(subject,30,'\0');
	fill(body,140,'\0');

	cursor->x = strlen(hovered->subject);
	cursor->y = getRecordY(hovered,activeBuffer,rArea);
	return TRUE;
}
int init_delete(int recordSelected,int *cursorArea,Record *hovered,struct RecordList *activeBuffer) {
	*cursorArea = UI_AREA_DELETE;
	setColor(R_COLOR_SCHEME_CLASSIC_DELETE);
	if(hovered->num != recordSelected) {
		selectRecord(hovered->num);
	}
	message(UI_DELETE_CONFIRM(hovered));
	return TRUE;
}
int commandMode(int cursorArea) {
	return UI_COMMANDABLE(cursorArea);
}