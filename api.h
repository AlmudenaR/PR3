#ifndef __UOCHEALTHCENTER_API__H
#define __UOCHEALTHCENTER_API__H
#include <stdbool.h>
#include "error.h"
#include "csv.h"
#include "do.h"
#include "person.h"
#include "winegrower.h"


// Type that stores all the application data
typedef struct _ApiData {
    ////////////////////////////////
    // PR1 EX2a
    ////////////////////////////////
    // People
    tPeople people;	
	// Winegrowers
    tWinegrowerList winegrowers;
    ////////////////////////////////
	
	////////////////////////////////
    // PR2 EX3a
    tDOData DOs;
    ////////////////////////////////
} tApiData;

// Get the API version information
const char* api_version();

// Load data from a CSV file. If reset is true, remove previous data
tApiError api_loadData(tApiData* data, const char* filename, bool reset);

// Add a new entry
tApiError api_addDataEntry(tApiData* data, tCSVEntry entry);

// Free all used memory
tApiError api_freeData(tApiData* data);

// Initialize the data structure
tApiError api_initData(tApiData* data);

// Add a new winegrower
tApiError api_addWinegrower(tApiData* data, tCSVEntry entry);

// Add a new vineyardplot
tApiError api_addVineyardplot(tApiData* data, tCSVEntry entry);

//Add weighing in a vineyardplot
tApiError api_addWeighing(tApiData* data, tCSVEntry entry);

// Add a new DO
tApiError api_addDO(tApiData* data, tCSVEntry entry);

// Find a Winegrower in the list of Winegrowers
tWinegrower* apiWinegrower_find(tApiData* data, const char* id);

// Get the number of people registered on the application
int api_peopleCount(tApiData data);

// Get the number of winegrowersregistered on the application
int api_winegrowersCount(tApiData data);

// Get the number of vineyardplots in all winegrowers registered on the application
int api_vineyardplotCount(tApiData data);

// Get the number of DOs redistered on the application
int api_DOCount(tApiData data);

// Get winegrower data
tApiError api_getWinegrower(tApiData data, const char *id, tCSVEntry *entry);

// Get registered winegrowers
tApiError api_getWinegrowers(tApiData data, tCSVData *winegrowers);

// Get vineyardplot
tApiError api_getVineyardplot(tApiData data, const char* vineyardCode, tCSVEntry *entry);

// Get registered vineyardsplots
tApiError api_getVineyardplots(tApiData data, tCSVData *vineyards);


#endif // __UOCHEALTHCENTER_API__H