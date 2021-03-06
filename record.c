#include "myui2.h"
#include "record.h"
#include "color.h"
extern struct NameValue *nvs;
extern int n_nvs;

extern struct ColorScheme defaultColors;

int recordSelected = -1;
//--------------------------Display----------------------------
void displayRecords(Record hovered,struct RecordList *buffer,Area rArea,struct RecordColorScheme *scheme) {

	Record *bot = adjustBufferForArea(hovered,buffer,rArea);
	printViewBuffer(hovered,buffer,rArea,scheme);

}

void printViewBuffer(Record hovered, struct RecordList *buffer, Area rArea,struct RecordColorScheme *scheme) {
	int i = 0;
	int width = rArea.right - rArea.left;
	char *color = "";
	Record *temp = buffer->top;		
	struct RecordColorScheme currentCS = *scheme;	
	while(temp && temp->num <= buffer->bottom->num){

		//color = R_TEXT_COLOR;
		displayColor(currentCS.normal.subject);
		int row = i + rArea.top;
		char numString[RECORD_NUM_SPACE+1];
		numString[RECORD_NUM_SPACE] = '\0';
		snprintf(numString,RECORD_NUM_SPACE,"%i.",temp->num);

		if(temp->num == hovered.num) {
			displayColor(currentCS.hovered.subject);
		}	

		if(temp->num == recordSelected) {
			displayColor(currentCS.selected.subject);
			if(temp->num == hovered.num) {
				displayColor(currentCS.selectedHovered.subject);
			}
			DisplayAt(row,rArea.left,color,RECORD_NUM_SPACE,numString);
			DisplayAt(row,rArea.left+RECORD_NUM_SPACE,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
			int spaceNeeded = requiredSpace(*temp,width);//strlen(sTemp) / width + 1;
			i += spaceNeeded;

			xt_par2(XT_SET_ROW_COL_POS,row,rArea.left);
			displayColor(currentCS.selected.body);
			if(temp->num == hovered.num) {
				displayColor(currentCS.selectedHovered.body);
			}
			wrapText(RECORD_NUM_SPACE,width,temp->body);
		}
		else if(row <= rArea.bot) {
			DisplayAt(row,rArea.left,color,RECORD_NUM_SPACE,numString);
			DisplayAt(row,rArea.left+RECORD_NUM_SPACE,color,MAX_SUBJECT_LEN,temp->subject);
			DisplayAt(row,rArea.right-MAX_TIME_LEN,color,MAX_TIME_LEN,temp->time);
		}


		temp = temp->next;
		displayColor(defaultColors);
		i++;	


	}

}
void trimBuffer(struct RecordList *buffer, Record *cutoff) {
	buffer->bottom = cutoff;
	while(buffer->bottom->next) {
		freeRecord(buffer->bottom->next);
	}
}
Record* adjustBufferForArea(Record hovered,struct RecordList *buffer, Area rArea) {
	int availableSpace;
	Record *current = buffer->top;
	Record *ans = NULL;

	if(current) {
		for(availableSpace = rArea.bot - rArea.top;availableSpace > 0 ;availableSpace--) {
			if(current->num == recordSelected){
				int openSpace = requiredSpace(*current,rArea.right - rArea.left);
				availableSpace -= openSpace;
				if(current->num == hovered.num) {
					while(availableSpace <= 0) {
						buffer->top =nextRecord(buffer->top);
						availableSpace++;
					}
				}
				else if(availableSpace<= 0){
					//scrollUp(buffer);
					while(availableSpace <= 0) {
						if(buffer->top->num - 1)
							buffer->top = previousRecord(buffer->top);
						availableSpace++;
						buffer->lengthfrombot++;
					}
					//buffer->lengthfrombot ++;
					current = current->prev;
				}
			}
			if(current->next && availableSpace - 1) {
				if(current == buffer->bottom) {
					buffer->lengthfrombot--;
					buffer->bottom = current->next;
				}
				current = current->next;
			} else if(buffer->lengthfrombot && availableSpace - 1){
				scrollDown(buffer);
				buffer->top = previousRecord(buffer->top);
				current = current->next;
			} else {
				break;
			}
			//buffer->bottom = current;
		}
		ans = current;
		for(;current->num != buffer->bottom->num;current = current->next) {
			buffer->lengthfrombot++;
		}
		buffer->bottom = ans;
	}


	return ans;
}
int getRecordY(Record *r, struct RecordList *buffer,Area rArea) {
	int availableSpace;
	Record *current = buffer->top;
	int ans = rArea.top;
	int found = 0;
	printf("\n\n\n");
	if(current) {
		for(availableSpace = rArea.bot - rArea.top;availableSpace > 0 ;availableSpace--) {

			if(current->num == recordSelected){
				int openSpace = requiredSpace(*current,rArea.right - rArea.left);
				availableSpace -= openSpace;
				if(current->num == r->num) {
					while(availableSpace <= 0) {
						buffer->top =nextRecord(buffer->top);
						availableSpace++;
						ans--;
					}
				}
				else if(availableSpace<= 0){
					//scrollUp(buffer);
					while(availableSpace <= 0) {
						if(buffer->top->num - 1)
							buffer->top = previousRecord(buffer->top);
						availableSpace++;
						ans--;
						buffer->lengthfrombot++;
					}
					//buffer->lengthfrombot ++;
					current = current->prev;
				}
				found = 1;
			}
			if(current->next && availableSpace - 1) {
				if(current == buffer->bottom) {
					buffer->lengthfrombot--;
					buffer->bottom = current->next;
				}
				current = current->next;
			} else if(buffer->lengthfrombot && availableSpace - 1){
				scrollDown(buffer);
				buffer->top = previousRecord(buffer->top);
				current = current->next;
			} else {
				break;
			}
			//buffer->bottom = current;
			if(!found)
				ans++;
		}
		Record *temp = current;
		for(;current->num != buffer->bottom->num;current = current->next) {
			buffer->lengthfrombot++;
		}
		buffer->bottom = temp;

	}
	return ans;

}
int requiredSpace(Record r,int width) {
	return strlen(r.body) / (width) + 1;
}
void wrapText(int left,int width, char *text) {
	int i = 0;
	while(text[i] != '\0') {
		if(i%width == 0) {
			putchar('\n');
			int q = 0;
			while(q++ < left) {
				putchar(' ');
			}
		}
		putchar(text[i]);
		i++;
	}

}
void selectRecord(int record){
	int rNum = record;
	if(rNum == recordSelected) {
		recordSelected = 0;
	} else {
		recordSelected = rNum;
	}
}
//--------------------------Operations-------------------------
void loadRecords(struct RecordList *buffer,int low,int high,int number) {
	Record *temp = getRecord(low);
	bufferRecord(buffer,temp);
	while(++low < high && low <= number) {
		temp = getRecord(low);
		bufferRecord(buffer,temp);
	}

}
Record *getRecord(int r) {
	ParseRecord(r);
	return allocateTopRecord();
}
Record *allocateTopRecord() {
	Record *ans = malloc(sizeof(Record));
	char *temp = NULL;
	temp = searchNvs("subject");
	if(temp) {
		ans->subject = malloc(sizeof(char) * MAX_SUBJECT_LEN);
		strcpy(ans->subject,temp);
		temp = searchNvs("body");
		ans->body = malloc(sizeof(char) * MAX_BODY_LEN);
		strcpy(ans->body,temp);
		temp = searchNvs("time");
		ans->time = malloc(sizeof(char) * MAX_TIME_LEN);
		strcpy(ans->time,temp);

		ans->num = atoi(searchNvs("item"));
		ans->prev = NULL;
		ans->next = NULL;
	}
	return ans;
}
Record *popRecord() {
	Record *ans = allocateTopRecord();
	nvs++;
	n_nvs--;
	for(;n_nvs; n_nvs--) {
		if(strcmp(nvs->name,"status") == 0) {
			break;
		}
		nvs++;
	}
	return ans;
}
Record *findRecord(struct RecordList buffer,int num) {
	Record *curL = buffer.top;
	Record *curR = buffer.top;
	while(curL = curL->prev) {
		if(curL->num == num)
			return curL;
	}
	while(curR = curR->next) {
		if(curR->num == num)
			return curR;
	}
	return NULL;
}
//-------------------Map------------------------------------------------------------------------------------------------------------------

void mapNum(int n, struct RecordList *buffer) {
	mapNextwards(n,buffer->top->next);
	mapPrevwards(n,buffer->top->prev);
	buffer->top->num += n;
}
void mapNextwards(int n, Record *r) {
	while(r) {
		r->num += n;
		r = r->next;
	}
}
void mapPrevwards(int n, Record *r) {
	while(r) {
		r->num += n;
		r = r->next;
	}
}
//-------------------DELETE------------------------------------------------------------------------------------------------------------------
void removeRecordFromBuffer(Record *r, struct RecordList *buffer) {
	if(buffer->top == r) buffer->top = buffer->top->next;
	if(buffer->bottom == r) buffer->bottom = buffer->bottom->prev;
	removeRecord(r);
}
void removeRecord(Record *r) {
	mapNextwards(-1,r);
	freeRecord(r);
}
void freeBuffer(struct RecordList *buffer) {
	while(buffer->top->next) freeRecord(buffer->top->next);
	while(buffer->top->prev) freeRecord(buffer->top->prev);
	freeRecord(buffer->top);
	buffer->top = NULL;
	buffer->bottom = NULL;
}
void freeRecord(Record *target) {
	int i = 0;
	if(target->body){
		free(target->body);
	}
	if(target->time){
		free(target->time);
	}
	if(target->subject){
		free(target->subject);
	}
	if(target->prev) {
		target->prev->next = target->next;
	}
	if(target->next != NULL) {
		target->next->prev = target->prev;
	}
	
	free(target);
}
//-------------------add------------------------------------------------------------------------------------------------------------------

void bufferRecord(struct RecordList *buffer,Record *r) {
	if(r == NULL) {
		printf("ERROR: Record does not exist\n");
	}
	else if(buffer->bottom != NULL) {
		r->prev = buffer->bottom;
		buffer->bottom->next = r;
		buffer->bottom = buffer->bottom->next;
	} else {
		buffer->bottom = r;
		buffer->top = r;
	}
}

//must parseRecord for this
void addBufferTop(struct RecordList *buffer,Record *r) { //r == new record
	if(buffer->bottom != NULL) {
		r->next = buffer->top;
		buffer->top->prev = r;
		buffer->top = r;
		buffer->bottom = buffer->bottom->prev;
		if(buffer->bottom->next != NULL){
			freeRecord(buffer->bottom->next);
		}
	}else {
		printf("%s\n", "can't shiftBufferUp");
	}
}
void addBufferBot(struct RecordList *buffer,Record *r){
	bufferRecord(buffer,r);
	buffer->top = buffer->top->next;
	if(buffer->top->prev != NULL){
		freeRecord(buffer->top->prev);
	}
}
int bufferLength(struct RecordList buffer) {
	return buffer.bottom->num - buffer.top->num;
}
Record *previousRecord(Record *r) {
	if(!r->prev){
		int prev = r->num - 1;
		r->prev = getRecord(prev);
		r->prev->next = r;
	}
	r = r->prev;
	return r;
}
Record *nextRecord(Record *r) {
	if(!r->next){
		int next = r->num + 1;
		r->next = getRecord(next);
		r->next->prev = r;
	}
	r = r->next;
	return r;
}
void scrollUp(struct RecordList *buffer){
	buffer->bottom = previousRecord(buffer->bottom);
	buffer->top = previousRecord(buffer->top);
	buffer->lengthfrombot++;
}
void scrollDown(struct RecordList *buffer){
	buffer->bottom = nextRecord(buffer->bottom);
	buffer->top = nextRecord(buffer->top);
	buffer->lengthfrombot--;
}

