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
		Record *bot = adjustBufferForArea(buffer,rArea);
		trimBuffer(buffer,bot);
	}
	temp = buffer->top;
	while(temp){// && i + rArea.top < rArea.bot){
		color = R_TEXT_COLOR;
		int row = i + rArea.top;
		char *num = malloc(RECORD_NUM_SPACE * sizeof(char));
		sprintf(num,"%i.",temp->num);
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
			DisplayAt(row,rArea.left,color,RECORD_NUM_SPACE,num);
			DisplayAt(row,rArea.left+RECORD_NUM_SPACE,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
			int spaceNeeded = requiredSpace(*temp,width);//strlen(sTemp) / width + 1;
			i += spaceNeeded;
				xt_par2(XT_SET_ROW_COL_POS,row,rArea.left);
				xt_par0(R_SELECTED_BODY_STYLE);
				xt_par0(R_SELECTED_BG_COLOR);
				xt_par0(R_SELECTED_BODY_COLOR);
				wrapText(RECORD_NUM_SPACE,width,temp->body);
			xt_par0(XT_CH_NORMAL);
		}
		else if(row <= rArea.bot) {
			DisplayAt(row,rArea.left,color,RECORD_NUM_SPACE,num);
			DisplayAt(row,rArea.left+RECORD_NUM_SPACE,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
		}
		free(num);
		temp = temp->next;
		xt_par0(XT_BG_DEFAULT);
		i++;
	}
}
void trimBuffer(struct RecordList *buffer, Record *newBot) {
	buffer->bottom = newBot;
	while(newBot->next != NULL) {
		freeRecord(newBot->next);
	}
}
Record* adjustBufferForArea(struct RecordList *buffer, Area rArea) {
	int availableSpace = rArea.bot - rArea.top;
	Record *current = buffer->top;
	message("RECORD SELECTED: %i",recordSelected);
	char *method;
	if(current->num == recordSelected){ 
		int openSpace = requiredSpace(*current,rArea.right - rArea.left);
		availableSpace -= openSpace;
		message("%s: %i bot: %i","current->next",availableSpace,recordSelected);
	}
	while(availableSpace-- > 0) {
		if(current->next){
			current = current->next;
		}
		else {
			Record *temp = getRecord(current->num + 1);
			if(temp) {
				bufferRecord(buffer,temp);
				current = temp; 
				method = "new record";
			}
			else {
				availableSpace = 0;
			}
		}
		if(current->num == recordSelected){ 
			int openSpace = requiredSpace(*current,rArea.right - rArea.left);
			availableSpace -= openSpace;
			message("%s: %i bot: %i","current->next",availableSpace,recordSelected);
			while(availableSpace < 0) {
				buffer->top = buffer->top->next;
				freeRecord(buffer->top->prev);
				availableSpace++;
			}
		}
	}
	printf("done: %p\n",current);
	availableSpace++;
	return current;
}
int requiredSpace(Record r,int width) {
	return strlen(r.body) / (width) + 1;
}
void wrapText(int left,int width, char *text) {
	int i = 0;
	while(text[i] != '\0') {
		if(i%width == 0) {
			putchar('\n');
			int q = 0;
			while(q++ < left) {
				putchar(' ');
			}
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
	if(temp) {
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
