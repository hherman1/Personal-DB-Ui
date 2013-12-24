#define R_TEXT_COLOR XT_CH_GREEN
#define R_HOVERED_BG_COLOR XT_BG_GREEN
#define R_HOVERED_TEXT_COLOR XT_CH_WHITE
#define R_SELECTED_BG_COLOR XT_BG_CYAN
#define R_SELECTED_TEXT_COLOR XT_CH_BLUE
#define R_SELECTED_BODY_COLOR XT_CH_BLUE
#define R_SELECTED_TEXT_STYLE XT_CH_BOLD
#define R_SELECTED_BODY_STYLE XT_CH_NORMAL
#define R_SELECTED_HOVERED_TEXT_COLOR XT_CH_RED
#define MAX_SUBJECT_LEN 30
#define MAX_BODY_LEN 140
#define MAX_TIME_LEN 20

#define MAX_RECORDS_TO_DISPLAY 10
#define MAX_EXTRA_RECORDS 100;
/*MultiBodyRecord pairs a single subject with all the bodies with that subject name
bodies are represented by the integer number that appear in mystore storage
*/
typedef struct Body
{
	int itemNum;
	//char *time; not using
	char *body;
	Body *next;
	Body *prev;
}Body;
typedef struct BodyList
{
	Body *top;
	Body *bot;
	int numBodies;
}BodyList;

typedef struct MultiBodyRecord
{
	//int num; not needed
	char *subject;
	BodyList *bodies; 
}Record;

// general
int failToMalloc(void);
void ParseRecord(int numRec);
void ParseStat(void);
char *searchNvs(char name[]);
//within mutiBodyRecordList 
void loadRecords();
void loadNextRecord(void);
void addRecord(char *subject, char* body);
//within single mutiBodyRecord
void addBody(BodyList *bodies, int newBody, int itemNum);


//from myui2_util
int ParseInput(char *in, int n_in);
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4);

void displayRecords(Record hovered,struct RecordList *buffer,Area rArea);
void selectRecord(Record record,struct RecordList buffer,Area rArea);
void wrapText(int width, char *text);
void loadRecords(struct RecordList *buffer,int low,int high,int number);
Record *getRecord(int n);
Record *findRecord(struct RecordList *buffer,int num);
void freeRecord(Record *target);
void bufferRecord(struct RecordList *buffer,Record *r);
void addBufferTop(struct RecordList *buffer,Record *r);
void addBufferBot(struct RecordList *buffer,Record *r);
int bufferLength(struct RecordList buffer);
