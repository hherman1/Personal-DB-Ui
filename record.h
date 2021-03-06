#pragma once

#define MAX_SUBJECT_LEN 30
#define MAX_BODY_LEN 140
#define MAX_TIME_LEN 20

#define MAX_RECORDS_TO_DISPLAY 10

#define RECORD_NUM_SPACE 5

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
	int lengthfrombot;
};
struct RecordColorScheme;
void displayRecords(Record hovered,struct RecordList *buffer,Area rArea,struct RecordColorScheme *scheme);
void printViewBuffer(Record hovered, struct RecordList *buffer, Area rArea,struct RecordColorScheme *scheme);
void trimBuffer(struct RecordList *buffer, Record *cutoff);
Record* adjustBufferForArea(Record hovered,struct RecordList *buffer, Area rArea);
int getRecordY(Record *r,struct RecordList *buffer,Area rArea);
int requiredSpace(Record r,int width);
void selectRecord(int record);
void wrapText(int left,int width, char *text);
void loadRecords(struct RecordList *buffer,int low,int high,int number);

//located in myui2.c
void ParseSearch(char *search, struct RecordList *sBuffer);
void deleteRecord(Record *r, struct RecordList *buffer);
void deleteRecordMystore(int num);

/////

void mapNum(int n, struct RecordList *buffer);
void mapPrevwards(int n, Record *r);
void mapNextwards(int n, Record *r);

void removeRecordFromBuffer(Record *r, struct RecordList *buffer);
void removeRecord(Record *r);



Record *getRecord(int n);
Record *allocateTopRecord();
Record *popRecord();
Record *findRecord(struct RecordList buffer,int num);
void freeBuffer(struct RecordList *buffer);
void freeRecord(Record *target);
void bufferRecord(struct RecordList *buffer,Record *r);
void addBufferTop(struct RecordList *buffer,Record *r);
void addBufferBot(struct RecordList *buffer,Record *r);
int bufferLength(struct RecordList buffer);

Record *nextRecord(Record *r);
Record *previousRecord(Record *r);
void scrollUp(struct RecordList *buffer);
void scrollDown(struct RecordList *buffer);
