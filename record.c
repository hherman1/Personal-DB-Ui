#include "myui2.h"
#include "record.h"

//--------------------------Display----------------------------
void displayRecords(struct RecordList buffer,Area rArea) {
	Record *temp = buffer.top;
	int row = rArea.top;
	//printf("TOP: %s\n",temp->subject);
	while(temp != NULL && row < rArea.bot){
		DisplayAt(row,rArea.left,XT_CH_GREEN,MAX_SUBJECT_LEN,temp->subject);
		DisplayAt(row++,rArea.right-MAX_TIME_LEN,XT_CH_GREEN,MAX_BODY_LEN,temp->time); //increment row, don't forget
		temp = temp->next;
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
	if(buffer->bottom != NULL) {
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
	buffer->top = r;
	buffer->bottom = buffer->bottom->prev;
	freeRecord(buffer->bottom->next);
	}else { printf("%s\n", "can't shiftBufferUp");}
}
void addBufferBot(struct RecordList *buffer,Record *r){ 
	bufferRecord(buffer,r);
	buffer->top = buffer->top->next;
	freeRecord(buffer->top->prev);
}
