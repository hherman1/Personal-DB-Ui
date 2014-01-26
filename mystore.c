/* mystore:  manages a linked-list database (called "mystore.dat") of little items

V 0.92: fixes bug in month output
V 0.90: implements edit

*/
/* Implements commands:
	add
	stat
	display
	delete
	edit
*/

#define version "0.92"
#define author "PBrooks"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "memory.h"

char *Usage = "Usage:\tmystore add \"subject\" \"body\"\n\
	mystore stat\n\
	mystore display {item-no}\n\
	mystore delete {item-no}\n\
	mystore edit {item-no} \"subject\" \"body\"\n";

#define NOTHING		0
#define ADD			1
#define STAT		2
#define DISPLAY		3
#define DELETE		4
#define EDIT		5
#define SEARCH 		6

#define TRUE	1
#define FALSE	0
// Prototypes:
int parseArgs(int argc, char *argv[]); //changed and
int parseArgsUtil(int argc, char *argv[]); //moved by Tianci Lin
int isPositive(char *s);
int readData(void);
int add(char *subject, char *body);
void status(void);
char *saveFormatted(char *format, ...);
int search(char *subject);
char *rstrip(char *s);
void list(void);
//---fifo prototypes
int Process(char *s);
int SeparateIntoFields(char *s, char **fields, int max_fields);
static void the_handler(int sig);

//
int fd_read, fd_writeTEMP, fd_write;
char *fifo_read = "/tmp/fifo_server.dat";

// Command line arguments processed:
int command = NOTHING;
char *subject = NULL;
char *body = NULL;
int item_start = -1;
int item_end = -1;



// this describes the data item on disk
struct data {
	time_t theTime;
	char theSubject[31];
	char theBody[141];
};

// this describes the data item in memory (data with a link to the next carrier)
struct carrier {
	struct data theData;
	struct carrier *next;
};

// These variables track the data in memory
int nitems = 0;
struct carrier *first = NULL;
struct carrier *last = NULL;

int rewrite = FALSE;		// if data changes then rewrite
char errmsg[100] = "";

// ---------------------------------- main() --------------------------------
int main(int argc, char *argv[]) {
	if (!parseArgs(argc, argv)) {
		if (errmsg[0] != '\0')
			printf("%s\n",errmsg);
		else
			printf("|status: ERROR: No command-line arguments, or error in arguments\n\nVersion: %s\n%s|\n",
			version,Usage);
		return 1;
	}
	
	//run comand
	
	
	return 0;
}

// ------------------------------- parseArgs() -------------------------------
// parse the command-line arguments, and assign the global variables from them
// return FALSE if any problem with the command-line arguments
int parseArgs(int argc, char *argv[]) {
	//----------------using fifo-----------
	//if (argc < 2) {
	int how_much;
	char input[BUFSIZ];
	// remove the FIFO in case it exists
	unlink(fifo_read);
	
	// create the FIFO pseudo-file
	if (mkfifo(fifo_read,0666) != 0) {
		perror("mkfifo error: ");
		return -1;
	}
	// open it for reading:
	fd_read = open(fifo_read, O_RDONLY);
	printf("Initialized fifo at %s\nAwaiting input\n",fifo_read);
	if (fd_read < 0) {
		printf("open(fifo_read) failed, returns: %d\n", fd_read);
		return fd_read;
	}
	
	// also open it for writing, but don't write anything to it (this is to prevent its closure when a client has finished writing to it).
	if ((fd_writeTEMP = open(fifo_read, O_WRONLY)) < 0) {
		perror("Cannot open fifo_read for writing. ");
		return fd_write;
	}
	while (1) {

		how_much = read(fd_read,input,BUFSIZ);
		printf("Received: %s\n",input);
		if (how_much > 0) {
			input[how_much]='\0';
			if (Process(input) == -1) { 
				printf("fifo_server quitting...\n");
				close(fd_read);
				unlink(fifo_read);
				return 0;
			}
		}
	}
	return 0;
	
	/*
	}else {  //no fifo
		parseArgsUtil(argc, argv);
		runCommand(argc,argv);
	}
	*/
}

