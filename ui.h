#include "record.h"

#define UI_AREA_RECORDS 0
#define UI_AREA_ADD_SUBJECT 1
#define UI_AREA_ADD_BODY 2
#define UI_AREA_SEARCH 3
#define UI_AREA_EDIT_SUBJECT 4
#define UI_AREA_EDIT_BODY 5
#define UI_AREA_DELETE 6

#define UI_DELETE_CONFIRM "Are you sure you want to delete record %i?"

void edit(char *str,int maxLength,Cursor *cursor,char c);
int scroll(Record **hovered, struct RecordList **activeBuffer,char c);

//mode switches
int modeCheck(int c,int recordSelected,int *cursorLeft,int *cursorArea, char *subject,Cursor *cursor,Area rArea,Record **hovered,struct RecordList **activeBuffer,  struct RecordList *RLBuffer);
//{
int check_record(int *cursorArea, Record **hovered, struct RecordList **activeBuffer,struct RecordList *RLBuffer);
int init_add(int *cursorArea, Cursor* cursor);
int init_search(char *subject,Cursor *cursor,int *cursorArea);
int init_edit(int recordSelected,int *cursorLeft,int *cursorArea, Cursor *cursor,Area rArea,Record *hovered,struct RecordList *activeBuffer);
int init_delete(int recordSelected,int *cursorArea,Record *hovered, struct RecordList *activeBuffer);
//}
