struct Node {
	struct Node *next;
	void *mem;
};

void queueMem(void *mem);
void flushPool(void);

char *saveFormatted(char *format,...);