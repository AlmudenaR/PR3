#ifndef __PERSON_H__
#define __PERSON_H__
#include "csv.h"
#include "date.h"

typedef struct _tPerson {
    char* document;
    char* name;
    char* surname;
    char* phone;
    char* email;
    char* address;
    char* cp;
    tDate birthday;
} tPerson;

typedef struct _tPeople {
    tPerson* elems;
    int count;
} tPeople;

// Initialize the people data
void people_init(tPeople* data);

// Initialize a person structure
void person_init(tPerson* data);

// Remove the data from a person
void person_free(tPerson* data);

// Remove the data from all persons
void people_free(tPeople* data);

// Parse input from CSVEntry
void person_parse(tPerson* data, tCSVEntry entry);

// Add a new person to people data
void people_add(tPeople* data, tPerson person);

// Remove a person from people data
void people_del(tPeople* data, const char *document);

// Return the position of a person with provided document. -1 if it does not exist
int people_find(tPeople data, const char* document);

// Print the people data
void people_print(tPeople data);

// Copy the data from the source to destination
void person_cpy(tPerson* destination, tPerson source);

// Return people lenght
int people_len(tPeople data);

#endif
