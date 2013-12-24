#include "myui2.h"
#include "record.h"


//parsing and myui2_util
char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte
struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;
int nitems = 0;  //numRecords in mystore


//------for MBRArray
MultiBodyRecord *MBRArray;
int nextRecordLoc = 1;
int numMBRecords = 0;
//---hunter
int recordSelected = 2;

//--------------------------Display----------------------------
void displayRecords(Record hovered,struct RecordList *MBRArray,Area rArea) {
	int i = 0;
	int width = rArea.right - rArea.left;
	char *color;
	Record *temp = MBRArray->top;
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
		if(temp == MBRArray->bot) {
			temp = temp->next;
			MBRArray->bot = MBRArray->bot->prev;	//CAUTION:: MAY NOT WORK IF bot HAS NO PREVIOUS/ POSSIBLE SCENARIO
			freeRecord(MBRArray->bot->next);
		}
		else if(temp != MBRArray->bot && temp != MBRArray->top) {
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
void selectRecord(Record record,struct RecordList MBRArray,Area rArea) {
	int rNum = record.num;
	if(rNum == recordSelected) {
		recordSelected = 0;
	} else {
		recordSelected = rNum;
	}
}

//----within record operations-------------------

void addBody(BodyList *bodies, int newBody, int itemNum, char* time){
	//to do: linklist 

}
//---------------------------------------------------
void loadRecords() { //first function to be called in this process
	ParseStat();
	nitems = atoi(searchNvs("nitems"));
	//new MBRArray
	if(MBRArray = malloc(sizeof MultiBodyRecord * (nitems + MAX_EXTRA_RECORDS)) != NULL || failToMalloc());
	MBRArray.top = MBRArray.bot = NULL;
	nextRecordLoc = 1; 
	numMBRecords = 0; 
	while(nextRecordLoc++ <= maxItems) {
		loadNextSubject();
	}
}//complete

void loadNextSubject(void){
	parseRecord(nextRecordLoc);
	int itemNum = atoi(searchNvs("item"));
	char *newTime = searchNvs("time");
	char *newSubject = searchNvs("subject");
	char *newBody = searchNvs("body");


	//search for identical subjects
	int i, anySameSubjects = 0;
	for (i = 0; i < numMBRecords; i++){
		if(strcmp(MBRArray[i].subject,newSubject) == 0){
			anySameSubjects = 1;
		}
	} //same subject location = i;
	if(!anySameSubjects){
		//new Record
		MultiBodyRecord temp;
		if( (temp  = MBRArray[numMBRecords] = malloc(sizeof MultiBodyRecord)) != NULL || failToMalloc());
		if( (temp.bodies  = malloc(sizeof BodyList)) != NULL || failToMalloc());
		temp.subject = newSubject;
		temp.bodies.top = temp.bodies.bot = NULL;
		temp.numBodies = 0;
		i = numMBRecords++;
	}//i = same subject location or numMRecord for newrecords;
	addBody(MBRArray[i].bodies, newBody, itemNum, newTime);
	//nextRecordLoc++;
}


MultiBodyRecord newRecord(char* subject){ //subject to deletion
	/*
	if(MBRArray.top == NULL){
		if(MBRArray.top = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(MBRArray.top.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		MBRArray.top.subject = subject;
		MBRArray.top.next = MBRArray.bot;
		MBRArray.top.prev = NULL;
	}else if(MBRArray.bot == NULL){
		if(MBRArray.bot = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(MBRArray.bot.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		MBRArray.bot.subject = subject;
		MBRArray.bot.prev = MBRArray.top;
		MBRArray.bot.next = NULL;
	}else{
		MultiBodyRecord temp;
		if(temp = malloc(sizeof MultiBodyRecord) != NULL || failToMalloc());
		if(MBRArray.temp.bodies = malloc(sizeof BodyList) != NULL || failToMalloc());
		MBRArray.temp.subject = subject;
		MBRArray.bot.next = temp;
		MBRArray.temp.prev = MBRArray.bot;
		MBRArray.temp.next = NULL;
		MBRArray.bot = temp;
		
	}
	*/

}

int failToMalloc(void){
	printf("%s\n", "A MBRArray that a NULL. Fail to malloc?");
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
Record *findRecord(struct RecordList *MBRArray,int num) {
	Record *cur = MBRArray->top;
	while(cur->num != num && (cur = cur->next) && cur != NULL);
	if(cur == NULL) {
		printf("ERROR: Record #%i could not be found in the MBRArray starting at %i and ending at %i\n",num,MBRArray->top->num,MBRArray->bot->num);
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
void MBRArrayRecord(struct RecordList *MBRArray,Record *r) {
	if(r == NULL) {
		printf("ERROR: Record does not exist\n");
	}
	else if(MBRArray->bot != NULL) {
		r->prev = MBRArray->bot;
		MBRArray->bot->next = r;
		MBRArray->bot = MBRArray->bot->next;
	} else {
		MBRArray->bot = r;
		MBRArray->top = r;
	}
}

//must parseRecord for this
void addMBRArrayTop(struct RecordList *MBRArray,Record *r) { //r == new record
	if(MBRArray->bot != NULL) {
		r->next = MBRArray->top;
		MBRArray->top->prev = r;
		MBRArray->top = r;
		MBRArray->bot = MBRArray->bot->prev;
		if(MBRArray->bot->next != NULL){
			freeRecord(MBRArray->bot->next);
		}
	}else { 
		printf("%s\n", "can't shiftMBRArrayUp");
	}
}
void addMBRArrayBot(struct RecordList *MBRArray,Record *r){ 
	MBRArrayRecord(MBRArray,r);
	MBRArray->top = MBRArray->top->next;
	if(MBRArray->top->prev != NULL){
		freeRecord(MBRArray->top->prev);
	}
}
int MBRArrayLength(struct RecordList MBRArray) {
	return MBRArray.bot->num - MBRArray.top->num;
}
