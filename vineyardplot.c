#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "error.h"
#include <ctype.h>
#include <stdbool.h>
#include "vineyardplot.h"
    

// Initialize the vineyardplot  data
tApiError vineyardplot_init(tVineyardplot* vineyardplot, const char* vineyardCode, const char* doCode, float weight, tGrapeVariety grapeVariety) {
    assert(vineyardplot != NULL);
    assert(vineyardCode != NULL);
    assert(doCode != NULL);
    
    // Allocate memory and set the vineyard code
    vineyardplot->code = (char*) malloc(strlen(vineyardCode) + 1);
    assert(vineyardplot->code != NULL);
    strcpy(vineyardplot->code, vineyardCode);

    // Allocate memory and set the DO code
    vineyardplot->doCode = (char*) malloc(strlen(doCode) + 1);
    assert(vineyardplot->doCode != NULL);
    strcpy(vineyardplot->doCode, doCode);

    // Set the weight
    vineyardplot->weight = weight;
    
    // Initialize list of weighing
    weighingList_init(&(vineyardplot->weights));
	
	// PR3 EX 1b-1
    // Set the variey of the grape
    vineyardplot->grapeVariety = grapeVariety;
    
    return E_SUCCESS;
}

void vineyardplot_free(tVineyardplot* plot){
    assert(plot != NULL);
    
    if (plot->code != NULL) {
        free(plot->code);
        plot->code = NULL; 
    }

    if (plot->doCode != NULL) {
        free(plot->doCode);
        plot->doCode = NULL;
    }
    
    weighingList_free(&(plot->weights));
}

// Copy the data of a vineyardplot from the source to destination
void vineyardplot_cpy(tVineyardplot* destination, tVineyardplot source) {
    assert(destination != NULL);
    
    // Set the data
    // PR3 EX 1b-2
    // Initialize vineyardplotdata
    vineyardplot_init(destination, source.code, source.doCode, source.weight, source.grapeVariety);
}

// Initialize the vineyardplot lots data
void vineyardplotData_init(tVineyardplotData* data) {
    assert(data != NULL);
    
    // Set the initial number of elements to zero.
    data->count = 0;    
    data->elems = NULL;
}


// Add a new vineyardplot
void vineyardplotData_add(tVineyardplotData* data, tVineyardplot plot){
    int idx = -1;

    // Check input data (Pre-conditions)
    assert(data != NULL);    
    
    // Check if an entry with this data already exists
    idx = vineyardplotData_find(*data, plot.code);
    
    // If it does not exist, create a new entry
    if (idx < 0) {    
        if (data->elems == NULL) {
            data->elems = (tVineyardplot*) malloc(sizeof(tVineyardplot));
        } else {
            data->elems = (tVineyardplot*) realloc(data->elems, (data->count + 1) * sizeof(tVineyardplot));
        }
        assert(data->elems != NULL);        
        vineyardplot_cpy(&(data->elems[data->count]), plot);
        data->count ++;        
    }   
    
}

// Return the position of a vineyardplot with provided information. -1 if it does not exist
int vineyardplotData_find(tVineyardplotData data, const char* code) {
    int i;
    
    assert(code != NULL);
    
    for(i = 0; i < data.count; i++) {
        if(strcmp(data.elems[i].code, code) == 0) {
            return i;
        }
    }
    
    return -1;
}

// Remove all elements
void vineyardplotData_free(tVineyardplotData* data) {
    int i;
    
    if (data != NULL) {
        if(data->elems != NULL) {
    
        for(i=0; i < data->count; i++) {
            vineyardplot_free(&(data->elems[i]));
        }
        free(data->elems);
        vineyardplotData_init(data);    
        }
    }    
    
}

// Get the number of vineyardplot
int vineyardplotData_len(tVineyardplotData data) {
    return data.count;
}

// Parse input from CSVEntry VINEYARD_PLOT
void vineyardplot_parse(tVineyardplot* vineyardplot, tCSVEntry entry) {
    char vineyardCode[MAX_VINEYARD_CODE_LENGTH + 1];
    char doCode[DO_CODE_LENGTH + 1];
    float weight;
    tGrapeVariety grapeVariety;
    
    // Check input data
    assert(vineyardplot != NULL);
    assert(csv_numFields(entry) == NUM_FIELDS_VINEYARD_PLOT);         
 
                 
    // Get vineyardplot data    
    csv_getAsString(entry, 1, vineyardCode, MAX_VINEYARD_CODE_LENGTH + 1);   
    csv_getAsString(entry, 2, doCode, DO_CODE_LENGTH + 1);    
    weight = csv_getAsReal(entry, 3);
    
    // PR3 EX 4b
    // Get the variety of the grape
    grapeVariety=csv_getAsInteger(entry,4);
    
    // Initialize the vineyardplot structure
    vineyardplot_init(vineyardplot, vineyardCode, doCode, weight, grapeVariety);
}


bool check_vineyard_code(const char* code) {
    assert(code != NULL);
    
    // Check the total length of the code
    if (strlen(code) != MAX_VINEYARD_CODE_LENGTH) {
        return false;
    }
    
    // Check that the first two letters are uppercase
    if (!isupper(code[0]) || !isupper(code[1])) {
        return false;
    }
    
    // Check the dashes are in the correct positions
    if (code[2] != '-' || code[7] != '-') {
        return false;
    }
    
    // Check that characters 4 to 6 are digits (year)
    for (int i = 3; i <= 6; i++) {
        if (!isdigit(code[i])) {
            return false;
        }
    }

    // Check that the last five characters (9 to  12) are digits
    if (!isdigit(code[8]) || !isdigit(code[9]) | !isdigit(code[10]) | !isdigit(code[11]) | !isdigit(code[12])) {
        return false;
    }
    
    // If all the above conditions are met, the code is valid
    return true;
}

