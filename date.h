#ifndef __DATE_H__
#define __DATE_H__
#include <stdbool.h>

// Length of the date
#define DATE_LENGTH 10

typedef struct _tDate {    
    int day; 
    int month;
    int year;
} tDate;

typedef struct _tTime {
    int hour; 
    int minutes;
} tTime;

typedef struct _tDateTime {
    tDate date;
    tTime time;    
} tDateTime;

// Copy a date from src to dst
void date_cpy(tDate* dst, tDate src);

// Compare two tDate structures and return -1 if date1<date2, 0 if equals and 1 if date1>date2.
int date_cmp(tDate date1, tDate date2);

// Parse a tDate from string information
void date_parse(tDate* date, const char* text);

// Parse a tDateTime from string information
void dateTime_parse(tDateTime* dateTime, const char* date, const char* time);

// Compare two tDateTime structures and return -1 if dateTime1<dateTime2, 0 if equals and 1 if dateTime1>dateTime2.
int dateTime_cmp(tDateTime dateTime1, tDateTime dateTime2);

// Compare two tDateTime structures and return true if they contain the same value or false otherwise.
bool dateTime_equals(tDateTime dateTime1, tDateTime dateTime2);

#endif // __DATE_H__