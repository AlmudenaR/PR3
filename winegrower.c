#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "error.h"
#include <ctype.h>
#include <stdbool.h>
#include "winegrower.h"
#include "vineyardplot.h"
#include "grapevariety.h"

// Initialize the winegrowers data
tApiError winegrower_init(tWinegrower* winegrower,const char * id, const char * document, tDate registrationDate) {
    
     // Verify pre conditions
    assert(id != NULL);
    assert(document != NULL);
    
    // Allocate the memory for the string fields, using the length of the provided text plus 1 space
    //for the "end of string" char '\0'. To allocate memory we use the malloc command.
    winegrower->id = (char*)malloc((strlen(id) + 1) * sizeof(char));

    winegrower->document = (char*)malloc((strlen(document) + 1) * sizeof(char));

    // Check that memory has been allocated for all fields. Pointer must be different from NULL.
    if (winegrower->id == NULL || winegrower->document == NULL) {
        // Some of the fields have a NULL value, what means that we found some problem allocating the memory
        return E_MEMORY_ERROR;
    }

    // Once the memory is allocated, copy the data.
    
     // Set the data
    strcpy(winegrower->id, id);
    strcpy(winegrower->document, document);
    winegrower->registrationDate.day = registrationDate.day;
    winegrower->registrationDate.month = registrationDate.month;
    winegrower->registrationDate.year = registrationDate.year;
    
    vineyardplotData_init(&winegrower->vineyardplots);
 

    return E_SUCCESS;

}

// Copy the data of a tWinegrower from the source to destination
void winegrower_cpy(tWinegrower* destination, tWinegrower source){
     assert(destination != NULL);
    
    // Set the data
    winegrower_init(destination, source.id, source.document, source.registrationDate);
}


  // Release winegrower data
void winegrower_free(tWinegrower* winegrower) {
    assert(winegrower != NULL);
    
    // Release used memory
    if (winegrower->id != NULL) {
        free(winegrower->id);
        winegrower->id = NULL;
    }
    
    if (winegrower->document != NULL) {
        free(winegrower->document);
        winegrower->document = NULL;
    }
        
    
}


// Parse input from CSVEntry WINEGROWER  
void winegrower_parse(tWinegrower* wg, tVineyardplot* vineyardplot, tCSVEntry entry) {
    char stringDate[DATE_LENGTH + 1];    
    tDate date;
    char document[MAX_DOCUMENT_ID + 1];
    char id[WINEGROWERS_ID_LENGTH + 1];
    char vineyardCode[MAX_VINEYARD_CODE_LENGTH + 1];
    char doCode[DO_CODE_LENGTH + 1];
    float weight;
    tGrapeVariety grapeVariety;
    
    // Check input data
    assert(wg != NULL);
    assert(vineyardplot != NULL);
    assert(csv_numFields(entry) != NUM_FIELDS_WINEGROWER || csv_numFields(entry) != NUM_FIELDS_ONLY_WINEGROWER);

    // Get Winegrower data
    csv_getAsString(entry, 0, stringDate, DATE_LENGTH + 1);    
    
    csv_getAsString(entry, 1, document, MAX_DOCUMENT_ID + 1);        
    csv_getAsString(entry, 2, id, WINEGROWERS_ID_LENGTH + 1);        
    
    date_parse(&(date), stringDate);  
             
    winegrower_init(wg, id, document, date);
       

    if (csv_numFields(entry) == NUM_FIELDS_WINEGROWER) {
        // Get vineyardplot data
        csv_getAsString(entry, 3, vineyardCode, MAX_VINEYARD_CODE_LENGTH + 1);   
        csv_getAsString(entry, 4, doCode, DO_CODE_LENGTH + 1);    
        weight = csv_getAsReal(entry, 5);
        
        // PR3 EX 4c
        // Get the variety of the grape
        grapeVariety=csv_getAsInteger(entry,6);

        // Initialize the vineyardplot structure
        vineyardplot_init(vineyardplot, vineyardCode, doCode, weight, grapeVariety);
    } else {

        // Initialize empty vineyardplot structure
        vineyardplot_init(vineyardplot, "", "", 0, grapeVariety);
    }
}

