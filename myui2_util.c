#include "myui2.h"

#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

extern struct NameValue *nvs;
extern int n_nvs;

extern char input[1000];
extern int n_input;

extern int nitems;
extern char subject[31];
extern char body[141];
extern char errmsg[80];


// ----------------------------------------- ReadMystoreFromChild ---------------------------
void addArgument(char *store,char *argv) {
	sprintf(store,"%s %s|",store,argv);
}
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4) {
	//update
	char *fifo_write = "/tmp/fifo_server.dat";
	char fifo_read[40];
	char send_message[300], read_message[300];
	int fd_write, fd_read, n_read;
	
	// create and open the client's own FIFO for reading
	sprintf(fifo_read, "/tmp/fifo_client_%d.dat",getpid());
	if (mkfifo(fifo_read,0666) != 0) {
		perror("client mkfifo failed, returns: ");
		return -1;
	}
	//int test = open(fifo_read,O_WRONLY);
	//fprintf(test,"|status: OK| |nitems: 0|");
	

	// open the server's FIFO for writing
	if ((fd_write = open(fifo_write, O_WRONLY)) < 0) {
		perror("Cannot open FIFO to server: ");
		return -1;
	}

	// compose and send write message to server's FIFO
	sprintf(send_message, " return|");
	addArgument(send_message,fifo_read);
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

	write(fd_write,send_message,strlen(send_message));
	close(fd_write);
	
	// open the client's FIFO for reading
	if ((fd_read = open(fifo_read, O_RDONLY)) < 0) {
		perror("Cannot open FIFO to read from server: ");
		return -1;
	}
	// read server's reply in client's FIFO
	int next = 1000;
	do {
		n_input = read(fd_read,&input,1000); /////CHANGED read_message to input		
	} while(n_input == 1000);
	
	if (n_input >= 0) input[n_input] = '\0';
	
	// close and delete client's FIFO
	close(fd_read);
	unlink(fifo_read);
	
	return n_input;
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
