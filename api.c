#include <stdio.h>
#include <assert.h>
#include "csv.h"
#include "api.h"

#include <string.h>
#include "person.h"
#include "winegrower.h"
#include "vineyardplot.h"


#define FILE_READ_BUFFER_SIZE 2048

// Get the API version information
const char* api_version() {
    return "UOC PP 20232";
}

// Load data from a CSV file. If reset is true, remove previous data
tApiError api_loadData(tApiData* data, const char* filename, bool reset) {
    tApiError error;
    FILE *fin;    
    char buffer[FILE_READ_BUFFER_SIZE];
    tCSVEntry entry;
    
    // Check input data
    assert( data != NULL );
    assert(filename != NULL);
    
    // Reset current data    
    if (reset) {
        // Remove previous information
        error = api_freeData(data);
        if (error != E_SUCCESS) {
            return error;
        }
        
        // Initialize the data
        error = api_initData(data);
        if (error != E_SUCCESS) {
            return error;
        }
    }

    // Open the input file
    fin = fopen(filename, "r");
    if (fin == NULL) {
        return E_FILE_NOT_FOUND;
    }
    
    // Read file line by line
    while (fgets(buffer, FILE_READ_BUFFER_SIZE, fin)) {
        // Remove new line character     
        buffer[strcspn(buffer, "\n\r")] = '\0';
        
        csv_initEntry(&entry);
        csv_parseEntry(&entry, buffer, NULL);
        // Add this new entry to the api Data
        error = api_addDataEntry(data, entry);
        if (error != E_SUCCESS) {
            return error;
        }
        csv_freeEntry(&entry);
    }
    
    fclose(fin);
    
    return E_SUCCESS;
}

// Initialize the data structure
tApiError api_initData(tApiData* data) {            
    //////////////////////////////////
    // Ex PR1 2b
    /////////////////////////////////
    // Check input data structure
    assert(data != NULL);
    
    // Initialize data structures
    people_init(&(data->people));
    winegrowerList_init(&(data->winegrowers));

    ////////////////////////////////
    // Ex PR2 3b
    doData_init(&(data->DOs));
    ////////////////////////////////
    
    return E_SUCCESS;
    
    /////////////////////////////////
    // return E_NOT_IMPLEMENTED;
}

// Find a Winegrower in the list of Winegrowers
tWinegrower* apiWinegrower_find(tApiData* data  , const char* id){
    //////////////////////////////////
    // Ex PR1 2c
    /////////////////////////////////
   tWinegrowerList list = data->winegrowers;
   tWinegrowerNode *pNode = NULL;
   tWinegrowerNode *pWinegrower= NULL;
        
    // Point the first element
    pNode = list.first;
    
    while(pNode != NULL && pWinegrower == NULL) {
        // Compare current node with given id
        if (strcmp(pNode->winegrower.id, id) == 0) {
            pWinegrower = pNode;
        }
        pNode = pNode->next;
    }
    
    return &(pWinegrower->winegrower);        

}

tApiError api_addWinegrower(tApiData* data, tCSVEntry entry) {
    //////////////////////////////////
    // Ex PR1 2d
    /////////////////////////////////
    tWinegrower winegrower;
    tVineyardplot vineyardplot;
    tWinegrower *pWinegrower;
    
    // Check input data structure
    assert(data != NULL);
    
    // Check the entry type
    if (strcmp(csv_getType(&entry), "WINEGROWER") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_WINEGROWER) {
        return E_INVALID_ENTRY_FORMAT;
    }
    
    // Parse the entry
     winegrower_parse(&winegrower, &vineyardplot, entry);

        
    // Check vineyardplot code
    if (!check_vineyard_code(vineyardplot.code)) {
        // Release temporal data
        winegrower_free(&winegrower);
        vineyardplot_free(&vineyardplot);
        return E_INVALID_VINEYARD_CODE;
    }
    
    // Check if winegrower exists
    pWinegrower = apiWinegrower_find(data, winegrower.id);
    if (pWinegrower == NULL) {
        // Add the winegrower
        winegrowerList_insert(&data->winegrowers, winegrower);
        pWinegrower = apiWinegrower_find(data, winegrower.id);
    }
    assert(pWinegrower != NULL);
    
    // Check if vineyardplot exists
    if (vineyardplotData_find(pWinegrower->vineyardplots, vineyardplot.code) == -1) {
        // Add the vineyardplot
        vineyardplotData_add(&(pWinegrower->vineyardplots), vineyardplot);
    }
 
    // Release temporal data
    winegrower_free(&winegrower);
    vineyardplot_free(&vineyardplot);
    
    return E_SUCCESS;
}