//use before runcommand
int parseArgsUtil(int argc, char *argv[]){   
	/////////////////////////////////////////////////////////OLD version, pipes
	// try zero-argument commands: list and stat
	if (argc == 1) {
		if (strcmp(argv[0], "stat") == 0) {
			command = STAT;
			return TRUE;
		}
		else {
			sprintf(errmsg, "Unrecognized argument: %s", argv[0]);
			return FALSE;
		}
	}
	// try the one-argument commands: delete and display
	else if (argc == 2) {
		if (strcmp(argv[0],"delete") == 0 && isPositive(argv[1])) {
			command = DELETE;
			item_start = atoi(argv[1]);
			return TRUE;
		}
		else if (strcmp(argv[0],"display") == 0 && isPositive(argv[1])) {
			command = DISPLAY;
			item_start = atoi(argv[1]);
			return TRUE;
		}
		else if (strcmp(argv[0],"search") == 0){
			command = SEARCH;
			subject = argv[1];
			return TRUE;
		}
		else {
			sprintf(errmsg, "Unrecognized 2-argument call: %s %s", argv[0],argv[1]);
			return FALSE;
		}
	}
	// try the two-argument command: add
	else if (argc == 3) {
		if (strcmp(argv[0],"add") == 0) {
			command = ADD;
			subject = argv[1];
			body = argv[2];
			return TRUE;
		}
		else {
			sprintf(errmsg, "Unrecognized 3-argument call: %s %s %s",argv[0],argv[1],argv[2]);
			return FALSE;
		}
	}
	// try the three-argument command: edit
	else if (argc == 4) {
		if (strcmp(argv[0], "edit") == 0 && isPositive(argv[1])) {
			command = EDIT;
			item_start = atoi(argv[1]);
			subject = argv[2];
			body = argv[3];
			return TRUE;
		}
		else {
			sprintf(errmsg, "Unrecognized 4-argument call: %s %s %s %s",argv[0],argv[1],argv[2],argv[3]);
			return FALSE;
		}
	}
	else
		return FALSE;
}

int runCommand(int argc, char *argv[]){

	if (!readData()) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: Error reading mystore.dat\n\n%s|\n", Usage);
		return 1;
	}

	if (command == ADD && !add(argv[1],argv[2])) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: Failure to add new item|\n");
		return 1;
	}
	if (command == STAT) {
		printf("running stat\n");
		status();
	}
	if (command == SEARCH && !search(argv[1])) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: Cannot search %s|\n",argv[1]);
		return 1;
	}

	if (command == DISPLAY && !display(argv[1])) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: Cannot display %s|\n",argv[1]);
		return 1;
	}
	
	if (command == DELETE && !delete(argv[1])) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: Cannot delete %s|\n", argv[1]);
		return 1;
	}
	
	if (command == EDIT && !edit(argv[1])) {
		if (errmsg[0] != '\0')
			printf("|status: ERROR: %s|\n", errmsg);
		else
			printf("|status: ERROR: cannot edit %s|\n",argv[1]);
		return 1;
	}
	
	if (rewrite) {
		printf("rewriting\n");
		if (!writeData()) {
			if (errmsg[0] != '\0')
				printf("|status: ERROR: %s|\n", errmsg);
			else
				printf("|status: ERROR: Could not write the data, file may be destroyed|\n");
			return 1;
		}
	}
	return 0;
}

// --------------------------------- isPositive ------------------------------
// return TRUE if the string is a positive (>= 1) integer
int isPositive(char *s) {
	char *p = s;
	while (*p >= '0' && *p <= '9') 
		++p;
	if (p != s && *p == '\0' && atoi(s) > 0)
		return TRUE;
	return FALSE;
}

