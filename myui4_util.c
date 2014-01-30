#include "myui4.h"

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
//sockets
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX_BUF		300

void gather_message(char *buffer, char **args, int nargs, int max_buf);

//----------
extern struct NameValue *nvs;
extern int n_nvs;

extern char input[MAX_BUF+1];
extern int n_input;

extern int nitems;
extern char subject[31];
extern char body[141];
extern char errmsg[80];

extern char* server_ip;
extern int portno;
// ----------------------------------------- ReadMystoreFromChild ---------------------------
void addArgument(char *store,char *argv) {
	sprintf(store,"%s %s|",store,argv);
}
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4) {
	//updated for sockets. removed fifo
	char send_message[MAX_BUF+1]
	
	// compose messageforserver
	sprintf(send_message, " return|");
	if(argv1) {
		addArgument(send_message,argv1);
		if(argv2) {
			addArgument(send_message,argv2);
			if(argv3) {
				addArgument(send_message,argv3);
				if(argv4) {
					addArgument(send_message,argv4);
				}
			}
		}
	}
	input[0] = '\0'; 
	
	if (send_to_server(server_ip, portno, send_message, input, MAX_BUF) < 0) {
		printf("Client: ERROR in send_to_server\n");
		return -1;
	}

	return n_input = sizeof(input);
}


// ----------------------------------------------- ParseInput -----------------------------
int ParseInput(char *in, int n_in) {
	int num_nvs, i_nvs;
	char *p;
	//printf("%s\n",in);
	//sleep(9999999);
	if (nvs != NULL) free(nvs);
	nvs = NULL;
	n_nvs = 0;
	
	if (n_in < 7)
		return 0;
	
	for (num_nvs = 0, p = in; *p; ++p) {
		if (*p == '|') ++num_nvs;
	}
	num_nvs /= 2; //two bars = 1 namevalue pair
	
	if ((nvs = calloc(num_nvs, sizeof(struct NameValue))) == NULL)
		return 0;
	
	for (i_nvs = 0, p = in; i_nvs < num_nvs; ++i_nvs) {
		// until record
		while (*p++ != '|')
			;
		// start of name
		nvs[i_nvs].name = p;
		while (*p != ':')
			p++;
		*p++ = '\0';
		// name finished, looking for value...
		while (*p == ' ')
			p++;
		nvs[i_nvs].value = p;
		while (*p != '|')
			p++;
		*p = '\0';
		// value finished
	}
	n_nvs = num_nvs;
	return n_nvs;
}
