#include "record.h"
#include "color.h"

#define UI_COMMANDABLE(r) r == UI_AREA_RECORDS || r == UI_AREA_DELETE

#define UI_AREA_RECORDS 0
#define UI_AREA_ADD_SUBJECT 1
#define UI_AREA_ADD_BODY 2
#define UI_AREA_SEARCH 3
#define UI_AREA_EDIT_SUBJECT 4
#define UI_AREA_EDIT_BODY 5
#define UI_AREA_DELETE 6

#define UI_AREA_ADD_SUBJECT_VERTICAL_MARGIN -6
#define UI_AREA_ADD_BODY_VERTICAL_MARGIN -5
#define UI_AREA_SEARCH_SUBJECT_VERTICAL_MARGIN 5

#define TEXT_ALIGN_LEFT 0
#define TEXT_ALIGN_CENTER 1
#define TEXT_ALIGN_RIGHT 2

#define UI_DELETE_CONFIRM(r) f_color("Are you sure you want to delete record %i, %s? " f_color("[y/N]", UI_DELETE_CONFIRM_COLOR),XT_CH_BLACK XT_CH_BOLD),r->num,r->subject

#define UI_MODE_LABEL_WIDTH 15

#define DB_INFO_FIELD_WIDTH 15

struct displayText {
	int verticalMargin;
	int alignment;
	char *color;
	char *text;
};

struct modeText {
	int *mode;
	int nModes;
	char *text;
};
struct modeBar {
	struct modeText *modes;
	int nModes;
	int verticalMargin;
};

//display functions
int displayUIElement(Area window,struct displayText ui,...);
void displayModeBar(int cursorArea,Area window,struct modeBar modeBar);

//mode functions

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
int commandMode(int cursorArea);
//}

//utility
int isMode(int mode, struct modeText mt);

int verticalMargin(int margin,Area ar);
int horizontalMargin(int margin,Area ar);
int margin(int margin, int low, int high);