#include "myui2.h"
#include "record.h"

struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;

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
	{5,1,XT_CH_YELLOW,"Search: _________________________________"},
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
struct RecordList searchView;
struct RecordList RLBuffer;
Record *hovered;

int nitems = 0;  //numRecords
char subject[MAX_SUBJECT_LEN+1]; //used for new sub and new body additions
char body[MAX_BODY_LEN+1];
char errmsg[ERROR_MESSAGE_BUFFER_LENGTH] = "";

char *cursorArea = "record"; // what are the cursor is at
 				//title, record,addSubject, message, ....
int cursorPos = 0; // cuurently only for adding subject and body
int boolShowCurrentRecord = FALSE;

// ------------------------------------------------ main --------------------
int main(void) {
	int c;
	
	fill(subject,30,'\0');
	fill(body,140,'\0');
	
	xt_par0(XT_CLEAR_SCREEN);
	

	loadRecords(&RLBuffer,1,MAX_RECORDS_TO_DISPLAY + 1,nitems);
	
	hovered = RLBuffer.top;
	
	draw();
	//records operation
	
	//current->prev = '\0';
	/*
	while (TRUE) {
		int redraw = FALSE;
		DUMP = FALSE;
		while ((c = getkey()) == KEY_NOTHING) ;

		if (c == KEY_F9 || c == 'q')  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			getkey_terminate();
			exit(0);
		}
		if (DEBUG && c == '=' || c == '-') {
			int change = 0;
			if(c == '=')
				change++;
			else
				change--;
			rArea.bot += change;
			redraw = TRUE;
		}
		if(DEBUG && c == 'r') {
			message("refreshed | botom: %i | hovered->prev:%p",RLBuffer.bottom->num,hovered->prev);
			message("| added in second call");
			redraw=TRUE; 
		}
		if(DEBUG && c == 'p') {
			DUMP = TRUE;
			redraw = TRUE;
			message("CORE DUMP: ");
		}
		if(cursorArea == "record"){
			if (c == KEY_ENTER) {
                                selectRecord(*hovered,RLBuffer,rArea);
                                redraw = TRUE;
                        }
                        if (c == KEY_DOWN) {
                                if(hovered->next) {
                                        hovered = hovered->next;
                                } else if (hovered->num != RLBuffer.srclength){
                                        scrollDown(&RLBuffer);
                                        hovered = RLBuffer.bottom;
                                }
                                redraw = TRUE;
                        } 
                        if (c == KEY_UP) {
                                if(hovered->prev != NULL) {
                                        hovered = hovered->prev;
                                } else {
                                        scrollUp(&RLBuffer);
                                        hovered = RLBuffer.top;
                                }
                                redraw = TRUE;
                        }
                        else if (c == 'a') {
                                cursorArea = "addSubject";
                        }
		}
		else if (cursorArea == "addSubject" || cursorArea == "addBody"){
			if (c == KEY_F2){
				cursorArea = "record";
			}else {
				if(c == KEY_LEFT){
					if(cursorPos > 0) cursorPos--;
				}
				if (c == KEY_RIGHT){
					cursorPos++;
				}
				if (c == KEY_ENTER){
					if (cursorArea == "addSubject"){
						cursorArea = "addBody";
						cursorPos = 0;
					}else if (cursorArea = "addBody"){
						cursorArea = "record";
						addRecord(subject,body);
						fill(subject,30,'\0');
						fill(body,140,'\0');
						cursorPos = 0;
					}
				}
				//to do: check if c a letter
				if(c >= ' ' && c <= '~') {
					if (cursorArea == "addSubject" && cursorPos <= MAX_SUBJECT_LEN){
						if(cursorPos > MAX_SUBJECT_LEN) cursorPos = 0;
						subject[cursorPos++] = c; 
					}else if (cursorArea == "addBody" && cursorPos <= MAX_BODY_LEN){ 
						if(cursorPos > MAX_BODY_LEN) cursorPos = 0;
						body[cursorPos++] = c; 
					}
				}
			}
			redraw = TRUE;
		}
		if(redraw)
			draw();
	}
	*/
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
	//new subject and body
	DisplayAt(newSubjectArea.top,newSubjectArea.left,XT_CH_CYAN,MAX_SUBJECT_LEN,subject);
	DisplayAt(newBodyArea.top,newBodyArea.left,XT_CH_WHITE,MAX_BODY_LEN,body);
	nitems = atoi(searchNvs("nitems"));
	RLBuffer.srclength = nitems;
	ParseSearch("ho",&searchBuffer);
	Record *current = searchBuffer.top;
	for(i = 0; i < searchBuffer.srclength; i++) {
		printf("S: %s\n",current->subject);
		current = current->next;
	}
	//displayRecords(*hovered,&searchBuffer,rArea);
	DisplayAt(51,0,XT_CH_DEFAULT,strlen(errmsg),errmsg);
	fill(errmsg,ERROR_MESSAGE_BUFFER_LENGTH,'\0');
}

// ------------------------------------ fill --------------------------------
void fill(char *s, int n, char c) {
	while (n--) *s++=c;
	*s='\0';
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
	int len = 0;
	while(n_nvs) {
		len++;
		Record *result = popRecord();
		bufferRecord(sBuffer,result);
	}
	sBuffer->srclength = len;
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
//------------------------ errors -----------------------------------------
void message(char *msg, ...) {
	int max = ERROR_MESSAGE_BUFFER_LENGTH;
	while(errmsg[ERROR_MESSAGE_BUFFER_LENGTH - max] && max--);
	va_list args;
	va_start(args,msg);
	vsnprintf(errmsg + ERROR_MESSAGE_BUFFER_LENGTH - max,max,msg,args);
	va_end(args);
}
