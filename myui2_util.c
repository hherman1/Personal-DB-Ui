#include "myui2.h"

extern struct NameValue *nvs;
extern int n_nvs;

extern char input[1000];
extern int n_input;

extern int nitems;
extern char subject[31];
extern char body[141];
extern char errmsg[80];


// ----------------------------------------- ReadMystoreFromChild ---------------------------
int ReadMystoreFromChild(char *argv1, char *argv2, char *argv3, char *argv4) {
	int pid, mypipe[2], i;
	char *newargv[7];
	
	if (nvs != NULL) {
		free(nvs);
		nvs = NULL;
	}
	n_nvs = 0;
	n_input = 0;
	
	// turn off special keyboard handling
	getkey_terminate();
	
	// create the pipe
	if (pipe(mypipe) == -1) {
		strcpy(errmsg,"Problem in creating the pipe");
		return 0;
	}
	
	pid = fork();
	
	if (pid == -1) {
		strcpy(errmsg, "Error in forking");
		return 0;
	}
	if (pid == 0) {  // child
		close(mypipe[0]);  // Don't need to read from the pipe
		dup2(mypipe[1],STDOUT_FILENO);  // connect the "write-end" of the pipe to child's STDOUT
		
		for (i = 2; i < 7; ++i) newargv[i] = NULL;
		newargv[0] = newargv[1] = "./mystore";
		newargv[2] = argv1;
		newargv[3] = argv2;
		newargv[4] = argv3;
		newargv[5] = argv4;
		newargv[6] = NULL;
		
		execvp(newargv[0],newargv+1);
		exit(0);
	}
	else if (pid > 0) {
		char *s = input;
		int c, i, n;
		close(mypipe[1]);  // Don't need to write to the pipe
		
		// read the data into the input array
		FILE *fpin;
		if ((fpin = fdopen(mypipe[0],"r")) == NULL) {
			printf("ERROR: Cannot read from mypipe[0]\n");
			exit(1);
		}
		for (n_input = 0; n_input < sizeof(input)-1; ++n_input) {
			if ((c = getc(fpin)) == EOF) break;
			*s++ = c;
		}
		input[n_input] = '\0';
		fclose(fpin);
		
		wait(NULL);  // wait for child to finish
		close(mypipe[0]);
	}
	
	return n_input;
}


// ----------------------------------------------- ParseInput -----------------------------
int ParseInput(char *in, int n_in) {
	int num_nvs, i_nvs;
	char *p;
	
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
