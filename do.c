#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "do.h"

// Initialize to NULL all pointers of a DO
void do_initEmpty(tDO* DO)
{
    // Preconditions
    assert(DO != NULL);
    
    DO->code = NULL;
    DO->name = NULL;
    
    winegrowerList_init(&(DO->winegrowers));
    vineyardplotData_init(&(DO->vineyards));
}

// Initialize a DO
tApiError do_init(tDO* DO, const char* code, const char* name, double avgCropField)
{
    // Preconditions
    assert(DO != NULL);
    assert(code != NULL);
    assert(name != NULL);
    
    DO->code = (char*)malloc(sizeof(char) * (strlen(code) + 1));
    
    if (DO->code == NULL) {
        return E_MEMORY_ERROR;
    }
    
    DO->name = (char*)malloc(sizeof(char) * (strlen(name) + 1));
    
    if (DO->name == NULL) {
        return E_MEMORY_ERROR;
    }
    
    strcpy(DO->code, code);
    strcpy(DO->name, name);
    DO->avgCropField = avgCropField;
    
    winegrowerList_init(&(DO->winegrowers));
    vineyardplotData_init(&(DO->vineyards));
    
    return E_SUCCESS;
}

// Copy a DO
tApiError do_cpy(tDO* dst, tDO src)
{
    // Preconditions
    assert(dst != NULL);
    
    return do_init(dst, src.code, src.name, src.avgCropField);
}

// Release a DO
void do_free(tDO* DO)
{
    // Preconditions
    assert(DO != NULL);
    
    if (DO->code != NULL) {
        free(DO->code);
        DO->code = NULL;
    }
    
    if (DO->name != NULL) {
        free(DO->name);
        DO->name = NULL;
    }
    
    winegrowerList_free(&(DO->winegrowers));
    vineyardplotData_free(&(DO->vineyards));
}

// Initialize a DO data
void doData_init(tDOData* data)
{
    // Preconditions
    assert(data != NULL);
    
    data->elems = NULL;
    data->count = 0;
}

// Release a DO data
void doData_free(tDOData* data)
{
    int i;
    
    // Preconditions
    assert(data != NULL);
    
    if(data->elems != NULL) {
        for(i = 0; i < data->count; i++) {
            do_free(&(data->elems[i]));
        }
        
        free(data->elems);
        doData_init(data);
    }
}

// Return the number of DO in data
int doData_len(tDOData data)
{
    return data.count;
}

// Insert a DO to the DO data
tApiError doData_add(tDOData* data, tDO DO)
{
    // Preconditions
    assert(data != NULL);
    
    if (data->count == 0) {
        data->elems = (tDO*)malloc(sizeof(tDO));
    } else {
        data->elems = (tDO*)realloc(data->elems, sizeof(tDO) * (data->count + 1));
    }
    
    if (data->elems == NULL) {
        return E_MEMORY_ERROR;
    }
    
    do_cpy(&(data->elems[data->count]), DO);
    data->count++;
    
    return E_SUCCESS;
}

// Find a DO in DO data
tDO* doData_find(tDOData data, const char* code)
{
    int i;
    
    // Preconditions
    assert(code != NULL);
    
    for (i = 0; i < data.count; i++) {
        if (strcmp(data.elems[i].code, code) == 0) {
            return &(data.elems[i]);
        }
    }
    
    return NULL;
}

// Parse input from CSVEntry
void do_parse(tDO* data, tCSVEntry entry) {
    // Check input data
    assert(data != NULL);
    
    // Check entry fields
    assert(csv_numFields(entry) == NUM_FIELDS_DO);
    
    int pos = 0; // Allow to easy change position of the income data
    
    // Release the DO
    do_free(data);
    
    // Copy code data
    data->code = (char*) malloc((strlen(entry.fields[pos]) + 1) * sizeof(char));
    assert(data->code != NULL);
    memset(data->code, 0, (strlen(entry.fields[pos]) + 1) * sizeof(char));
    csv_getAsString(entry, pos, data->code, strlen(entry.fields[pos]) + 1);
    
    // Copy name data
    pos = 1;
    data->name = (char*) malloc((strlen(entry.fields[pos]) + 1) * sizeof(char));
    assert(data->name != NULL);
    memset(data->name, 0, (strlen(entry.fields[pos]) + 1) * sizeof(char));
    csv_getAsString(entry, pos, data->name, strlen(entry.fields[pos]) + 1);
    
    // Copy avgCropField data
    pos = 2;
    data->avgCropField = csv_getAsReal(entry, pos);
    
    winegrowerList_init(&(data->winegrowers));
    vineyardplotData_init(&(data->vineyards));
}