// Initialize the vaccine's list
void winegrowerList_init(tWinegrowerList* list) {
    assert(list != NULL);
    
    list->first = NULL;
    list->count = 0;
}

// Find a Winegrower that contains a vineyardplot in the list of Winegrowers
tWinegrower* winegrowerList_containsVineyardplot(tWinegrowerList list, const char* code){
   tWinegrower* pWinegrower = NULL;
   tWinegrowerNode *pNode = NULL;
        
    // Point the first element
    pNode = list.first;
    
    while(pWinegrower == NULL && pNode != NULL) {
       
        if(pNode->winegrower.vineyardplots.elems != NULL) {
    
            for(int i=0; i < pNode->winegrower.vineyardplots.count; i++) {
                // Compare current with given code
                if(strcmp(pNode->winegrower.vineyardplots.elems[i].code, code) == 0) {
                    pWinegrower = &pNode->winegrower;
                }
            }
        }        
            
        pNode = pNode->next;
    }
   
    return pWinegrower;    
    
    
}

void winegrowerList_insert(tWinegrowerList* list, tWinegrower winegrower){
    
    tWinegrowerNode *pNode = NULL;
    tWinegrowerNode *pPrev = NULL;
    
    assert(list != NULL);
    
    // If the list is empty add the node as first position
    if (list->count == 0) {
        list->first = (tWinegrowerNode*) malloc(sizeof(tWinegrowerNode));
        list->first->next = NULL;
        winegrower_cpy(&(list->first->winegrower), winegrower);
    } else {    
        // Point the first element
        pNode = list->first;
        pPrev = pNode;
                
        // Advance in the list up to the insertion point or the end of the list
        while(pNode != NULL && strcmp(pNode->winegrower.id, winegrower.id) < 0) {            
            pPrev = pNode;
            pNode = pNode->next;
        }
                
        if (pNode == pPrev) {
            // Insert as first element
            list->first = (tWinegrowerNode*) malloc(sizeof(tWinegrowerNode));
            list->first->next = pNode;
            winegrower_cpy(&(list->first->winegrower), winegrower);            
        } else {
            // Insert after pPrev
            pPrev->next = (tWinegrowerNode*) malloc(sizeof(tWinegrowerNode));        
            winegrower_cpy(&(pPrev->next->winegrower), winegrower);
            pPrev->next->next = pNode;            
        }
    }
    list->count ++;
    
}

tWinegrowerList winegrowerList_orderByDateAndId(tWinegrowerList* list){
    // PR3 EX 2a
    
    // Sort a list of winegrowers by date and id
    // The input list is in the pointer to list
    // The input list is not modified
    // A copy of the list is sorted by registrationDate and id
    // The sorted list is returned by this method
    
    tWinegrowerList listSorted;
    winegrowerList_init(&listSorted);
    listSorted.count = -1;
    tWinegrowerNode *pActualAux = NULL;
    tWinegrowerNode *pNextAux = NULL;
    tWinegrower wgTemp;
    
    // Create a copy of data
    winegrowerList_cpy(&listSorted,*list);
    
    //Assign the first node of the copied data to the actual auxiliar node
    pActualAux = listSorted.first;
    
    //While the actual auxiliar node is not null
    while (pActualAux != NULL){
        //Assign the next node of the copied data from the actual auxiliar to the next auxiliar node
        pNextAux = pActualAux ->next;
        //While the next auxilair node is not null (is not the last one)
        while (pNextAux != NULL){
            //Compare the dates between the actual auxiliar node and the next auxiliar one in case they are different
            if (date_cmp(pActualAux->winegrower.registrationDate, pNextAux->winegrower.registrationDate)>0 ||
            //Or, if the dates are equal, compare the wg ids between the actual auxiliar node and the next auxiliar node
                (date_cmp(pActualAux->winegrower.registrationDate, pNextAux->winegrower.registrationDate)==0 &&
                strcmp(pActualAux->winegrower.id, pNextAux->winegrower.id)>0)) {
                //If one of the previous cases match, we assign to a temporal variable the winegrower from the actual auxiliar node
                wgTemp = pActualAux->winegrower;
                //Then assign the wg from the next auxiliar node the the wg from the actual auxiliar node (this has been assign to a temporal variable)
                pActualAux->winegrower = pNextAux->winegrower;
                //Assign to the temporal variable the next node since it'll be larger than the previous
                pNextAux->winegrower=wgTemp;
            }
            //Prepare the next auxiliar node to the next iteration
            pNextAux = pNextAux ->next;
        }
        //Prepare the actual auxiliar node to the next iteration
        pActualAux =pActualAux->next;
    }

    //Return the list sorted
    return listSorted;
}   
// Find a winegrower
tWinegrower* winegrowerList_find(tWinegrowerList list, const char* id)
{
    tWinegrowerNode *pNode = NULL;
    
    assert(id != NULL);
    
    pNode = list.first;
    
    while (pNode != NULL) {
        if (strcmp(pNode->winegrower.id, id) == 0) {
            return &(pNode->winegrower);
        }
        
        pNode = pNode->next;
    }
    
    return NULL;
}

