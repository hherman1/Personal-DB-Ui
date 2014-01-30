#include <signal.h>
#include "myui4.h"
#include "record.h"
#include "ui.h"
#include "bindings.h"
#include "color.h"
#include "memory.h"
////sockets update
extern char* server_ip; 
extern int portno; 
//----
struct NameValue *nvs = NULL; //namevalues storage i think
int n_nvs = 0;
extern int recordSelected;
char input[1000];
int n_input=0;	// number of chars in the input, not including terminating NULL-byte

Area rArea = {1,120,7,7 + MAX_RECORDS_TO_DISPLAY - 1,"",TRUE};
Area newSubjectArea = {15,120,44,44,"",TRUE};
Area newBodyArea = {12,120,45,47,"",TRUE};

extern struct RecordColorScheme scheme;
struct winsize window;
Area windowArea;


//
struct displayText DBInfo = {
	1,TEXT_ALIGN_LEFT,XT_CH_CYAN,"Version: " db_var_col(" %-*.2g") "\t||\t" f_color("DBName: ",XT_CH_CYAN) db_var_col(" %-*s") "\t||\t" f_color("Author: ",XT_CH_CYAN) db_var_col(" %-*s") "\t||\t" f_color("Total Cards: ",XT_CH_CYAN) db_var_col(" %-*i")
};


struct displayText UI[] = {
	{UI_AREA_SEARCH_SUBJECT_VERTICAL_MARGIN,TEXT_ALIGN_LEFT,XT_CH_YELLOW,"Search:"},
	{UI_AREA_SEARCH_SUBJECT_VERTICAL_MARGIN,TEXT_ALIGN_RIGHT,XT_CH_RED,"F2 to return from search"},
	{UI_AREA_ADD_SUBJECT_VERTICAL_MARGIN,TEXT_ALIGN_LEFT,XT_CH_YELLOW,"new Subject: "},																				
	{UI_AREA_ADD_BODY_VERTICAL_MARGIN,TEXT_ALIGN_LEFT,XT_CH_YELLOW,"new Body: "},
	{-2,TEXT_ALIGN_LEFT,XT_CH_RED,"Note:\tF9 or 'q' at any time to quit"}, //pls save to commit changes.
};
int nUI = sizeof(UI)/sizeof(UI[0]);
///


///
struct modeText UIModes[] = {
	{(int[1]){UI_AREA_SEARCH},1,"s: Search"},
	{(int[2]){UI_AREA_ADD_SUBJECT,UI_AREA_ADD_BODY},2,"a: Add"},
	{(int[2]){UI_AREA_EDIT_SUBJECT,UI_AREA_EDIT_BODY},2,"e: Edit"},
	{(int[1]){UI_AREA_DELETE},1,"d: Delete"}
};
int nUIModes = sizeof(UIModes)/sizeof(UIModes[0]);

struct modeBar UIModeBar;
//




struct RecordList searchBuffer;
struct RecordList RLBuffer;

struct RecordList *activeBuffer;

Record *hovered = NULL;


int nitems = 0;  //numRecords
float version = 0;
char *author = NULL;


char subject[MAX_SUBJECT_LEN+1]; //used for new sub and new body additions
struct displayText UIInputSubject = {
	UI_AREA_ADD_SUBJECT_VERTICAL_MARGIN,TEXT_ALIGN_LEFT,XT_CH_NORMAL,subject
};

char body[MAX_BODY_LEN+1];
struct displayText UIInputBody = {
	UI_AREA_ADD_BODY_VERTICAL_MARGIN,TEXT_ALIGN_LEFT,XT_CH_NORMAL,body
};



char errmsg[ERROR_MESSAGE_BUFFER_LENGTH+1]; // message at bottom
struct displayText UIMessage = {
	-3,TEXT_ALIGN_LEFT,XT_CH_DEFAULT,errmsg
};


int cursorArea =UI_AREA_RECORDS; // what are the cursor is at //title, record,addSubject, message, ....



Cursor cursor = {0,0}; //the cursor

int cursorLeft = 0; //ridiculous hack but saves a lot of time from proper code