tApiError api_addVineyardplot(tApiData* data, tCSVEntry entry) {
    //////////////////////////////////
    // Ex PR1 2e
    /////////////////////////////////
    char winegrowerId[WINEGROWERS_ID_LENGTH + 1];

    tVineyardplot vineyardplot;
    tWinegrower *pWinegrower;
    
    // Check input data structure
    assert(data != NULL);
    
    // Check the entry type
    if (strcmp(csv_getType(&entry), "VINEYARD_PLOT") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_VINEYARD_PLOT) {
        return E_INVALID_ENTRY_FORMAT;
    }
    
    
    // Parse the entry
    
    csv_getAsString(entry, 0, winegrowerId, WINEGROWERS_ID_LENGTH + 1);        

    vineyardplot_parse( &vineyardplot, entry);

        
    // Check vineyardplot code
    if (!check_vineyard_code(vineyardplot.code)) {
        vineyardplot_free(&vineyardplot);
        return E_INVALID_VINEYARD_CODE;
    }
    
    // Check if winegrower exists
    pWinegrower = apiWinegrower_find(data, winegrowerId);
    if (pWinegrower == NULL) {
        vineyardplot_free(&vineyardplot);
        return E_WINEGROWER_NOT_FOUND;
    }
    
    // Check if vineyardplot exists
    if (vineyardplotData_find(pWinegrower->vineyardplots, vineyardplot.code) == -1) {
        // Add the vineyardplot
        vineyardplotData_add(&(pWinegrower->vineyardplots), vineyardplot);
    } else {
        vineyardplot_free(&vineyardplot);
        return     E_DUPLICATED_VINEYARD;
    }
 
    // Release temporal data
    vineyardplot_free(&vineyardplot);
    
    return E_SUCCESS;
}

//Add weighing in a vineyardplot
tApiError api_addWeighing(tApiData* data, tCSVEntry entry) {
    //////////////////////////////////
    // Ex PR3 EX 4d
    /////////////////////////////////
    tWeighing weighing;
    tWinegrower *pWinegrower;
//    tVineyardplot vineyardplot;
//    tWinegrower *pWinegrower;
    int i,j;
    
    // Check input data structure
    assert(data!=NULL);
    
    // Initialize an empty weighing to avoid possible memory errors with weighing_parse
    //do_initEmpty(&DO);
    
    // Check the entry type
    if (strcmp(csv_getType(&entry), "WEIGHING") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_WEIGHING) {
        return E_INVALID_ENTRY_FORMAT;
    }
    
    // Parse the entry
    weighing_parse(&weighing, entry);



    //vineyardplotData_find  int vineyardplotData_find(tVineyardplotData data, const char* code);
    
    //tWeighingNode* weighingList_findNode(tWeighingList list, const char* code, tDate harvestDay);

    //weighingList_add  tApiError weighingList_add(tWeighingList* list, tWeighing weighing);
}

// Add a new DO
tApiError api_addDO(tApiData* data, tCSVEntry entry) {
    tDO DO;
    tApiError error;
    
    // Check input data structure
    assert(data != NULL);
    
    // Initialize an empty DO to avoid possible memory errors with do_parse
    do_initEmpty(&DO);
    
    // Check the entry type
    if (strcmp(csv_getType(&entry), "DO") != 0) {
        return E_INVALID_ENTRY_TYPE;
    }
    
    // Check the number of fields
    if(csv_numFields(entry) != NUM_FIELDS_DO) {
        return E_INVALID_ENTRY_FORMAT;
    }
    
    // Parse the entry
    do_parse(&DO, entry);
    
    // Add to the structure
    if (doData_find(data->DOs, DO.code) == NULL) {
        error = doData_add(&(data->DOs), DO);
        
        if (error != E_SUCCESS) {
            do_free(&DO);
            return error;
        }
    } else {
        do_free(&DO);
        return E_DUPLICATED_DO;
    }
    
    // Release temporal data
    do_free(&DO);
    
    return E_SUCCESS;
}


