#include "record.h"
void edit(char *str,int maxLength,Cursor *cursor,char c);
int scroll(Record **hovered, struct RecordList **activeBuffer,char c);

//mode switches
int modeCheck(int c,int recordSelected,int *cursorLeft,char **cursorArea, char *subject,Cursor *cursor,Area rArea,Record **hovered,struct RecordList **activeBuffer,  struct RecordList *RLBuffer);
//{
int check_record(char **cursorArea, Record **hovered, struct RecordList **activeBuffer,struct RecordList *RLBuffer);
int init_add(char **cursorArea, Cursor* cursor);
int init_search(char *subject,Cursor *cursor,char **cursorArea);
int init_edit(int recordSelected,int *cursorLeft,char **cursorArea, Cursor *cursor,Area rArea,Record *hovered,struct RecordList *activeBuffer);
//}
