#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "memory.h"
/*
Memory pool system
Necessitated by "asprintf" and its constant use
It still seems better than the alternative (albeit slower)
The alternative would be to use oversized arrays in place of precise ones
fuck that

using a pool and exactstrings might turn out really slow, but i dont see the unstdness of it
*/
struct Node *memPool = NULL;
void queueMem(void *mem) {
	struct Node *top = malloc(sizeof(struct Node));
	top->mem = mem;
	top->next = memPool;
	memPool = top;
}
void flushPool(void) {
	while(memPool) {
		void *temp = memPool;
		free(memPool->mem);
		memPool = memPool->next;
		free(temp);
	}
}

char *saveFormatted(char *format, ...) { //hunter
	int init = 1;
	char *ans = NULL;
	char *test = NULL;
	va_list args;
	va_start(args,format);
	int queue = vasprintf(&ans,format,args);
	va_end(args);
	if(queue > 0) queueMem(ans);
	else printf("%i: %s\n",queue,ans);
	return ans;
}