// Get the number of people registered on the application
int api_peopleCount(tApiData data) {
    //////////////////////////////////
    // Ex PR1 2f
    /////////////////////////////////
    return people_len(data.people);
    /////////////////////////////////
    //return -1;
}


// Get the number of winegrowers registered on the application
int api_winegrowersCount(tApiData data) {
    //////////////////////////////////
    // Ex PR1 2f
    /////////////////////////////////
    return winegrowerList_len(data.winegrowers);    
    /////////////////////////////////
    //return -1;
}

// Get the number of vineyardplots in all winegrowers registered on the application
int api_vineyardplotCount(tApiData data) {
    //////////////////////////////////
    // Ex PR1 2f
    /////////////////////////////////
    return winegrowerList_vineyardplots_total(data.winegrowers);    
    /////////////////////////////////
    //return -1;
}

// Get the number of DOs redistered on the application
int api_DOCount(tApiData data) {
    //////////////////////////////////
    // Ex PR2 3e
    return doData_len(data.DOs);
    /////////////////////////////////
    //return -1;
}

// Free all used memory
tApiError api_freeData(tApiData* data) {
    //////////////////////////////////
    // Ex PR1 2g
    /////////////////////////////////
    people_free(&(data->people));
    winegrowerList_free(&(data->winegrowers));
    /////////////////////////////////
    
    ////////////////////////////////
    // Ex PR2 3d
    doData_free(&(data->DOs));
    ////////////////////////////////
    
    return E_SUCCESS;
    //return E_NOT_IMPLEMENTED;
}


