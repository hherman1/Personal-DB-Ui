#include "myui2.h"
#include "record.h"
#include "ui.h"
#include "bindings.h"

struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;
extern int recordSelected;
char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte

Area rArea = {1,120,7,7 + MAX_RECORDS_TO_DISPLAY - 1,"",TRUE};
Area SCREEN = {1,120,1,55,"",TRUE};
Area newSubjectArea = {15,120,44,44,"",TRUE};
Area newBodyArea = {12,120,45,47,"",TRUE};


struct TemplateString TS[] = {
	{1,1,XT_CH_CYAN,"dbname | Max Cards cards | 					FutureDiary				(c) Hunter Herman & Tian Ci Lin"},
	{2,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{3,32,XT_CH_YELLOW,"S: Search	[R: Read]	A: Add		H: Help"},
	{5,1,XT_CH_YELLOW,"Search:"},
	{44,1,XT_CH_YELLOW,"new Subject: "},																				
	{45,1,XT_CH_YELLOW,"new Body: "},
	{48,1,XT_CH_RED,"Note:     F9 to quit"}, //pls save to commit changes.
	{49,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"}//,
	//{50,1,XT_CH_GREEN,"Message: nitems = "}
};
int nTS = sizeof(TS)/sizeof(TS[0]);

struct StringPosition SP[] = {
	{4,10,70,"message"},
	{50,25,3,"nitems"}
};
int nSP = sizeof(SP)/sizeof(SP[0]);

struct RecordList searchBuffer;
struct RecordList RLBuffer;

struct RecordList *activeBuffer;

Record *hovered = NULL;

int nitems = 0;  //numRecords
char subject[MAX_SUBJECT_LEN+1]; //used for new sub and new body additions
char body[MAX_BODY_LEN+1];
char errmsg[ERROR_MESSAGE_BUFFER_LENGTH+1];

char *cursorArea = "record"; // what are the cursor is at
 				//title, record,addSubject, message, ....
Cursor cursor = {0,0};
int cursorLeft = 0;
int boolShowCurrentRecord = FALSE;
void debug(struct RecordList *buffer) {
	return;
}
// ------------------------------------------------ main --------------------
int main(void) {
	int c;
	
	fill(subject,30,'\0');
	fill(body,140,'\0');
	
	xt_par0(XT_CLEAR_SCREEN);
	ParseStat();
	SearchDisplay("nitems","nitems",XT_CH_WHITE);
	nitems = atoi(searchNvs("nitems"));
	

	loadRecords(&RLBuffer,1,MAX_RECORDS_TO_DISPLAY + 1,nitems);
	RLBuffer.lengthfrombot = nitems - RLBuffer.bottom->num;
	//ParseSearch("te",&searchBuffer);
	activeBuffer = &RLBuffer;
	//ParseSearch("hun",activeBuffer);
	hovered = RLBuffer.top;

	draw();
	while (TRUE) {

		int redraw = FALSE;
		DUMP = FALSE;
		while ((c = getkey()) == KEY_NOTHING) ;

		redraw = TRUE;

		if (c == KEY_F9 || c == 'q')  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			getkey_terminate();
			exit(0);
		}

		if(!strcmp(cursorArea,"record")){
			if (DEBUG && c == '=' || c == '-') {
				int change = 0;
				if(c == '=')
					change++;
				else
					change--;
				rArea.bot += change;
				redraw = TRUE;
			}
			redraw = scroll(&hovered,&activeBuffer,c) || redraw;
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;

		}
		else if (!strcmp(cursorArea,"addSubject") || !strcmp(cursorArea,"addBody")){
			cursor.y = newSubjectArea.top + (!strcmp(cursorArea,"addBody"));
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;
			if (!strcmp(cursorArea,"addSubject")){
				if (c == KEY_ENTER){
					cursorArea = "addBody";
					cursor.x = 0;
					cursor.y ++;
				}
				else {
					edit(subject,MAX_BODY_LEN,&cursor,c);
				}
			} else if (!strcmp(cursorArea,"addBody")){
				if (c == KEY_ENTER){
					cursorArea = "record";
					addRecord(subject,body);
					RLBuffer.lengthfrombot++;
					fill(subject,30,'\0');
					fill(body,140,'\0');
					cursor.x = 0;
					cursor.y = 0;
				}
				else {
					edit(body,MAX_BODY_LEN,&cursor,c);
				}
			}
			redraw = TRUE;
		} else if (!strcmp(cursorArea,"search")) {
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;
			if (c == KEY_ENTER){
				cursorArea = "record";
				if(searchBuffer.top) freeBuffer(&searchBuffer);
				ParseSearch(subject,&searchBuffer);
				fill(subject,30,'\0');
				cursor.x = 0;
				cursor.y = 0;
				activeBuffer = &searchBuffer;
				hovered = activeBuffer->top;
			}
			else {
				edit(subject,MAX_SUBJECT_LEN,&cursor,c);
			}
			redraw = TRUE;
				
		}

		else if (!strcmp(cursorArea,"editSubject") || !strcmp(cursorArea,"editBody")){
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;
			if (!strcmp(cursorArea,"editSubject")){
				if(c == KEY_ENTER) {
					cursorArea = "editBody";
					cursor.x = 0;
					cursor.y ++;
				} else {
					edit(hovered->subject,MAX_SUBJECT_LEN,&cursor,c);
				}
			}else if (!strcmp(cursorArea,"editBody")){
				if(c == KEY_ENTER) {
					cursorArea = "record";
					editRecord(hovered->num,hovered->subject,hovered->body);
					cursor.x = 0;
					cursor.y = 0;
				} else {
					edit(hovered->body,MAX_BODY_LEN,&cursor,c);
				}
			}
			redraw = TRUE;
		}
		
		if(redraw)
			draw();

	}
	
	return 1;
}
// -------------------------------------draw---------------------------------
void draw() {
	printf("redrawing");

	int i = 0;
	xt_par0(XT_CLEAR_SCREEN);
	
	// display template
 	for (i = 0; i < nTS; ++i) {
		xt_par2(XT_SET_ROW_COL_POS,TS[i].row,TS[i].col);
		xt_par0(XT_CH_DEFAULT);
		xt_par0(TS[i].color);
		printf("%s",TS[i].string);
	}
	//perform operations on stat
	ParseStat();
	SearchDisplay("nitems","nitems",XT_CH_WHITE);
	nitems = atoi(searchNvs("nitems"));
	//new subject and body

	if(!strcmp(cursorArea,"addBody") || !strcmp(cursorArea,"addSubject")){
		DisplayAt(newSubjectArea.top,ENTRY_FIELD_LABEL_SPACE,XT_CH_CYAN,MAX_SUBJECT_LEN,subject);
		DisplayAt(newBodyArea.top,ENTRY_FIELD_LABEL_SPACE,XT_CH_WHITE,MAX_BODY_LEN,body);
		cursorLeft = ENTRY_FIELD_LABEL_SPACE;
	} else if (strcmp(cursorArea,"search") == 0) {
		cursorLeft = ENTRY_FIELD_LABEL_SPACE;
		DisplayAt(5,ENTRY_FIELD_LABEL_SPACE,XT_CH_DEFAULT,MAX_SUBJECT_LEN,subject);
	} else if (strcmp(cursorArea,"edit") == 0) {
		cursorLeft = RECORD_NUM_SPACE;
	}
	//buffer

	if(hovered) {
		displayRecords(*hovered,activeBuffer,rArea);
	}

	DisplayAt(51,0,XT_CH_DEFAULT,strlen(errmsg),errmsg);
	fill(errmsg,ERROR_MESSAGE_BUFFER_LENGTH,'\0');
	
	xt_par2(XT_SET_ROW_COL_POS,cursor.y,cursor.x + cursorLeft);
}

// ------------------------------------ fill --------------------------------
void fill(char *s, int n, char c) {
	printf("[Done%s]\n",s);
	while (n--) *s++=c;
	*s='\0';		
	printf("Done%s\n",s);

	printf("Done%s\n",s);

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
void ParseSearch(char *search,struct RecordList *sBuffer) {
	ReadMystoreFromChild("search",search,NULL,NULL);
	ParseInput(input,n_input);
	struct NameValue *startPos = nvs;
	while(n_nvs) {
		Record *result = popRecord();
		bufferRecord(sBuffer,result);
	}
	nvs = startPos;
	sBuffer->lengthfrombot = 0;
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
	char *ans = NULL;
	for (i = 0; i < n_nvs; ++i) {
		if (strcmp(nvs[i].name,name) == 0) {
			ans = nvs[i].value;
			break;
		}
	}
	return ans;
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
		if (++col == SCREEN.right) {
			xt_par2(XT_SET_ROW_COL_POS,row+1,1);
			col = 1;
		}
	}
	fflush(stdout);
}

//RLBuffer must exist for scroll
// ---------------------------------- 	FindStringPosition ----------------
int FindStringPosition(char *prompt) { //pos in string array 
	int i;
	
	for (i = 0; i < nSP; ++i) {
		if (strcmp(prompt,SP[i].name) == 0)
			return i;
	}
	return 0;
}
//------------------------ adding ----------------------------------------
void addRecord(char *subject, char* body){
	ReadMystoreFromChild("add",subject,body,NULL);
	ParseRecord(++nitems);
}
//------------------------ editing----------------------------------------
void editRecord(int num,char *subject, char* body){
	char sNum[15];
	sNum[14] = '\0';
	sprintf(sNum,"%d",num);
	ReadMystoreFromChild("edit",sNum,subject,body);
	//ParseRecord(nitems);
}
//------------------------ errors -----------------------------------------
void message(char *msg, ...) {
	int max = ERROR_MESSAGE_BUFFER_LENGTH;
	while(errmsg[ERROR_MESSAGE_BUFFER_LENGTH - max] && max--);
	va_list args;
	va_start(args,msg);
	vsnprintf(errmsg + ERROR_MESSAGE_BUFFER_LENGTH - max,max,msg,args);
	va_end(args);
}
