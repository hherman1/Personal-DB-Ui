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

#define version "0.92" //base version
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

//sockets start
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define PORT_NUMBER		51000		// the server's default port number

// Messages sent to client:
#define HUH			"Huh?\n"
#define EMPTY_MSG	"Empty message received\n"
#define QUITTING	"Server agrees to quit\n"

int portno = PORT_NUMBER;
int master_sockfd, current_sockfd;
// Prototypes:
void server_start();
void server_stop();
int send_to_server(char *server_name, int portno, char *send_buffer, char *receive_buffer, int max_buf);
//----------------------------------------------------------------------------------------------------------------------sockets end
#define MAX_BUF		10000
void gather_message(char *buffer, char **args, int nargs, int max_buf);
int send_to_server(char *server, int portno, char *send_buffer, char *receive_buffer, int max_buf);
/////////////////////
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
	struct sigaction sigHandler;
	sigHandler.sa_handler = the_handler;
	sigaction(SIGINT,&sigHandler,NULL);

	//sockets
	if (strcmp(argv[argc-1],"start") != 0 && strcmp(argv[argc-1],"stop") != 0) {		// print help
		printf("Creates or controls a server on this computer at port %d\n",PORT_NUMBER);
		printf("Usage: sockets_server {start | stop}\n   or   sockets_server {portno} {start | stop}\n");
		printf("Send a string to the server, (whose first char is \"u\") and get it back in UPPER-CASE\n");
		printf("If first char is \"q\", then we're telling the server to quit\n");
		printf("For any other string, server returns \"Huh?\"\n");
		return 0;
	}
	
	if (argc == 3)
		portno = atoi(argv[1]);
		
	if (strcmp(argv[argc-1],"start") == 0)
		server_start();
	else
		server_stop();

	return 0;
}

// ------------------------------- parseArgs() -------------------------------
//note parsargs has been replaced by server_start in this new socket version

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
	write(current_sockfd,message,strlen(message));
	
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
	write(current_sockfd,message,strlen(message));
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
	write(current_sockfd,ans,strlen(ans));


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
	write(current_sockfd,ans,strlen(ans));
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
	write(current_sockfd,message,strlen(message));
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
	char **noResults = NULL;
	printf("prepare to search\n");
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
	printf("writing %p\n",ans);
	if(!ans) {
		ans = malloc(sizeof(char));
		*ans = '\0';
		noResults = ans;
	}
	printf("Writing %i bytes: %s\n(%i written)\n",strlen(ans),ans,write(current_sockfd,ans,strlen(ans)));
	if(noResults) {
		free(*noResults);
	}
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
	nfields = SeparateIntoFields(s, fields, 10); //tian : changed maxfield from 3 to 10
	// do the commands:

	if (strcmp(fields[0],"quit") == 0){ 
		return -1;
	}else if (strcmp(fields[0],"return") == 0 && nfields <= 10) {
		printf("|%s|\n",fields[1]);

		parseArgsUtil(nfields - 1, &fields[1]); //take out the "return" keyword
		runCommand(nfields - 1,&fields[1]);
		flushPool();
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
	printf("Signal caught: server terminated by signal %d\n",sig);
	server_stop();
	exit(0);
}

//==================================SOCKETS==================================================
// ---------------------------------------- server_start -------------------------------------------
void server_start() {
	socklen_t client_len;
	struct sockaddr_in serv_addr, client_addr;
	char buffer[301];
	int nread;
	
	// Create master socket:
	if ((master_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Server: Cannot create master socket.");
		exit(-1);
	}
	
	// create socket structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	
	// bind the socket to the local port
	if (bind(master_sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Server: Error on binding");
		exit(1);
	}
	
	// listen
	listen(master_sockfd, 5);
	
	client_len = sizeof(client_addr);
	printf("Server listening on port %d\n", portno);
	
	// master loop
	while(TRUE) {
		// block until a client connects
		if ((current_sockfd = accept(master_sockfd, (struct sockaddr *) &client_addr, &client_len)) < 0) {
			perror("Server: Error on accept()");
			exit(1);
		}
		
		nread = read(current_sockfd, buffer, 300);
		if (nread > 0) {
			buffer[nread] = '\0';
			// Quit command received?
			if (Process(buffer) == -1) { 
				write(current_sockfd, QUITTING, sizeof(QUITTING));
				close(current_sockfd);
				close(master_sockfd);
				printf("Server quitting...\n");
				exit(0);
			}
		}
		else {	// zero length message?
			write(current_sockfd, EMPTY_MSG, sizeof(EMPTY_MSG));
			close(current_sockfd);
			printf("Server: empty message received\n");
		}
	}
}

// ------------------------------------------- server_stop ------------------------------------------
void server_stop() {
	char receive_buffer[100];
	
	send_to_server("localhost",portno,"q", receive_buffer, 100);
}