void debug(struct RecordList *buffer) { // worthless function for debug purposes
	return;
}
// ------------------------------------------------ main --------------------
int main(int argc, char *argv[]) {
	int c;
	//sockets
	if (argc < 3){
		printf("Sockets client, usage:\n");
		printf("sockets_client  {IP-address} {port}\n");
		return 0;
	}
	
	portno = atoi(argv[2]);
	server_ip = argv[1]
	////-----------------------------
	struct sigaction sigHandler;
	sigHandler.sa_handler = sig_handler;

	sigaction(SIGINT,&sigHandler,NULL);


	ioctl(STDOUT_FILENO,TIOCGWINSZ, &window);
	windowArea = windowToArea(window);

	UIModeBar.modes = UIModes;
	UIModeBar.nModes = nUIModes;
	UIModeBar.verticalMargin = 3;


	rArea.right = window.ws_col - rArea.left;
	rArea.bot = window.ws_row - rArea.top;

	initScheme();
	
	fill(subject,30,'\0');
	fill(body,140,'\0');
	
	xt_par0(XT_CLEAR_SCREEN);
	ParseStat();

	
	activeBuffer = &RLBuffer;
	if(nitems) {
		loadRecords(&RLBuffer,1,MAX_RECORDS_TO_DISPLAY + 1,nitems);
		
			 if( RLBuffer.top) {
				RLBuffer.lengthfrombot = nitems - RLBuffer.bottom->num;
				hovered = RLBuffer.top;
			}
	}
	draw();

	while (TRUE) {

		int redraw = FALSE;
		DUMP = FALSE;
		while ((c = getkey()) == KEY_NOTHING) {
			struct winsize old = window;
			ioctl(STDOUT_FILENO,TIOCGWINSZ, &window);
			if(old.ws_col != window.ws_col || old.ws_row != window.ws_row) {
				draw();
			}
		} 

		redraw = TRUE;

		if (c == KEY_F9 || c == 'q')  {
			xt_par0(XT_CLEAR_SCREEN);
			xt_par0(XT_CH_NORMAL);
			xt_par2(XT_SET_ROW_COL_POS,1,1);
			if(c == 'q') printf("Exited with keypress: %c\n",c);
			else printf("Exited with keycode: %i\n",c);
			destroy();
		}

		if(cursorArea == UI_AREA_RECORDS) {
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
		else if (cursorArea == UI_AREA_ADD_SUBJECT || cursorArea == UI_AREA_ADD_BODY){
			cursor.y = newSubjectArea.top + (cursorArea - UI_AREA_ADD_SUBJECT);
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;
			if (cursorArea == UI_AREA_ADD_SUBJECT){
				if(c == KEY_DOWN) {
					cursorArea = UI_AREA_ADD_BODY;
					cursor.y++;
					redraw = TRUE;
				}
				if (c == KEY_ENTER){
					cursorArea = UI_AREA_ADD_BODY;
					cursor.x = 0;
					cursor.y ++;
				}
				else {
					redraw = TRUE;
				}
			} else if (cursorArea == UI_AREA_ADD_BODY){
				if(c == KEY_UP) {
					cursorArea = UI_AREA_ADD_SUBJECT;
					cursor.y--;
					redraw = TRUE;
				}
				if (c == KEY_ENTER){
					cursorArea = UI_AREA_RECORDS;
					addRecord(subject,body);
					RLBuffer.lengthfrombot++;
					cursor.x = 0;
					cursor.y = 0;
				}
			}
			if(cursorArea == UI_AREA_ADD_SUBJECT) edit(subject,MAX_SUBJECT_LEN,&cursor,c);
			else if(cursorArea == UI_AREA_ADD_BODY) edit(body,MAX_BODY_LEN,&cursor,c);


			redraw = TRUE;

		} else if (cursorArea == UI_AREA_SEARCH) {
			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;
			if (c == KEY_ENTER){
				cursorArea = UI_AREA_RECORDS;
				if(searchBuffer.top) freeBuffer(&searchBuffer);
				ParseSearch(subject,&searchBuffer);
				ParseStat();
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

		else if (cursorArea == UI_AREA_EDIT_SUBJECT || cursorArea == UI_AREA_EDIT_BODY){

			redraw = modeCheck(c,recordSelected,&cursorLeft,&cursorArea,subject,&cursor,rArea,&hovered,&activeBuffer,&RLBuffer) || redraw;


			if (cursorArea == UI_AREA_EDIT_SUBJECT){
				if(c == KEY_DOWN) {
					cursorArea = UI_AREA_EDIT_BODY;
					cursor.y++;
					redraw = TRUE;
				} else if(c == KEY_ENTER) {
					cursorArea = UI_AREA_EDIT_BODY;
					cursor.y ++;
				} else {

				}
			}else if (cursorArea == UI_AREA_EDIT_BODY){
				if(c == KEY_UP) {
					cursorArea = UI_AREA_EDIT_SUBJECT;
					cursor.y--;
					redraw = TRUE;
				} else if(c == KEY_ENTER) {
					cursorArea = UI_AREA_RECORDS;
					editRecord(hovered->num,hovered->subject,hovered->body);
					ParseStat();
					cursor.x = 0;
					cursor.y = 0;
				} 
			}
			if(cursorArea == UI_AREA_EDIT_SUBJECT) edit(hovered->subject,MAX_SUBJECT_LEN,&cursor,c);
			else if(cursorArea == UI_AREA_EDIT_BODY) edit(hovered->body,MAX_BODY_LEN,&cursor,c);

			redraw = TRUE;
		}
		else if (cursorArea == UI_AREA_DELETE) {
			if(KEY_MODE_DELETE_CONFIRM(c)) {
				if(activeBuffer != &RLBuffer) {
					Record *RLhovered = findRecord(RLBuffer,hovered->num);
					if(RLhovered) removeRecordFromBuffer(hovered,&RLBuffer);
				}
				struct Record *selectNext = NULL;
				if(hovered->prev) selectNext = hovered->prev;
				else if(hovered->next) selectNext = hovered->next;
				deleteRecord(hovered,activeBuffer);
				ParseStat();
				hovered = selectNext;
				selectRecord(-1);
				cursorArea = UI_AREA_RECORDS;
				setColor(R_COLOR_SCHEME_CLASSIC);
			} else if (KEY_MODE_DELETE_DENY(c)) {
				cursorArea = UI_AREA_RECORDS;
				setColor(R_COLOR_SCHEME_CLASSIC);
			} else {
				message(UI_DELETE_CONFIRM(hovered));
			}
		}
		
		if(redraw)
			draw();

		flushPool();
	}
	
	return 1;
}
// -------------------------------------draw---------------------------------
void resize() {
	ioctl(STDOUT_FILENO,TIOCGWINSZ, &window);
	windowArea = windowToArea(window);
	rArea.right = windowArea.right - rArea.left;
	rArea.bot = windowArea.bot - rArea.top;	
}
void draw() {
	//config
	printf("redrawing");

	int i = 0;
	xt_par0(XT_CLEAR_SCREEN);

	resize();
	//perform operations on stat
	// display basic elements
	if(!author || !version) {
		ParseStat();
	}
	int w = DB_INFO_FIELD_WIDTH;
	displayUIElement(windowArea,DBInfo,w,version,w,"mystore.dat",w,author,w,nitems);
	for (i = 0; i < nUI; ++i) {
		displayUIElement(windowArea,UI[i]);
	}
	displayModeBar(cursorArea,windowArea,UIModeBar);

	//new subject and body
	//draw entered text (not so) properly
	Area subjectArea = windowArea;
	if(cursorArea == UI_AREA_ADD_BODY || cursorArea == UI_AREA_ADD_SUBJECT){
		//DisplayAt(newSubjectArea.top,ENTRY_FIELD_LABEL_SPACE,XT_CH_CYAN,MAX_SUBJECT_LEN,subject);
		//DisplayAt(newBodyArea.top,ENTRY_FIELD_LABEL_SPACE,XT_CH_WHITE,MAX_BODY_LEN,body);
		int pos = ENTRY_FIELD_LABEL_SPACE;
		subjectArea.left = pos;
		UIInputSubject.verticalMargin = UI_AREA_ADD_SUBJECT_VERTICAL_MARGIN;
		UIInputBody.verticalMargin = UI_AREA_ADD_BODY_VERTICAL_MARGIN;
		
		cursor.y = displayUIElement(subjectArea,UIInputSubject);
		int t2 = displayUIElement(subjectArea,UIInputBody);
		if(cursorArea == UI_AREA_ADD_BODY) {
			cursor.y = t2;
		}
		cursorLeft = pos;
		//cursorLeft = ENTRY_FIELD_LABEL_SPACE;
	} else if (activeBuffer == &searchBuffer || cursorArea == UI_AREA_SEARCH ) {
		int pos = ENTRY_FIELD_LABEL_SPACE;
		subjectArea.left = pos;
		UIInputSubject.verticalMargin = UI_AREA_SEARCH_SUBJECT_VERTICAL_MARGIN;
		int t = displayUIElement(subjectArea,UIInputSubject);
		if(cursorArea == UI_AREA_SEARCH) {
			cursor.y = t;
			cursorLeft = pos;
		}
	} else if (cursorArea == UI_AREA_RECORDS) {
		cursorLeft = RECORD_NUM_SPACE;
	}
	//buffer
	if(nitems) {
		if(!RLBuffer.top){
			loadRecords(&RLBuffer,1,MAX_RECORDS_TO_DISPLAY + 1,nitems);
			RLBuffer.lengthfrombot = nitems - RLBuffer.bottom->num;
		} 
		if(!hovered) {
			hovered = activeBuffer->top;
		}
		if(activeBuffer->top) {
			displayRecords(*hovered,activeBuffer,rArea,&scheme);			
		}
	}

	displayUIElement(windowArea,UIMessage);
	//DisplayAt(51,0,XT_CH_DEFAULT,strlen(errmsg),errmsg);
	fill(errmsg,ERROR_MESSAGE_BUFFER_LENGTH,'\0');
	
	xt_par2(XT_SET_ROW_COL_POS,cursor.y,cursor.x + cursorLeft);
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
	char *nitemsTemp;
	int change = nitems;

	if((nitemsTemp = searchNvs("nitems"))) {
		nitems = atoi(nitemsTemp);
	} else {
		printf("ERROR: Server returned NULL\nRestart your mystore and check db.dat for corruptions, then try again.\n");
		destroy();
	}
	change -= nitems;
	RLBuffer.lengthfrombot += change;
	version = atof(searchNvs("version"));
	char *temp = searchNvs("author");
	if(author) {
		free(author);
	}
	author = malloc(sizeof(char) * strlen(temp));
	strcpy(author,temp);

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
		if (++col == windowArea.right) {
			xt_par2(XT_SET_ROW_COL_POS,row+1,1);
			col = 1;
		}
	}
	fflush(stdout);
}
//------------------------ adding ----------------------------------------
void addRecord(char *subject, char* body){
	ReadMystoreFromChild("add",subject,body,NULL);
	ParseRecord(++nitems);
}
//------------------------ editing ---------------------------------------
void editRecord(int num,char *subject, char* body){
	char sNum[15];
	sNum[14] = '\0';
	sprintf(sNum,"%d",num);
	ReadMystoreFromChild("edit",sNum,subject,body);
	//ParseRecord(nitems);
}
//------------------------ deleting ---------------------------------------
void deleteRecord(Record *r, struct RecordList *buffer) {
	deleteRecordMystore(r->num);
	removeRecordFromBuffer(r,buffer);
	nitems--;
}
void deleteRecordMystore(int num) {
	char numS[10];
	snprintf(numS,10,"%i",num);
	int p = ReadMystoreFromChild("delete",numS,NULL,NULL);
	ParseInput(input,n_input);
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
//------------------------ resizing ---------------------------------------
Area windowToArea(struct winsize window) {
	Area ans;
	ans.top = 1;
	ans.left = 1;
	ans.bot = window.ws_row;
	ans.right = window.ws_col;
	return ans;
}
void destroy() {
	if(searchBuffer.top || searchBuffer.bottom) {
		freeBuffer(&searchBuffer);
	}
	if(RLBuffer.top || RLBuffer.bottom) {
		freeBuffer(&RLBuffer);
	}
	if(author) free(author);
	getkey_terminate();
	exit(0);
}
void sig_handler(int signal){
	xt_par0(XT_CLEAR_SCREEN);
	printf("\nCAUGHT SIGNAL: %i\nExiting...\n",signal);
	destroy();
}