// Find winegrowers that has a vineyard with a specific variety of grape
tWinegrowerList winegrowerList_findByGrapevariety(tWinegrowerList winegrowerList, tGrapeVariety grapeVariety) {
    // PR3 EX 2b
    // Input a list of winegrowers ordered by document id
    // Input a variety of grape
    // Output a new list of winegrowers orderd by document id that has a vineyardplot with the given variety of grape
    
    tWinegrowerList newList;
    tWinegrower* foundWinegrower;
    int i = 0;
    
    // Initialize to an empty list
    winegrowerList_init(&newList);
    
    //Assign to an auxiliar node the first node of the list from the input
    tWinegrowerNode* pNode = winegrowerList.first;
    
    //While the auxiliar node is not null (in the first iteration will be in case if the list is not empty)
    while (pNode != NULL ){
        //Iterate all the vineyardplots
        for (i = 0; i< pNode->winegrower.vineyardplots.count; i++){
            //For the actual position of the iteration, if we find a match in the grapevariety between the input list and the input grapevariety
            if (pNode->winegrower.vineyardplots.elems[i].grapeVariety== grapeVariety){
                //Find the pointer to the wg
                foundWinegrower = winegrowerList_find(newList, pNode->winegrower.id);
                //In case the pointer is not null because it exists or is not in the new list yet
                if (foundWinegrower == NULL) {
                    //Add the wg list of the grapevariety from the input
                    winegrowerList_insert(&newList, pNode->winegrower);
                }
            }
        }
        //Iterate to the next node
        pNode = pNode->next;
    }

    //Return the wg list of the grepevariety from the input
    return newList;
}

