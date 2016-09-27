#ifndef _PHONEBOOK_H
#define _PHONEBOOK_H

#define MAX_LAST_NAME_SIZE 16

#define OPT 1

typedef struct __ENTRY_DETAIL {
	char firstName[16];
    char email[16];
    char phone[10];
    char cell[10];
    char addr1[16];
    char addr2[16];
    char city[16];
    char state[2];
    char zip[5];
} entry_detail;

typedef struct __PHONE_BOOK_ENTRY {
    char lastName[MAX_LAST_NAME_SIZE];
	entry_detail *detail;
    struct __PHONE_BOOK_ENTRY *pNext;
} entry;


entry *findName(char lastName[], entry *pHead);
entry *append(char lastName[], entry *e);

#endif