// Get the total weight from an specific winegrower
double doData_getTotalWeightByWinegrower(tDO DO, const char* winegrowerId)
{
    // PR2 EX 2a
    tWinegrower *winegrower;
    
    // Preconditions
    assert(winegrowerId != NULL);
    
    if ((winegrower = winegrowerList_find(DO.winegrowers, winegrowerId)) == NULL) {
        return 0.0;
    }
    
    return doData_getTotalWeightByWinegrower_recursive(winegrower->vineyardplots, winegrower->vineyardplots.count);
    /////////////////////////////////
    //return 0.0;
}

// Recursive version to get the total weight
double doData_getTotalWeightByWinegrower_recursive(tVineyardplotData vineyards, int index)
{
    double totalWeight;
    
    if (index == 0) {
        // Base case
        totalWeight = 0.0;
    } else {
        // Recursive case
        totalWeight = doData_getTotalWeightByWinegrower_recursive(vineyards, index-1) + vineyards.elems[index-1].weight;
    }
    
    return totalWeight;
}

// Get the total weighing for an specific winegrower, specific vineyard and specific year
double doData_getTotalWeighingByWineGrowerAndVineyardByYear(tDO DO, const char* winegrowerId, const char* vineyardplotCode, int year)
{
    // PR2 EX 2b
    tWinegrower *winegrower;
    tVineyardplot *vineyardplot;
    int idx;
    
    // Preconditions
    assert(winegrowerId != NULL);
    assert(vineyardplotCode != NULL);
    
    if ((winegrower = winegrowerList_find(DO.winegrowers, winegrowerId)) == NULL) {
        return 0.0;
    }
    
    if ((idx = vineyardplotData_find(winegrower->vineyardplots, vineyardplotCode)) == -1) {
        return 0.0;
    }
    
    vineyardplot = &(winegrower->vineyardplots.elems[idx]);
    
    return doData_getTotalWeighingByWineGrowerAndVineyardByYear_recursive(vineyardplot->weights.first, year);
    /////////////////////////////////
    //return 0.0;
}

// Recursive version to get the total weighing
double doData_getTotalWeighingByWineGrowerAndVineyardByYear_recursive(tWeighingNode *pNode, int year)
{
    double totalWeighing;
    
    if (pNode == NULL) {
        // Base case
        totalWeighing = 0.0;
    } else {
        // Recursive case
        totalWeighing = doData_getTotalWeighingByWineGrowerAndVineyardByYear_recursive(pNode->next, year);
        
        // Add the weighing if the year is the same
        if (pNode->elem.harvestDay.year == year) {
            totalWeighing += pNode->elem.weight;
        }
    }
    
    return totalWeighing;
}

// Get the total weighing for a specific DO on a specific year
double do_getTotalWeighing(tDO DO, int year) {
    // PR3 EX 3a
    // Each DO can have multiple vineyards and each vineyard can have multiple weighings for a given year
    // The sum of all the weighing for a given year is returned by this method
    double totalWeight =0.0;
    tWinegrower* winegrower;
    tVineyardplot* vineyardplot;
    tWeighingNode* weighingNode;
    
    //Assign the first node of the wg first node from the source list to a winegrower new Node
   tWinegrowerNode* winegrowerNode = DO.winegrowers.first;
    
    //While this winegrower new Node is null (or in the first iteration the struct is not empty)
    while (winegrowerNode != NULL) {
        
        //Assign to the wg pointer the value of the wg of the wg new node 
        winegrower = &(winegrowerNode->winegrower);
        //Iterate all the vineyardplots
        for (int i = 0; i < winegrower->vineyardplots.count; i++) {
            //Assign to the vineyardplot pointer the value of the vineyardplot [i] elem of the wg new node 
            vineyardplot = &(winegrower->vineyardplots.elems[i]);
            //Assign to the weighingNode pointer the value of first node of this weights list 
            weighingNode = vineyardplot->weights.first;
            //While this weighingNode pointer is not empty
            while (weighingNode != NULL) {
                //Assign to the year of the element of th weighingNode pointer the value of the input
                if (weighingNode->elem.harvestDay.year == year) {
                    //Add the weight value of this weighing node pointer element to the totalWeight variable
                    totalWeight += weighingNode->elem.weight;
                }
                //Iterate to the next weighing node
                weighingNode = weighingNode->next;
            }
        }
        //Iterate to the next wg node
        winegrowerNode = winegrowerNode->next;
    }
    //Return the total weighing for a specific DO on a specific year
    return totalWeight;
}