// Add a new entry
tApiError api_addDataEntry(tApiData* data, tCSVEntry entry) { 
    //////////////////////////////////
    // Ex PR1 2h
    /////////////////////////////////
    tPerson person;
        
    assert(data != NULL);

    if (strcmp(csv_getType(&entry), "PERSON") == 0) {
        // Add person
        // Initialize the person object
        person_init(&person);

        // Check the number of fields
        if(csv_numFields(entry) != 8) {
            return E_INVALID_ENTRY_FORMAT;
        }
        // Parse the data
        person_parse(&person, entry);
        
        // Check if this person already exists
        if (people_find(data->people, person.document) >= 0) {
            // Release person object
            person_free(&person);
            return E_DUPLICATED_PERSON;
        }
        
        // Add the new person
        people_add(&(data->people), person);
        
        // Release person object
        person_free(&person);
        
 
    } else if (strcmp(csv_getType(&entry), "WINEGROWER") == 0) {
        // Add winegrower
        return api_addWinegrower(data, entry);        
    } else if (strcmp(csv_getType(&entry), "VINEYARD_PLOT") == 0) {
        // Add vineyard plot
        return api_addVineyardplot(data, entry);
    } 
    //////////////////////////////////
    // Ex PR2 3c
    /////////////////////////////////
    else if (strcmp(csv_getType(&entry), "DO") == 0) {
        // Add DO
        return api_addDO(data, entry);
    }
    //////////////////////////////////
    // PR3 EX 4e
    /////////////////////////////////
    else if(strcmp(csv_getType(&entry), "WEIGHING") == 0) {
        // Add WEIGHING
        return api_addWeighing(data, entry);
        
    }

    /////////////////////////////////
    else {
        return E_INVALID_ENTRY_TYPE;
    }
    
    return E_SUCCESS;
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Get winegrower data
tApiError api_getWinegrower(tApiData data, const char *id, tCSVEntry *entry) {
    //////////////////////////////////
    // Ex PR1 3a
    /////////////////////////////////
    char buffer[2048];
    tWinegrower* winegrower = NULL;
        
    assert(id != NULL);
    assert(entry != NULL);
    
    // Search winegrower
    winegrower = apiWinegrower_find(&data, id);
    
    if (winegrower == NULL) {
        return E_WINEGROWER_NOT_FOUND;
    }
    
    // Print data in the buffer
    sprintf(buffer, "%s;%s;%02d/%02d/%04d", winegrower->id, winegrower->document,  winegrower->registrationDate.day, winegrower->registrationDate.month, winegrower->registrationDate.year);
    // Initialize the output structure
    csv_initEntry(entry);
    csv_parseEntry(entry, buffer, "WINEGROWER");
    
    return E_SUCCESS;
    
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED; 
}

// Get vineyardplot
tApiError api_getVineyardplot(tApiData data, const char* vineyardCode, tCSVEntry *entry) {
    //////////////////////////////////
    // Ex PR1 3b
    /////////////////////////////////
    char buffer[2048];    
    tWinegrower *pWinegrower= NULL; 
    int idx;

    assert(vineyardCode != NULL);
    assert(entry != NULL);
    
    // Check vineyardCode code
    if (!check_vineyard_code(vineyardCode)) {
        return E_INVALID_VINEYARD_CODE;
    }
    
    pWinegrower = winegrowerList_containsVineyardplot(data.winegrowers, vineyardCode);
        
    if (pWinegrower == NULL) {
        return E_VINEYARD_NOT_FOUND;
    }
    
   // Search winegrower that contains the vineyard
   idx = vineyardplotData_find(pWinegrower->vineyardplots, vineyardCode);

    // Print data in the buffer
    sprintf(buffer, "%s;%s;%.2f", 
        pWinegrower->vineyardplots.elems[idx].code,
        pWinegrower->vineyardplots.elems[idx].doCode,
        pWinegrower->vineyardplots.elems[idx].weight
    );
    // Initialize the output structure
    csv_initEntry(entry);
    csv_parseEntry(entry, buffer, "VINEYARD_PLOT");
    
    return E_SUCCESS;
    
    
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED; 
}


// Get registered winegrower
tApiError api_getWinegrowers(tApiData data, tCSVData *winegrowers) {
    //////////////////////////////////
    // Ex PR1 3c
    /////////////////////////////////
    char buffer[2048];
    tWinegrowerNode *pNode = NULL;
    
    csv_init(winegrowers);
        
    pNode = data.winegrowers.first;
    while(pNode != NULL) {
        sprintf(buffer, "%s;%s;%02d/%02d/%04d", pNode->winegrower.id, pNode->winegrower.document,  
            pNode->winegrower.registrationDate.day, pNode->winegrower.registrationDate.month, pNode->winegrower.registrationDate.year);

        csv_addStrEntry(winegrowers, buffer, "WINEGROWER");
        pNode = pNode->next;
    }    
    
    return E_SUCCESS;
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Get registered vineyardplots
tApiError api_getVineyardplots(tApiData data, tCSVData *vineyards) {
    //////////////////////////////////
    // Ex PR1 3d
    /////////////////////////////////
    char buffer[2048];
    tWinegrowerNode *pNode = NULL;
    
    csv_init(vineyards);
        
    pNode = data.winegrowers.first;
    while(pNode != NULL) {
        
        
        if(pNode->winegrower.vineyardplots.elems != NULL) {
    
            for(int i=0; i < pNode->winegrower.vineyardplots.count; i++) {
                  // Print data in the buffer
                    sprintf(buffer, "%s;%s;%.2f", 
                        pNode->winegrower.vineyardplots.elems[i].code,
                        pNode->winegrower.vineyardplots.elems[i].doCode,
                        pNode->winegrower.vineyardplots.elems[i].weight
                    );
                

                csv_addStrEntry(vineyards, buffer, "VINEYARD_PLOT");
                
            }
        }    
        
                
        
        pNode = pNode->next;
    }    
    
    return E_SUCCESS;
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}


