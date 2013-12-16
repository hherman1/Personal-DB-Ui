#include "myui2.h"
#include "record.h"

int recordSelected = 2;
//--------------------------Display----------------------------
void displayRecords(Record hovered,struct RecordList buffer,Area rArea) {
	int i = 0;
	int width = rArea.right - rArea.left;
	char *color;
	Record *temp = buffer.top;
	while(temp != NULL && i + rArea.top < rArea.bot){
		i++;
		color = R_TEXT_COLOR;
		int row = i + rArea.top;
		if(temp->num == hovered.num) {
			xt_par0(R_HOVERED_BG_COLOR);
			color = R_HOVERED_TEXT_COLOR;
		}
		if(temp->num == recordSelected) {
			xt_par0(R_SELECTED_BG_COLOR);
			xt_par0(R_SELECTED_TEXT_STYLE);
			color = R_SELECTED_TEXT_COLOR;
			if(temp->num == hovered.num) {
				color = R_SELECTED_HOVERED_TEXT_COLOR;
			}
			DisplayAt(row,rArea.left,color,30,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,30,temp->time);
			char *sTemp = malloc(141 * sizeof(char));
			strcpy(sTemp,temp->body);
			int spaceNeeded = strlen(sTemp) / width + 1;
			i += spaceNeeded;
			xt_par2(XT_SET_COL_POS,row+2,rArea.left);
			xt_par0(R_SELECTED_BODY_STYLE);
			xt_par0(R_SELECTED_BG_COLOR);
			xt_par0(R_SELECTED_BODY_COLOR);
			wrapText(width,sTemp);
			free(sTemp);
			xt_par0(XT_CH_NORMAL);
		}
		else {
			DisplayAt(row,rArea.left,color,30,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,30,temp->time);
		}
		temp = temp->next;
		xt_par0(XT_BG_DEFAULT);
	}
}
void wrapText(int width, char *text) {
	int i = 0;
	while(text[i] != '\0') {
		if(i%width == 0) {
			putchar('\n');
		}
		putchar(text[i]);
		i++;
	}
	
}
void selectRecord(Record record,struct RecordList buffer,Area rArea) {
	int rNum = record.num;
	if(rNum == recordSelected) {
		recordSelected = 0;
	} else {
		recordSelected = rNum;
	}
}
//--------------------------Operations-------------------------
void loadRecords(struct RecordList *buffer,int low,int high,int number) {
	Record *temp = getRecord(low);
	bufferRecord(buffer,temp);
	while(++low < high && low <= number) {
		temp = getRecord(low);
		bufferRecord(buffer,temp);
	}	
	
}
Record *getRecord(int r) {
	ParseRecord(r);
	Record *ans = malloc(sizeof(Record));
	char *temp;
	ans->subject = malloc(sizeof(char) * 31);
	temp = searchNvs("subject");
	strcpy(ans->subject,temp);
	
	temp = searchNvs("body");
	ans->body = malloc(sizeof(char) * 141);
	strcpy(ans->body,temp);
	
	temp = searchNvs("time");
	ans->time = malloc(sizeof(char) * 141);
	strcpy(ans->time,temp);
	
	ans->num = atoi(searchNvs("item"));
	ans->prev = NULL;
	ans->next = NULL;
	return ans;
}
Record *findRecord(struct RecordList *buffer,int num) {
	Record *cur = buffer->top;
	while(cur->num != num && (cur = cur->next) && cur != NULL);
	if(cur == NULL) {
		printf("ERROR: Record #%i could not be found in the buffer starting at %i and ending at %i\n",num,buffer->top->num,buffer->bottom->num);
		exit(EXIT_FAILURE);
	}
	return cur;
}
void freeRecord(Record *target) {
	free(target->time);
	free(target->body);
	free(target->subject);
	if(target->prev != NULL) {
		target->prev->next = target->next;
	}
	if(target->next != NULL) {
		target->next->prev = target->prev;
	}
	free(target);
}
void recordcpy(Record *dest,Record src) {
	dest->body = src.body;
	dest->subject = src.subject;
	dest->time = src.time;
	dest->num = src.num;
	dest->next = src.next;
	dest->prev = src.prev;
}
void bufferRecord(struct RecordList *buffer,Record *r) {
	if(r == NULL) {
		printf("ERROR: Record does not exist\n");
	}
	else if(buffer->bottom != NULL) {
		r->prev = buffer->bottom;
		buffer->bottom->next = r;
		buffer->bottom = buffer->bottom->next;
	} else {
		buffer->bottom = r;
		buffer->top = r;
	}
}

//must parseRecord for this
void addBufferTop(struct RecordList *buffer,Record *r){ //r = new record
	if(buffer->bottom != NULL) {
		r->next = buffer->top;
		buffer->top->prev = r;
		buffer->top = r;
		buffer->bottom = buffer->bottom->prev;
		if(buffer->bottom->next != NULL){
			freeRecord(buffer->bottom->next);
		}
	}else { 
		printf("%s\n", "can't shiftBufferUp");
	}
}
void addBufferBot(struct RecordList *buffer,Record *r){ 
	bufferRecord(buffer,r);
	buffer->top = buffer->top->next;
	printf("SCROLLING\n");
	if(buffer->top->prev != NULL){
		freeRecord(buffer->top->prev);
	}
}
