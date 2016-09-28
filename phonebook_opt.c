#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#include "phonebook_opt.h"

extern entry *hashTable[TABLE_SIZE];
extern pthread_mutex_t locks[TABLE_SIZE];

unsigned long hash(unsigned char *str) {
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

entry *findName(char lastName[], entry *pHead)
{
    int hashIndex = (int)(hash((unsigned char *)lastName) % TABLE_SIZE);
	
	if(hashTable[hashIndex]) {
		entry *tmp = hashTable[hashIndex];		
		while (strcasecmp(lastName, tmp->lastName) != 0) 
        	tmp = tmp->pNext;	
		return tmp;
	}
    return NULL;
}

entry *append(char lastName[], unsigned int hashIndex)
{
    /* allocate memory for the new entry and put lastName */
		
	/* empty slot*/
	if(!hashTable[hashIndex]) {
		hashTable[hashIndex] = (entry *) malloc(sizeof(entry));
		strcpy(hashTable[hashIndex]->lastName, lastName);
		hashTable[hashIndex]->pNext = NULL;
		return hashTable[hashIndex];
	}
	/* space taken. find the slot at the end of the list */
	else {
		entry *tmp = (entry *) malloc(sizeof(entry));
    	strcpy(tmp->lastName, lastName);
		tmp->pNext = hashTable[hashIndex];		
		hashTable[hashIndex] = tmp;
		return tmp;
	}
}


void thread_work(void *param)
{
	thread_data *td = (thread_data *)param;
	char line[MAX_LAST_NAME_SIZE];
	int i,j;

	for(i=0; i < td->tid; i++) {
		if(!fgets(line, sizeof(line), td->fp))
			break;
	}

	while (fgets(line, sizeof(line), td->fp)) {
        while (line[i] != '\0')
            i++;
        line[i - 1] = '\0';
        i = 0;
		unsigned int hashIndex = (unsigned int)(hash((unsigned char *)line) % TABLE_SIZE);

		/* critical section */
		pthread_mutex_lock(&locks[hashIndex]);
        append(line, hashIndex);
		pthread_mutex_unlock(&locks[hashIndex]);

		for(j=0; j < td->tnum-1; j++) {
			if(!fgets(line, sizeof(line), td->fp))
				break;
		}
    }
}

thread_data *pack_param(FILE *fp, int tid,int tnum)
{
	thread_data *td = (thread_data *)malloc(sizeof(thread_data));	
	td->fp = fp;
	td->tid = tid;
	td->tnum = tnum;
	
	return td;
}
