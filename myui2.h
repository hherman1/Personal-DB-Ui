#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#include "xterm_control.h"
#include "keyboard.h"

#define TRUE	1
#define FALSE	0

#define DEBUG 1
#define ERROR_MESSAGE_BUFFER_LENGTH 120

#if DEBUG
#include <assert.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Globals ---------------------------------
struct NameValue {
	char *name;
	char *value;
};

struct TemplateString {
	int row;
	int col;
	char *color;
	char *string;
};

struct StringPosition {
	int row;
	int col;
	int length;
	char *name;
};
typedef struct Area
{	//help define the boundary of areas like display space, title space, 
	int left;
	int right;
	int top;
	int bot;
	char* text;
	int toDisplay;
}Area;

typedef struct Cursor
{
	int x;
	int y;
} Cursor;

int DUMP;
// Prototypes -------------------------------------
int ParseInput(char *in, int n_in);
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4);
void DisplayStat(void);
void fill(char *s, int n,char c);
void SearchDisplay(char *prompt, char *name, char *color);
void DisplayAt(int row, int col, char *color, int maxlength, char *s);
int FindStringPosition(char *prompt);
//new
void draw();
char *searchNvs(char name[]);
void addRecord(char *subject, char* body);
void ParseStat(void);
void ParseRecord(int numRec);
void addRecord(char *subject, char* body);

void message(char *msg, ...);
