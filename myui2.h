#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xterm_control.h"
#include "keyboard.h"

#define TRUE	1
#define FALSE	0

#define MAX_SUBJECT_LEN 30
#define MAX_BODY_LEN 140
#define MAX_TiME_LEN 20
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
	int down;
}Area;

// Prototypes -------------------------------------
int ParseInput(char *in, int n_in);
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4);
void DisplayStat(void);
void fill(char *s, int n);
void SearchDisplay(char *prompt, char *name, char *color);
void DisplayAt(int row, int col, char *color, int maxlength, char *s);
int FindStringPosition(char *prompt);
//new
char *searchNvs(char name[]);
void ParseStat(void);
void ParseRecord(int numRec);
