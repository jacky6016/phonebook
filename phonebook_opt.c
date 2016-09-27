#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "phonebook_opt.h"

extern entry *hashTable[TABLE_SIZE];

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

entry *append(char lastName[], entry *e)
{
    /* allocate memory for the new entry and put lastName */
    int hashIndex = (int)(hash((unsigned char *)lastName) % TABLE_SIZE);

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