// ---------------------------------- readData --------------------------------
int readData(void) {
	int i;
	struct data current_data;
	struct carrier *current_carrier;
	struct carrier *previous_carrier;
	
	FILE *fp = fopen("mystore.dat", "rb");	// read in binary file mode
	if (!fp)
		return TRUE;	// no such file, that's OK: we're doing this for the first time
	
	if (fread(&nitems, sizeof(int), 1, fp) != 1) {  // try to read nitems
		fclose(fp);
		sprintf(errmsg, "Cannot read nitems");
		return FALSE;
	}
	
	for (i = 0; i < nitems; ++i) {
		if (fread(&current_data, sizeof(struct data), 1, fp) != 1) { //try to read the next item
			fclose(fp);
			sprintf(errmsg,"Cannot read item %d\n",i+1);
			return FALSE;
		}
		if ((current_carrier = calloc(1, sizeof(struct carrier))) == NULL) {  //allocate memory
			fclose(fp);
			sprintf(errmsg,"Cannot allocate %d\n",sizeof(struct carrier));
			return FALSE;
		}
		current_carrier->theData = current_data;	// load the data into the carrier
		// weave the linked-list
		if (i == 0)
			first = current_carrier;
		else
			previous_carrier->next = current_carrier;
		previous_carrier = current_carrier;
	}
	
	fclose(fp);
	last = current_carrier;
	return TRUE;
}

// ---------------------------------------- add --------------------------------------
int add(char *subject, char *body) {
	struct data current_data;
	struct carrier *current_carrier;
	
	// fill up the members of current_data
	strncpy(current_data.theSubject, subject, 30);
	current_data.theSubject[30]='\0';
	strncpy(current_data.theBody, body, 140);
	current_data.theBody[140] = '\0';
	current_data.theTime = time(NULL);
	
	if ((current_carrier = calloc(1, sizeof(struct carrier))) == NULL) // allocate memory
		return FALSE;
	
	current_carrier->theData = current_data;
	if (nitems == 0)
		first = last = current_carrier;
	else {
		last->next = current_carrier;
		last = current_carrier;
	}
	printf("adding\n");
	++nitems;
	rewrite = TRUE;
	char* message = "|status: OK|\n";
	write(fd_write,message,strlen(message));
	
	return TRUE;
}

// ------------------------------------- edit ------------------------------------
int edit(char *sn) {
	int n = atoi(sn);
	int i;
	struct carrier *ptr;
	struct data this_data;
	
	if (n > nitems) {
		sprintf(errmsg, "Cannot edit item %d.  Item numbers range from 1 to %d",n,nitems);
		return FALSE;
	}
	
	for (i = 1, ptr = first; i < n; ++i)
		ptr = ptr->next;
	
	this_data = ptr->theData;
	strncpy(this_data.theSubject,subject,30);
	this_data.theSubject[30] = '\0';
	strncpy(this_data.theBody,body,140);
	this_data.theBody[140] = '\0';
	this_data.theTime = time(NULL);
	ptr->theData = this_data;
	
	rewrite = TRUE;
	char* message = "|status: OK|\n";
	write(fd_write,message,strlen(message));
	return TRUE;
}
	
// ----------------------------------- writeData ---------------------------------
int writeData(void) {
	int i;
	struct carrier *ptr;
	struct data this_data;
	
	FILE *fp = fopen("mystore.dat", "wb");  // writing in binary
	if (!fp) {
		sprintf(errmsg, "Cannot open mystore.dat for writing.");
		return FALSE;
	}
		
	if (fwrite(&nitems, sizeof(int), 1, fp) != 1) {
		fclose(fp);
		sprintf(errmsg, "Cannot write the nitems element");
		return FALSE;
	}
	
	for (i = 0, ptr = first; i < nitems; ++i) {
		this_data = ptr->theData;
		if (fwrite(&this_data, sizeof(struct data), 1, fp) != 1) {
			fclose(fp);
			sprintf(errmsg, "Cannot write item: %d",i+1);
			return FALSE;
		}
		ptr = ptr->next;
	}
	
	fclose(fp);
	return TRUE;
}

