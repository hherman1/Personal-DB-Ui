#include "myui2.h"
#include "record.h"


char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte
struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;
int nitems = 0;  //numRecords in mystore

MultiBodyRecordList buffer;

//------for MultiBodyRecordList

//---hunter
int recordSelected = 2;

//--------------------------Display----------------------------
void displayRecords(Record hovered,struct RecordList *buffer,Area rArea) {
	int i = 0;
	int width = rArea.right - rArea.left;
	char *color;
	Record *temp = buffer->top;
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
			DisplayAt(row,rArea.left,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
			char *sTemp = malloc(141 * sizeof(char));
			strcpy(sTemp,temp->body);
			int spaceNeeded = strlen(sTemp) / width + 1;
			i += spaceNeeded;
			if(i + rArea.top < rArea.bot) {
				xt_par2(XT_SET_ROW_COL_POS,row,rArea.left);
				xt_par0(R_SELECTED_BODY_STYLE);
				xt_par0(R_SELECTED_BG_COLOR);
				xt_par0(R_SELECTED_BODY_COLOR);
				wrapText(width,sTemp);
			}
			free(sTemp);
			xt_par0(XT_CH_NORMAL);
		}
		else if(i + rArea.top < rArea.bot) {
			DisplayAt(row,rArea.left,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
		}
		temp = temp->next;
		xt_par0(XT_BG_DEFAULT);
	}
	
	while(temp != NULL){
		if(temp == buffer->bot) {
			temp = temp->next;
			buffer->bot = buffer->bot->prev;	//CAUTION:: MAY NOT WORK IF bot HAS NO PREVIOUS/ POSSIBLE SCENARIO
			freeRecord(buffer->bot->next);
		}
		else if(temp != buffer->bot && temp != buffer->top) {
			temp = temp->prev;
			freeRecord(temp->next);
			temp = temp->next;
		}
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

//----within record operations-------------------

void addBody(BodyList *bodies, int newBody, char* time){
	//to do: dyanmatic arraylist of integers 

}
//---------------------------------------------------
void loadRecords() { //first function to be called in this process
	//new buffer
	if(buffer = malloc(sizeof MultiBodyRecordList) != NULL || failToMalloc());
	buffer.top = buffer.bot = NULL;
	buffer.numSubjects = 0; 
	//buffer.processedSubjects = malloc(5 * sizeof(int));
	buffer.nextRecordLoc = 1; 


	ParseStat();
	nitems = atoi(searchNvs("nitems"));
	while(buffer.nextRecordLoc++ <= maxItems) {
		loadNextSubject();
	}
	
}

void loadNextSubject(void){
	parseRecord(buffer.nextRecordLoc);
	char *subject = searchNvs("subject");
	int itemNum = atoi(searchNvs("item"));
	newRecord(subject);
	//search for identical subjects

	char *body = searchNvs("body");
	buffer.numSubjects++;
}


MultiBodyRecord newRecord(char* subject){
	if(buffer.top == NULL){
		if(buffer.top = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(buffer.top.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		buffer.top.subject = subject;
		buffer.top.next = buffer.bot;
		buffer.top.prev = NULL;
	}else if(buffer.bot == NULL){
		if(buffer.bot = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(buffer.bot.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		buffer.bot.subject = subject;
		buffer.bot.prev = buffer.top;
		buffer.bot.next = NULL;
	}else{
		MultiBodyRecord temp;
		if(temp = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(buffer.temp.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		buffer.temp.subject = subject;
		buffer.bot.next = temp;
		buffer.temp.prev = buffer.bot;
		buffer.temp.next = NULL;
		buffer.bot = temp;
		
	}

}

int failToMalloc(void){
	printf("%s\n", "A buffer that a NULL. Fail to malloc?");
	exit(EXIT_FAILURE);
	return 0;
}
//---------------------------------parse inputs--------------------------
void ParseStat(void){
	ReadMystoreFromChild("stat",NULL,NULL,NULL);
	ParseInput(input,n_input);
}

void ParseRecord(int numRec){
	char str[15];
	sprintf(str, "%d", numRec);
	ReadMystoreFromChild("display",str,NULL,NULL);
	ParseInput(input,n_input);
}

char *searchNvs(char name[]){
	int i;
	for (i = 0; i < n_nvs; ++i) {
		if (strcmp(nvs[i].name,name) == 0) {
			return nvs[i].value;
		}
	}
}

//--------------------------///subject to deletion!!!!!!!!
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
		printf("ERROR: Record #%i could not be found in the buffer starting at %i and ending at %i\n",num,buffer->top->num,buffer->bot->num);
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
	else if(buffer->bot != NULL) {
		r->prev = buffer->bot;
		buffer->bot->next = r;
		buffer->bot = buffer->bot->next;
	} else {
		buffer->bot = r;
		buffer->top = r;
	}
}

//must parseRecord for this
void addBufferTop(struct RecordList *buffer,Record *r) { //r == new record
	if(buffer->bot != NULL) {
		r->next = buffer->top;
		buffer->top->prev = r;
		buffer->top = r;
		buffer->bot = buffer->bot->prev;
		if(buffer->bot->next != NULL){
			freeRecord(buffer->bot->next);
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
	return buffer.bot->num - buffer.top->num;
}
