#include "myui2.h"
#include "record.h"

struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;

char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte

Area rArea = {7,8 + MAX_RECORDS_TO_DISPLAY,1,120,"",TRUE};
Area SCREEN = {1,55,1,120,"",TRUE};
Area newSubjectArea = {44,44,15,120,"",TRUE};
Area newBodyArea = {45,47,12,120,"",TRUE};																												
struct TemplateString TS[] = {
	{1,1,XT_CH_CYAN,"dbname | Max Cards cards | 					FutureDiary				(c) Hunter Herman & Tian Ci Lin"},
	{2,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{3,32,XT_CH_YELLOW,"S: Search	[R: Read]	A: Add		H: Help"},
	{5,1,XT_CH_YELLOW,"Search: _________________________________"},
	{44,1,XT_CH_YELLOW,"new Subject: "},																				
	{45,1,XT_CH_YELLOW,"new Body: "},
	{48,1,XT_CH_RED,"Note:     F9 to quit"}, //pls save to commit changes.
	{49,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{50,1,XT_CH_GREEN,"Message: nitems = "}
};
int nTS = sizeof(TS)/sizeof(TS[0]);

struct StringPosition SP[] = {
	{4,10,70,"message"},
	{50,25,3,"nitems"}
};
int nSP = sizeof(SP)/sizeof(SP[0]);

struct RecordList RLBuffer;
Record hovered;

int nitems = 0;  //numRecords
char subject[MAX_SUBJECT_LEN+1]; //used for new sub and new body additions
char body[MAX_BODY_LEN+1];
char errmsg[80] = "";

char *cursorArea = "record"; // what are the cursor is at
 				//title, record,addSubject, message, ....
int cursorPos = 0; // cuurently only for adding subject and body
int boolShowCurrentRecord = FALSE;

// ------------------------------------------------ main --------------------
int main(void) {
	int c;
	
	fill(subject,30);
	fill(body,140);
	
	xt_par0(XT_CLEAR_SCREEN);
	
	draw();

	loadRecords(&RLBuffer,1,MAX_RECORDS_TO_DISPLAY,nitems);
	
	hovered = *(RLBuffer.top);
	
	displayRecords(hovered,RLBuffer,rArea);
	//records operation
	
	//current->prev = '\0';
	while (TRUE) {
		int redraw = FALSE;
		while ((c = getkey()) == KEY_NOTHING) ;

		if (c == KEY_F9 || c == 'q')  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			getkey_terminate();
			exit(0);
		}
		if(cursorArea == "record"){
			if (c == KEY_ENTER) {
                                selectRecord(hovered,RLBuffer,rArea);
                                redraw = TRUE;
                        }
                        if (c == KEY_DOWN) {
                                if(hovered.next != NULL) {
                                        hovered = *(hovered.next);
                                } else {
                                        scrollDown();
                                        hovered = *(RLBuffer.bottom);
                                }
                                redraw = TRUE;
                        } 
                        if (c == KEY_UP) {
                                if(hovered.prev != NULL) {
                                        hovered = *(hovered.prev);
                                } else {
                                        scrollUp();
                                        hovered = *(RLBuffer.top);
                                }
                                redraw = TRUE;
                        }
                        else if (c == 'a') {
                                cursorArea = "addSubject";
                        }
		}
		if (cursorArea == "addSubject" || cursorArea == "addBody"){
			if (c == KEY_F1){
				cursorArea = "record";
			}else {
				if(c == KEY_LEFT){
					if(cursorPos > 0) cursorPos--;

				}
				if (c == KEY_RIGHT){
					cursorPos++;
				}
				if (cursorArea == "addSubject" && cursorPos <= MAX_SUBJECT_LEN){
					if (c == KEY_ENTER){
						cursorArea = "addBody";
					}else{ 
						if(cursorPos > MAX_SUBJECT_LEN) cursorPos = 0;
						subject[cursorPos++] = c; 
					}
				}else if (cursorPos <= MAX_BODY_LEN){ 
					if (c == KEY_ENTER){
						cursorArea = "record";
						addRecord(subject,body);
						fill(subject,30);
						fill(body,140);
						cursorPos = 0;
					}else{ 
						if(cursorPos > MAX_BODY_LEN) cursorPos = 0;
						subject[cursorPos++] = c; 
					}
				}
			}
			redraw = TRUE;
		}
		if(redraw)
			draw();
	}
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
	
	displayRecords(hovered,RLBuffer,rArea);

}
// ------------------------------------ fill --------------------------------
void fill(char *s, int n) {
	while (n--) *s++=' ';
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
		if (++col == SCREEN.right) {
			xt_par2(XT_SET_ROW_COL_POS,row+1,1);
			col = 1;
		}
	}
	fflush(stdout);
}

//RLBuffer must exist for scroll
void scrollUp(){
	int nextRecord = RLBuffer.top->num - 1;
	if(nextRecord >= 1) {
		addBufferTop(&RLBuffer, getRecord(nextRecord),rArea.bot - rArea.top);
	} else {
		printf("bottom\n");
	}
}
void scrollDown(){
	int nextRecord = RLBuffer.bottom->num + 1;
	if(nextRecord <= nitems) {
		addBufferBot(&RLBuffer, getRecord(nextRecord),rArea.bot - rArea.top);
	} else {
		printf("bottom\n");
	}
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
//------------------------ adding ---------------------------------
void addRecord(char *subject, char* body){
	ReadMystoreFromChild("add",subject,body,NULL);
	ParseRecord(++nitems);
}
