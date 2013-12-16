#define MAX_SUBJECT_LEN 30
#define MAX_BODY_LEN 140
#define MAX_TIME_LEN 20

#define MAX_RECORDS_TO_DISPLAY 30
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
void displayRecords(struct RecordList buffer,Area rArea);
void loadRecords(struct RecordList *buffer,int low,int high,int number);
Record *getRecord(int n);
Record *findRecord(struct RecordList *buffer,int num);
void freeRecord(Record *target);
void recordcpy(Record *dest,Record src);
void bufferRecord(struct RecordList *buffer,Record *r);
void addBufferTop(struct RecordList *buffer,Record *r);
void addBufferbot(struct RecordList *buffer,Record *r);