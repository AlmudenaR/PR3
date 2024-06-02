#ifndef __DO_H__
#define __DO_H__

#include "winegrower.h"

#define NUM_FIELDS_DO 3

// Maximum length of DO name
#define MAX_DO_NAME_LENGTH 64

typedef struct _tDO { 
    char *code;
    char *name;
    tWinegrowerList winegrowers;
    double avgCropField;
    tVineyardplotData vineyards;
} tDO;

typedef struct _tDOData {
    tDO *elems;
    int count;
} tDOData;


// Initialize to NULL all pointers of a DO
void do_initEmpty(tDO* DO);

// Initialize a DO
tApiError do_init(tDO* DO, const char* code, const char* name, double avgCropField);

// Copy a DO
tApiError do_cpy(tDO* dst, tDO src);

// Release a DO
void do_free(tDO* DO);

// Initialize a DO data
void doData_init(tDOData* data);

// Release a DO data
void doData_free(tDOData* data);

// Return the number of DO in data
int doData_len(tDOData data);

// Insert a DO to the DO data
tApiError doData_add(tDOData* data, tDO DO);

// Find a DO in DO data
tDO* doData_find(tDOData data, const char* code);

// Parse input from CSVEntry
void do_parse(tDO* data, tCSVEntry entry);

// Get the total weight from an specific winegrower
double doData_getTotalWeightByWinegrower(tDO DO, const char* winegrowerId);

// Recursive version to get the total weight
double doData_getTotalWeightByWinegrower_recursive(tVineyardplotData vineyards, int index);

// Get the total weighing for an specific winegrower, specific vineyard and specific year
double doData_getTotalWeighingByWineGrowerAndVineyardByYear(tDO DO, const char* winegrowerId, const char* vineyardplotCode, int year);

// Recursive version to get the total weighing
double doData_getTotalWeighingByWineGrowerAndVineyardByYear_recursive(tWeighingNode *pNode, int year);

// Get the total weighing for a specific DO on a specific year
double do_getTotalWeighing(tDO DO, int year);

// Sort a DO data by the weighing in a given year
tDOData doData_orderByWeighing(tDOData* DOData, int year);



// AUXILIAR FUNCTIONS


void swap(tDO* a, tDO* b);
void quickSort(tDO* v, int begin, int end, int year, tDOData* originalDOData);
#endif