tWinegrowerList winegrowerList_findByWeighingYearAndGrapevariety(tWinegrowerList winegrowerList, int year, tGrapeVariety grapeVariety) {
    // PR3 EX 2c
    // Input a list of winegrowers ordered by document id
    // Input year of the weighing
    // Input a variety of grape
    // Output a new list of winegrowers orderd by document id that has a vineyardplot that had weighing on the given year and variety of grape
    tWinegrowerList newList;
    tWinegrower* foundWinegrower;
    tWeighingNode* weighNode;
    
    int i = 0;
    
    // Initialize to an empty list
    winegrowerList_init(&newList);
    
    //Assign to an auxiliar node the first node of the list from the input
    tWinegrowerNode* pNode = winegrowerList.first;

    //While the auxiliar node is not null (in the first iteration will be in case if the list is not empty)
    while (pNode != NULL ){
        //Iterate all the vineyardplots
        for (i = 0; i< pNode->winegrower.vineyardplots.count ; i++){
            //For the actual position of the iteration, if we find a match in the grapevariety between the input list and the input grapevariety
            if (pNode->winegrower.vineyardplots.elems[i].grapeVariety== grapeVariety){
                //Assign to an auxiliar weighing node the first node of weights
                weighNode = pNode->winegrower.vineyardplots.elems[i].weights.first;
                //While the auxiliar weighing node is not null (or in the first iteration has some weights registered)
                while (weighNode !=NULL ){
                    //Check if for the auxiliar weighing node the harvest year is equal to the input
                    if (weighNode->elem.harvestDay.year == year) {
                        //Find the pointer to the wg for the input
                        foundWinegrower = winegrowerList_find(newList, pNode->winegrower.id);
                        //In case the pointer is not null because it exists or is not in the new list yet
                        if (foundWinegrower == NULL) {
                            //Add the wg list of the grapevariety from the input
                            winegrowerList_insert(&newList, pNode->winegrower);
                        }
                    }
                     //Iterate to the next weighing node
                    weighNode = weighNode->next;
                }
            }
        }
        //Iterate to the next auxiliar list node
        pNode = pNode->next;
    }
    //Return the wg list of the grepevariety and year from the input
    return newList;
}
// Remove all elements
void winegrowerList_free(tWinegrowerList* list) {
    tWinegrowerNode *pNode = NULL;
    tWinegrowerNode *pAux = NULL;
    
    assert(list != NULL);
    
    pNode = list->first;
    while(pNode != NULL) {
        // Store the position of the current node
        pAux = pNode;
        
        vineyardplotData_free(&pNode->winegrower.vineyardplots);
        // Move to the next node in the list
        pNode = pNode->next;        
        // Remove previous node
        winegrower_free(&(pAux->winegrower));
        free(pAux);
    }
    
    // Initialize to an empty list
    winegrowerList_init(list);
}

// Get the number of winegrowers
int winegrowerList_len(tWinegrowerList list) {
    return list.count;
}

// Get the total number of vineyardplots
int winegrowerList_vineyardplots_total(tWinegrowerList list) {
    int count = 0;
    tWinegrowerNode *pNode = NULL;    
    
    pNode = list.first;
    while(pNode != NULL) {
        // Store the position of the current node
        count = count + pNode->winegrower.vineyardplots.count;
        // Move to the next node in the list
        pNode = pNode->next;            
    }
    
    return count;
}

// Get the number of vineyardplot registered on winegrower
int winegrowerVineyardplotCount(tWinegrower winegrower){
    //////////////////////////////////
    // Ex PR1 2d
    /////////////////////////////////
    return winegrower.vineyardplots.count;   
    /////////////////////////////////
    //return -1;
}



///AUXILIARY FUNCTIONS
// Copy the data of a tWinegrowerList from the source to destination
void winegrowerList_cpy(tWinegrowerList* destination, tWinegrowerList source){
    
    // Check input data
    assert(destination!=NULL);
    
    tWinegrowerNode* auxSource;
    tWinegrowerNode* auxDestination;
    tWinegrowerNode* lastNode;
    
    // Initialize to an empty list
    winegrowerList_init(destination);
    
    //Assign the first node from the source list to the auxSource
    auxSource = source.first;
    
    //While the aux node is null (or in the first iteration the struct is not empty)
    while (auxSource !=NULL){
        //Assign memory to the auxdestination node
        auxDestination = (tWinegrowerNode*) malloc (sizeof(tWinegrowerNode));
        //Initialize winegrower list
        winegrower_init(&auxDestination->winegrower, auxSource->winegrower.id, auxSource->winegrower.document, auxSource->winegrower.registrationDate );
        //Remove data of the auxdestination next node
        auxDestination->next=NULL;
        
        //If the first node of the destination list is empty (we didn't enter any data yet)
        if(destination->first ==NULL){
            //Allocate the content from the auxdestination node to the first element of the destination structure
            destination->first=auxDestination;
        }else{
            //In case there is already data in the destination structure the data of the next empty pointer will be added from the auxdestination one
            lastNode->next=auxDestination;
        }
        //We assign the auxdestination value to the lastnode (this will be the last one entered)
        lastNode = auxDestination;
        //Prepare de auxsource node for the next iteration
        auxSource = auxSource->next;
    }
    //Allocate the count of the destination list as the source's one
    destination->count=source.count;
}