// ------------------------------------- status ------------------------------
void status(void) {
	struct tm *tp;

	char *statusS = saveFormatted("|status: OK| ");
	char *versionS = saveFormatted("|version: %s| ",version);
	char *authorS = saveFormatted("|author: %s| ",author);

	char *nitemsS = saveFormatted("|nitems: %i| ", nitems);
	char *ans = saveFormatted("%s%s%s%s",statusS,versionS,authorS,nitemsS);

	if(nitems > 0) {
		tp = localtime(&(first->theData.theTime));
		char *timeSS = saveFormatted("|first-time: %d-%02d-%02d %02d:%02d:%02d| ",
			tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
		tp = localtime(&(last->theData.theTime));

		char *timeES = saveFormatted("|last-time: %d-%02d-%02d %02d:%02d:%02d| ",
			tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
		ans = saveFormatted("%s%s%s",ans,timeSS,timeES);
	}

	// make sure fd_write is opened already
	write(fd_write,ans,strlen(ans));


	return;
}

// ------------------------------------- rstrip ------------------------------
// removes the trailing whitespace 
char *rstrip(char *s) {
	char *p = s + strlen(s) - 1;
	while ((*p == ' ' || *p == '\t' || *p == '\n') && p >= s)
		--p;
	p[1] = '\0';
	return s;
}
//######################################################################

// ------------------------------------ display -----------------------------
int display(char *sn) {
	int n = atoi(sn);
	int i;
	struct carrier *ptr;
	struct data this_data;
	struct tm *tp;
		printf("Beep\n");


	if (n > nitems) {
		sprintf(errmsg, "Cannot display item %d.  Item numbers range from 1 to %d",n,nitems);
		return FALSE;
	}
	
	for (i = 1, ptr = first; i < n; ++i)
		ptr = ptr->next;
	
	this_data = ptr->theData;

	char *statusS = saveFormatted("|status: OK|\n");
	char *itemS = saveFormatted("|item: %d|\n",n);
	tp = localtime(&this_data.theTime);
	char *timeS = saveFormatted("|time: %d-%02d-%02d %02d:%02d:%02d|\n",
		tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
	//printf("|time: %s|\n",rstrip(ctime(&this_data.theTime)));

	char *subjectS = saveFormatted("|subject: %s|\n",this_data.theSubject);
	char *bodyS = saveFormatted("|body: %s|\n",this_data.theBody);
	char *ans = saveFormatted("%s%s%s%s%s",statusS,itemS,timeS,subjectS,bodyS);
	write(fd_write,ans,strlen(ans));
	return TRUE;
}

// ------------------------------------ delete ------------------------------
int delete(char *sn) {
	int n = atoi(sn);
	int i;
	struct carrier *ptr, *previous;

	
	if (n > nitems) {
		sprintf(errmsg, "Cannot delete item %d.  Item numbers range from 1 to %d",n,nitems);
		return FALSE;
	}
		
	previous = first;
	if (n == 1) {
		first = first->next;
		if (nitems == 1) last = NULL;
	}
	else {
		for (i = 2, ptr = first->next; i < n; ++i) {
			previous = ptr;
			ptr = ptr->next;
		}
		previous->next = ptr->next;
		if (n == nitems) last = previous;
	}
	
	--nitems;
	rewrite = TRUE;
	char* message = "|status: OK|";
	write(fd_write,message,strlen(message));
	return TRUE;

}

//------------------------------------- search ------------------------------
char *toLowerStr(char *str) {
	int i;
	for (i =0;str[i];i++) {
		str[i] = tolower(str[i]);
	}
	return str;
}
int contains(char *haystack, char *needle) {
	int ans = FALSE;
	for(;*haystack;haystack++) {
		char *h = haystack;
		char *n = needle;
		for(;*n;n++,h++) {
			if(*n != *h) {
					ans = FALSE;
					break;
			} else {
					ans = TRUE;
			}
		}
		if(ans)
			break;
	}
	return ans;
}
int containsLC(char *haystack, char *needle) {
	char *hsCpy = malloc(sizeof(char) * strlen(haystack));
	char *ndCpy = malloc(sizeof(char) * strlen(needle));
	strcpy(hsCpy,haystack);
	strcpy(ndCpy,needle);
	
	hsCpy = toLowerStr(hsCpy);
	ndCpy = toLowerStr(ndCpy);
	
	int ans = contains(hsCpy,ndCpy);

	free(hsCpy);
	free(ndCpy);
	
	return ans;
}
int search(char *subject) {
	int i;
	struct carrier *ptr;
	struct data this_data;
	struct tm *tp;
	char *ans = NULL;
	for(i = 1, ptr = first; i <= nitems; i++) { // change 1 to nitems
		if(containsLC(ptr->theData.theSubject,subject)) {	
			this_data = ptr->theData;

			char *statusS = saveFormatted("|status: OK|");
			char *itemS = saveFormatted("|item: %d|",i);
			tp = localtime(&this_data.theTime);
			char *timeS = saveFormatted("|time: %d-%02d-%02d %02d:%02d:%02d|",
				tp->tm_year+1900,tp->tm_mon+1,tp->tm_mday,tp->tm_hour,tp->tm_min,tp->tm_sec);
			char *subjectS = saveFormatted("|subject: %s|",this_data.theSubject);
			char *bodyS = saveFormatted("|body: %s|",this_data.theBody);
			char *out = saveFormatted("%s%s%s%s%s\n",statusS,itemS,timeS,subjectS,bodyS);
			if(ans) {
				ans = saveFormatted("%s%s",ans,out);
			} else {
				ans = saveFormatted(out);
			}
		}
		ptr = ptr->next;
	}
	printf("Writing %i bytes: %s\n(%i written)\n",strlen(ans),ans,write(fd_write,ans,strlen(ans)));

	return TRUE;
}

//-----------------------FIFO update-----------------------------
/*
||======\\	
||       ||	
||		 //	
||======//	
||			
||			
||			
||			

*/
	// ================================ Process ===========================
int Process(char *s) {
	printf("\n\n");
	char *fields[10];
	int nfields;
	//debug
	nfields = SeparateIntoFields(s, fields, 10); //tian : changed maxfiled from 3 to 10
	// do the commands:

	if (strcmp(fields[0],"quit") == 0){ 
		return -1;
	}else if (strcmp(fields[0],"return") == 0 && nfields <= 10) {
		printf("|%s|\n",fields[1]);

		if ((fd_write = open(fields[1],O_WRONLY)) < 0) {
			printf("Cannot write to %s\n", fields[1]);
		}
		else {
			parseArgsUtil(nfields - 2, &fields[2]);
			runCommand(nfields - 2,&fields[2]);
			//write(fd_write,Capital(fields[2]),strlen(fields[2]));
			close(fd_write);
			flushPool();
		}
	}
	else
		printf("Unrecognized command: %s %s %s\n", fields[0],fields[1],fields[2]);
	return 0;
}
// ================================ SeparateIntoFields ===================================
char *findToken(char *s,char token) {
	if(*s != token) {
		while(*s && *s != token && s++);
	}
	return s;
}
int SeparateIntoFields(char *s, char **fields, int max_fields) {
	int i;
	/*static char null_c = '\0';
	
	for (i = 0; i < max_fields; ++i) fields[i] = &null_c;
	//this code is illegible & glitchy
		//please dont write like this
		//its awful
	for (i = 0; i < max_fields; ++i) {
		while (*s && (*s == ' ' || *s == '\t' || *s == '\n')) ++s;	// skip whitespace
		if (!*s) return i;
		fields[i] = s;
		if (i == max_fields - 1) return i+1;
		while (*s && *s != ' ' && *s != '\t' && *s != '\n') ++s;	// skip non-whitespace
		if (!*s) return i+1;
		*s++ = '\0';
	}*/
	for(i = 0; i < max_fields; ++i) {
		s =findToken(s,' ');
		fields[i] = ++s;
		s = findToken(s,'|');
		*s = '\0';
		if(!*(++s)) {
			break;
		}
	}
	i += 1;
	int t = 0;
	printf("[%i:",i);
	for(t = 0;t < i; t++) {
		printf(" %s,",fields[t]);
	}
	printf("\n");
	return i;
}
// ============================ the_handler ==================
static void the_handler(int sig) {
	printf("Signal caught: fifo_server terminated by signal %d\n",sig);
	close(fd_read);
	close(fd_write);
	close(fd_writeTEMP);
	unlink(fifo_read);
	exit(0);
}