// Sort a DO Data by the weighing on a given year
tDOData doData_orderByWeighing(tDOData* DOData, int year) {
    // PR3 EX 3b
    // The input DO Data is in the pointer to tDOData
    // A copy of the DO Data is sorted by weighing on a given year
    // The sorted DO Data is returned by this method
    
    tDOData newDOData;
    tApiError error;
    
    // Initialize a newDOData structure
    doData_init(&newDOData);
    
    //If the input data is not empty
    if (DOData->count > 0) {
        //Allocate memory to the newDODAta as the input DODAta
        newDOData.elems = (tDO*)malloc(DOData->count * sizeof(tDO));
        
        //Assign to the new DOData the same count as input
        newDOData.count = DOData->count;
        
        //Iterate all the DOData elems
        for (int i = 0; i < DOData->count; i++) {
            //Copy the DOData of the i element to the newDOData i position
            error = do_cpy(&newDOData.elems[i], DOData->elems[i]);
            //If the do_cpy function returns a value different from SUCCESS
            if (error != E_SUCCESS) {
                // Free the allocated memory assigned to the newDODAta structure
                doData_free(&newDOData);
                //And return the newDOData values. In this case it hasn't copied any data
                return newDOData;
            }
        }
        // When the DOData has been copied to the newDODAta, we sort this new structure with quickSort method
        quickSort(newDOData.elems, 0, newDOData.count - 1, year, DOData);
    }
    //Return the new structure
    return newDOData;
}


//AUXILIAR FUNCTIONS
void swap(tDO* a, tDO* b) {
    tDO aux = *a;
    memcpy(&aux, a, sizeof(tDO));
    memcpy(a,b, sizeof(tDO));
    memcpy(b, &aux, sizeof(tDO));
}

// QuickSort implementation
void quickSort(tDO* v, int begin, int end, int year, tDOData* DOData) {
    int i, j;
    tDO pivot;
    
    //If the begin from the input is smaller tha the end (we are goint to call this function recursively)
    if (begin < end) {
        i = begin + 1;
        j = end;
        pivot = v[begin];

        //While data in i position is smaller or iqual than the j one
        while (i <= j) {
            //Assign to an iAux, jAux and pivotAux pointer de DODAta searched from the input
            tDO* iAux = doData_find(*DOData, v[i].code);
            tDO* jAux = doData_find(*DOData, v[j].code);
            tDO* pivotAux = doData_find(*DOData, pivot.code);
            //While the i value is smaller or equal to the end input value and the aux pointers are not null AND 
            //(the weihing of the iAux pointer and input year is greater than the pivot one )OR
            //(the weihing of the iAux pointer and input year is equal to the pivot one and the code of i pointer is smaller than the pivot one)
            while (i <= end && iAux != NULL && pivotAux != NULL && 
                    (do_getTotalWeighing(*iAux, year) > do_getTotalWeighing(*pivotAux, year) || 
                    (do_getTotalWeighing(*iAux, year) == do_getTotalWeighing(*pivotAux, year) && strcmp(v[i].code, pivot.code) <= 0))) {
                //Iterate to the next position of i since the weight is greater than the one in the pivot position so we don't need to exchange
                i++;
                if (i <= end) {
                    //ASsign to iAux pointer the value of the new i position after the iteration
                    iAux = doData_find(*DOData, v[i].code);
                }
            }
            //While jAux pointer is not null nor the pivotAux one AND
            //(the year of the jAux pointer is lower than the pivot one) OR
            //(the year of the jAux pointer is equal to the pivot one and the j code is greater than the pivot one)
            while (jAux != NULL && pivotAux != NULL &&
                    (do_getTotalWeighing(*jAux, year) < do_getTotalWeighing(*pivotAux, year) || 
                    (do_getTotalWeighing(*jAux, year) == do_getTotalWeighing(*pivotAux, year) && strcmp(v[j].code, pivot.code) > 0))) {
                //DEcrease j in one position
                j--;
                //If j still greater or equal than the begin position
                if (j >= begin) {
                    //Assign to jAux pointer the value of the new j position after decreasing the j value
                    jAux = doData_find(*DOData, v[j].code); 
                }
            }
            //If i value is smaller than j value
            if (i < j) {
                //Call to the swap function wto exchange i and j tuples
                swap(&v[i], &v[j]);
            }
        }
        //Exchagne the initial position array to the j one
        swap(&v[begin], &v[j]);
        //THen sort with the parts left after the firts sort calling recursively to the parts left from the sort (leaving the right values from the pivot smaller than it and the left ones greater than it until the complete array is sorted.
        quickSort(v, begin, j - 1, year, DOData);
        quickSort(v, j + 1, end, year, DOData);
    }
}