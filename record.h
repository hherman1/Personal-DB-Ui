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
typedef struct MultiBodyRecord
{
	//int num; not needed
	char *subject;
	int *body; //list of the locations of the bodies in mystore that have the subject name 
	char *time;
	struct Record *next;
	struct Record *prev;
}Record;
struct MultiBodyRecordList
{
	MultiBodyRecord *top;
	MultiBodyRecord *bottom;
	int length;
};

//within record operations
void addBody(char *body);


// record operations
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
