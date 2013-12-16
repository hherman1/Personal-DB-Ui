#define R_TEXT_COLOR XT_CH_GREEN
#define R_HOVERED_BG_COLOR XT_BG_MAGENTA
#define R_HOVERED_TEXT_COLOR XT_BG_BLUE
typedef struct Record
{
	int num;
	char *subject;
	char *body;
	char *time;
	struct Record *next;
	struct Record *prev;
}Record;
struct RecordList
{
	Record *top;
	Record *bottom;
	int length;
};
void displayRecords(int cursorRow,struct RecordList buffer,Area rArea);
void wrapText(int width, char *text);
int getHoveredRNum(int cursorRow,struct RecordList buffer,Area rArea);
void loadRecords(struct RecordList *buffer,int low,int high,int number);
Record *getRecord(int n);
Record *findRecord(struct RecordList *buffer,int num);
void freeRecord(Record *target);
void recordcpy(Record *dest,Record src);
void bufferRecord(struct RecordList *buffer,Record *r);
void bufferNewRecord(struct RecordList *buffer,Record *r);
