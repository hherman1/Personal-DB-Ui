#include "myui2.h"

struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;

char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte


struct TemplateString TS[] = {
	{1,1,XT_CH_CYAN,"dbname | Max Cards cards | 					FutureDiary				(c) Hunter Herman & Tian Ci Lin"},
	{2,1,XT_CH_WHITE,"--------------------------------------------------------------------------------------------------------------------------------"},
	{3,32,XT_CH_YELLOW,"S: Search	[R: Read]	A: Add		H: Help"},
	{5,1,XT_CH_YELLOW,"Search: ____________________"},
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
	int i, c;
	
	fill(subject,30);
	fill(body,140);
	
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

	//records operation
	
	int recordDisplayCounter = 0;
	for (i = recordDisplayStart; i <= recordDisplayEnd && i <= nitems; i++){
		ParseRecord(i);
		int row = i - recordDisplayStart + 7;
		DisplayAt(row,1,XT_CH_GREEN,30,searchNvs("subject"));
		DisplayAt(row,75,XT_CH_GREEN,30,searchNvs("time"));
	}

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
	value = "";
	for (j = 0; j < n_nvs; ++j) {
		if (strcmp(nvs[j].name,name) == 0) {
			value = nvs[j].value;
			break;
		}
	}
	
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
