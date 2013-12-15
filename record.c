#include "myui2.h"
#include "record.h"

//--------------------------Display----------------------------

//--------------------------Operations-------------------------
void loadRecords(struct RecordList *buffer,int low,int high,int number) {
	ParseRecord(low);
	Record *temp = malloc(sizeof(Record));
	getRecord(temp);
	bufferRecord(buffer,temp);
	while(low++ < high && low< number) {
		ParseRecord(low);
		temp = malloc(sizeof(Record));
		getRecord(temp);
		bufferRecord(buffer,temp);
	}	
	
}
void getRecord(Record *holder) {
	char *temp;
	char *temp2 = malloc(sizeof(char) * 141);
	temp = searchNvs("subject");
	strcpy(temp2,temp);
	holder->subject = temp2;
	
	temp = searchNvs("body");
	temp2 = malloc(sizeof(char) * 141);
	strcpy(temp2,temp);
	holder->body = temp2;
	
	temp = searchNvs("time");
	temp2 = malloc(sizeof(char) * 141);
	strcpy(temp2,temp);
	holder->time = temp2;
	
	holder->num = atoi(searchNvs("item"));
	holder->prev = NULL;
	holder->next = NULL;
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
	target->prev->next = target->next;
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
	if(buffer->bottom != NULL) {
		r->prev = buffer->bottom;
		buffer->bottom->next = r;
		buffer->bottom = r;
	} else {
		buffer->bottom = r;
		buffer->top = r;
	}
}

