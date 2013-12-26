#include "myui2.h"
#include "record.h"

int recordSelected = 2;
//--------------------------Display----------------------------
void displayRecords(Record hovered,struct RecordList *buffer,Area rArea) {
	int i = 0;
	int width = rArea.right - rArea.left;
	char *color;
	
	Record *temp = buffer->top;
	if(temp != NULL) {
		printf("starting cleanup\n");
		Record *bot = fillBufferForArea(buffer,rArea);
		printf("trimming buffer\n");
		trimBuffer(buffer,bot);
		printf("clean\n");
	}
	while(temp){// && i + rArea.top < rArea.bot){
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
			DisplayAt(row,rArea.left,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
			int spaceNeeded = requiredSpace(*temp,width);//strlen(sTemp) / width + 1;
			i += spaceNeeded;
			if(i + rArea.top < rArea.bot) {
				xt_par2(XT_SET_ROW_COL_POS,row,rArea.left);
				xt_par0(R_SELECTED_BODY_STYLE);
				xt_par0(R_SELECTED_BG_COLOR);
				xt_par0(R_SELECTED_BODY_COLOR);
				wrapText(width,temp->body);
			}
			xt_par0(XT_CH_NORMAL);
		}
		else if(row <= rArea.bot) {
			DisplayAt(row,rArea.left,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
		}
		temp = temp->next;
		xt_par0(XT_BG_DEFAULT);
		i++;
	}
	/*
	while(temp != NULL){
		if(temp == buffer->bottom) {
			temp = temp->next;
			buffer->bottom = buffer->bottom->prev;	//CAUTION:: MAY NOT WORK IF BOTTOM HAS NO PREVIOUS/ POSSIBLE SCENARIO
			freeRecord(buffer->bottom->next);
		}
		else if(temp != buffer->bottom && temp != buffer->top) {
			temp = temp->prev;
			freeRecord(temp->next);
			temp = temp->next;
		}
	}*/
}
void trimBuffer(struct RecordList *buffer, Record *newBot) {
	buffer->bottom = newBot;
	while(newBot->next != NULL) {
		freeRecord(newBot->next);
	}
}
Record* fillBufferForArea(struct RecordList *buffer, Area rArea) {
	int availableSpace = rArea.bot - rArea.top;
	Record *current = buffer->top;
	while(availableSpace && current->next) {
		if(current->num == recordSelected){
			int openSpace = requiredSpace(*current,rArea.right - rArea.left);
			availableSpace -= openSpace;
		}
		availableSpace--;
		current = current->next;
	}
	printf("refilling\n");
	while(availableSpace-- > 0) {
		printf("about to access\n");
		Record *temp = getRecord(current->num + 1);
		printf("retrieved new record\n");
		if(temp) {
			printf("prepare for insertion\n");
			if(temp->num == recordSelected){
				printf("inserting a selected record\n");
				int openSpace = requiredSpace(*temp,rArea.right - rArea.left);
				availableSpace -= openSpace;
			}
			if(availableSpace >= 0) {
				bufferRecord(buffer,temp);
				current = temp; /// POSIBLY ERROR: not sure if bufferRecord updates the proper memory addresses w/ refs
			}
		}
		else {
			availableSpace = 0;
		}
	}
	return current;
}
int requiredSpace(Record r,int width) {
	return strlen(r.body) / (width) + 1;
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
	
	Record *ans = NULL;
	char *temp;
	temp = searchNvs("subject");
		printf("working on %p\n",temp);
	if(temp) {
		printf("working in %s\n",temp);
		
		ans = malloc(sizeof(Record));
		ans->subject = malloc(sizeof(char) * 31);
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
	}
	printf("working\n");
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
void addBufferTop(struct RecordList *buffer,Record *r) { //r == new record
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
	if(buffer->top->prev != NULL){
		freeRecord(buffer->top->prev);
	}
}
int bufferLength(struct RecordList buffer) {
	return buffer.bottom->num - buffer.top->num;
}
