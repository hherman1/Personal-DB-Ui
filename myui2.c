#include "myui2.h"

struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;

char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte

Area SCREEN = {1,120,1,55};
Area rArea = {1,120,7,45};
																																	
struct TemplateString TS[] = {
	{1,1,XT_CH_CYAN,"dbname | Max Cards cards | 					FutureDiary				(c) Hunter Herman & Tian Ci Lin"},
	{2,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{3,32,XT_CH_YELLOW,"S: Search	[R: Read]	A: Add		H: Help"},
	{5,1,XT_CH_YELLOW,"Search: _________________________________"},
	{48,1,XT_CH_RED,"Note: pls save to commit changes"},
	{49,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{50,1,XT_CH_GREEN,"Message: nitems = "}
};
int nTS = sizeof(TS)/sizeof(TS[0]);

struct StringPosition SP[] = {
	{4,10,70,"message"},
	{50,25,3,"nitems"},
	{8,20,20,"first-record"},
	{9,20,20,"last-record"},
	{10,9,30,"author"},
	{11,10,10,"version"},
	{14,11,3,"record-num"},
	{15,7,20,"time"},
	{16,10,30,"subject"},
	{17,7,140,"body"},
	{21,11,30,"edit-subject"},
	{22,11,140,"edit-body"}
};
int nSP = sizeof(SP)/sizeof(SP[0]);

struct RecordList RLBuffer;

int nitems = 0;  //numRecords
char subject[31];
char body[141];
char errmsg[80] = "";

char *cursorPos; // what are the cursor is at
 				//title, record1, record2, message, ....
int recordDisplayStart = 1; //display records 1-20; start to end 
int recordDisplayEnd = 20;
int boolShowCurrentRecord = FALSE;


// ------------------------------------------------ main --------------------
int main(void) {
	printf("hello world\n");
	
	printf("starting...\n");
	int i, c;
	
	fill(subject,30);
	fill(body,140);
	
	xt_par0(XT_CLEAR_SCREEN);
	printf("testSTetseresrdfas\n");
	
	// display template
 	for (i = 0; i < nTS; ++i) {
		xt_par2(XT_SET_ROW_COL_POS,TS[i].row,TS[i].col);
		xt_par0(XT_CH_DEFAULT);
		xt_par0(TS[i].color);
		printf("%s,%i",TS[i].string,i);
	}
	
	
	//perform operations on stat
	ParseStat();
	//SearchDisplay("nitems","nitems",XT_CH_WHITE);
	nitems = atoi(searchNvs("nitems"));
	//records operation
	ParseRecord(recordDisplayStart);
	Record *temp = malloc(sizeof(Record));
	getRecord(temp);
	bufferRecord(&RLBuffer,temp);
	Record *current = RLBuffer.top;
	//current->prev = '\0';
	for (i = recordDisplayStart+1; i <= recordDisplayEnd && i <= nitems; i++){
		ParseRecord(i);
		temp = malloc(sizeof(Record));
		getRecord(temp);
		bufferRecord(&RLBuffer,temp);
		
		int row = rArea.top + i - recordDisplayStart;
		DisplayAt(row,rArea.left,XT_CH_GREEN,30,temp->subject);
		DisplayAt(row,rArea.right,XT_CH_GREEN,30,temp->time);
	}
	//*/
	while (TRUE) {
		while ((c = getkey()) == KEY_NOTHING) ;
		if (c == KEY_F9)  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			getkey_terminate();
			exit(0);
		}
	}
}
//--------------------------getRecord-------------------------
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

// ------------------------------------ fill --------------------------------
void fill(char *s, int n) {
	while (n--) *s++=' ';
	*s='\0';
}

// ------------------------------------------------ DisplayStat --------------
void DisplayStat(void) {
	ParseStat();
	/*
	SearchDisplay("nitems","nitems",XT_CH_WHITE);
	SearchDisplay("first-record","first-time",XT_CH_WHITE);
	SearchDisplay("last-record","last-time",XT_CH_WHITE);
	SearchDisplay("author","author",XT_CH_WHITE);
	SearchDisplay("version","version",XT_CH_WHITE);
	*/
	fflush(stdout);
	//SearchDisplay("message",errmsg,XT_CH_RED);
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
// --------------------------- searching-----------------------------------
void SearchDisplay(char *prompt, char *name, char *color) {
	int loc, col, i, j;
	int instring = TRUE;
	char *value;
	
	// search for location
	loc = FindStringPosition(prompt);
	col = SP[loc].col;
	
	// search for value
	value = searchNvs(name);
	
	DisplayAt(SP[loc].row,SP[loc].col,XT_CH_WHITE,SP[loc].length,value);
}

char *searchNvs(char name[]){
	int i;
	for (i = 0; i < n_nvs; ++i) {
		if (strcmp(nvs[i].name,name) == 0) {
			return nvs[i].value;
		}
	}
}

// ------------------------------------- DisplayAt -------------------------
void DisplayAt(int row, int col, char *color, int maxlength, char *value) {
	int i, instring = TRUE;
	
	xt_par2(XT_SET_ROW_COL_POS,row,col);
	xt_par0(color);
	for (i = 0; i < maxlength; ++i) {
		if (value[i] == '\0') 
			instring = FALSE;
		printf("%c",instring?value[i]:' ');
		if (++col == 100) {
			xt_par2(XT_SET_ROW_COL_POS,row+1,1);
			col = 1;
		}
	}
	fflush(stdout);
}

// ---------------------------------- 	FindStringPosition ----------------
int FindStringPosition(char *prompt) { //pos in string array 
	int i;
	
	for (i = 0; i < nSP; ++i) {
		if (strcmp(prompt,SP[i].name) == 0)
			return i;
	}
	return 0;
}
