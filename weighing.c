#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "error.h"
#include "weighing.h"
#include "vineyardplot.h"
#include "grapevariety.h"

// Initialize a weighing
tApiError weighing_init(tWeighing* weighing, const char* code, float weight, tDate harvestDay, tGrapeVariety grapeVariety) {
    
    // Preconditions
    assert(weighing != NULL);
    assert(code != NULL);
    
    // Allocate the memory for the code
    weighing->code = (char*)malloc(sizeof(char) * (strlen(code) + 1));
    
    if (weighing->code == NULL)
    {
        return E_MEMORY_ERROR;
    }
    
    // Assign all weighing properties
    strcpy(weighing->code, code);
    weighing->weight = weight;
    date_cpy(&(weighing->harvestDay), harvestDay);
    
    // PR3 EX 1d-1
    // Set the variey of the grape
    weighing->grapeVariety = grapeVariety;


    return E_SUCCESS;
}

// Copy a weighing from src to dst
tApiError weighing_cpy(tWeighing* dst, tWeighing src)
{
    // Preconditions
    assert(dst != NULL);
    
    // Initialize all values with init function
    // PR3 EX 1d-2
    return weighing_init(dst, src.code, src.weight, src.harvestDay, src.grapeVariety);
}

// Release a weighing
void weighing_free(tWeighing* weighing)
{
    // Preconditions
    assert(weighing != NULL);
    
    // Release the memory allocated in init function
    if (weighing->code != NULL) {
        free(weighing->code);
        weighing->code = NULL;
    }
}

// Initialize a weighing list
tApiError weighingList_init(tWeighingList* list)
{
    // PR2 EX 1a
    // Preconditions
    assert(list != NULL);
    
    // Initialize the first and last node to NULL
    list->first = NULL;
    list->last = NULL;
    
    return E_SUCCESS;
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Check if a weighing list is empty
bool weighingList_empty(tWeighingList list)
{
    // PR2 EX 1b
    return list.first == NULL;
    /////////////////////////////////
    //return false;
}

// Insert new weighing data
tApiError weighingList_add(tWeighingList* list, tWeighing weighing)
{
    // PR2 EX 1c
    tWeighingNode *pNode = NULL, *pPrev = NULL;
    
    // Preconditions
    assert(list != NULL);
    
    if (weighingList_empty(*list))
    {
        // Create the new node
        pNode = weighingList_createNode(weighing);
        
        if (pNode == NULL)
        {
            return E_MEMORY_ERROR;
        }
        
        // Point the created node to the first and last node at the same time
        list->first = pNode;
        list->last = pNode;
    }
    else
    {
        // Check if there is a node for that day and code
        pNode = weighingList_findNode(*list, weighing.code, weighing.harvestDay);
        
        // If the node already exists, update its weight
        if (pNode != NULL)
        {
            pNode->elem.weight += weighing.weight;
        }
        else
        {
            // Create a new node for this day and code
            pNode = weighingList_createNode(weighing);
            
            if (pNode == NULL)
            {
                return E_MEMORY_ERROR;
            }
            
            // Find the previous node to insert the new node
            pPrev = weighingList_findPrev(*list, weighing.code, weighing.harvestDay);
            
            // If pPrev is NULL, the new node has to be pointer at the beginning
            if (pPrev == NULL)
            {
                // No condition needed to check if list->first is NULL
                // This scope assume that the list is not empty
                list->first->prev = pNode;
                pNode->next = list->first;
                pNode->prev = NULL;
                list->first = pNode;
            }
            else
            {
                // Link all pointers of the current, previous and next node
                if (pPrev->next != NULL)
                {
                    pPrev->next->prev = pNode;
                }
                
                pNode->next = pPrev->next;
                pNode->prev = pPrev;
                pPrev->next = pNode;
                
                // Check if the previous node is the current last node and update it
                if (list->last == pPrev)
                {
                    list->last = pNode;
                }
            }
        }
    }
    
    return E_SUCCESS;
    /////////////////////////////////
    //return E_NOT_IMPLEMENTED;
}

// Return the weight until the day received as a parameter according to a code
double weighingList_getWeight(tWeighingList list, const char* code, tDate day)
{
    // PR2 EX 1d
    double totalWeight;
    tWeighingNode *pNode = NULL;
    
    // Preconditions
    assert(code != NULL);
    
    // Initialize the summatory
    totalWeight = 0.0;
    
    // Get the first node to start
    pNode = list.first;
    
    // Iterate until the day received or the end of the list
    while (pNode != NULL && date_cmp(pNode->elem.harvestDay, day) <= 0) {
        if (strcmp(pNode->elem.code, code) == 0) {
            totalWeight += pNode->elem.weight;
        }
        
        pNode = pNode->next;
    }
    
    return totalWeight;
    /////////////////////////////////
    //return 0.0;
}

// Release the list
void weighingList_free(tWeighingList* list)
{
    // PR2 EX 1e
    tWeighingNode *pNode = NULL;
    
    // Preconditions
    assert(list != NULL);
    
    // Get the first node to release
    pNode = list->first;
    
    // Iterate the list until empty
    while (pNode != NULL) {
        // Remove the first node
        list->first = pNode->next;
        
        // Release the elem
        weighing_free(&(pNode->elem));
        
        // Release the allocated memory for the node
        free(pNode);
        
        // Prepare pNode for the next iteration with the current first node
        pNode = list->first;
    }
    
    // Set all the pointers to NULL
    weighingList_init(list);
    ////////////////
}

// AUX

// Create a new node for weighing list
tWeighingNode* weighingList_createNode(tWeighing weighing)
{
    tWeighingNode* pNode = NULL;
    
    // Allocate memory for the node
    pNode = (tWeighingNode*)malloc(sizeof(tWeighingNode));
    
    if (pNode == NULL)
    {
        return NULL;
    }
    
    // Assign all its properties
    pNode->next = NULL;
    pNode->prev = NULL;
    weighing_cpy(&(pNode->elem), weighing);
    
    return pNode;
}

// Find a node by harvest date and code
tWeighingNode* weighingList_findNode(tWeighingList list, const char* code, tDate harvestDay)
{
    tWeighingNode* pNode = NULL;
    
    // Preconditions
    assert(code != NULL);
    
    // Get the first node to start to find
    pNode = list.first;
    
    // Iterate through the doubly linked list until the node is found or passed
    // (according to its harving day and code)
    while (pNode != NULL && date_cmp(pNode->elem.harvestDay, harvestDay) <= 0) {
        if (date_cmp(pNode->elem.harvestDay, harvestDay) == 0 && strcmp(pNode->elem.code, code) == 0) {
            return pNode;
        }
        
        // Iterate the doubly linked list
        pNode = pNode->next;
    }
    
    return NULL;
}

// Find the previous node to insert a new node by its harvest day and its code
tWeighingNode* weighingList_findPrev(tWeighingList list, const char* code, tDate harvestDay)
{
    tWeighingNode *pNode = NULL;
    
    // Preconditions
    assert(code != NULL);
    
    // Iterate the list to find the previous node to insert the new one
    pNode = list.first;
    
    // Find the node and return the previous one
    while (pNode != NULL)
    {
        if (date_cmp(pNode->elem.harvestDay, harvestDay) > 0 
            || (date_cmp(pNode->elem.harvestDay, harvestDay) == 0 && strcmp(pNode->elem.code, code) > 0))
        {
            return pNode->prev;
        }
        
        pNode = pNode->next;
    }
    
    // If the list is empty, return NULL, otherwise, return the last node
    // because the new node has to be linked at the end of the list
    return list.last;
}


// Parse input from CSVEntry WEIGHING  
void weighing_parse(tWeighing* weighing, tCSVEntry entry) {
    // PR3 EX 4a
    // CSV format example "WEIGHING;30/12/2023;ABCD;20.00;1;PE-2024-00001"
    // WEIGHING = It indicates that represents a weighing
    // 30/12/2023 = Harvest date
    // ABCD = code
    // 1 = Variety of grape (TEMPRANILLO)
    // PE-2024-00001 = Vineyard code
    char code[WEIGHING_CODE_LENGTH + 1];
    float weight;
    char stringDate[DATE_LENGTH + 1];    
    tDate date;
    char vineyardCode[MAX_VINEYARD_CODE_LENGTH + 1];
    tGrapeVariety grapeVariety;
    
    // Check input data
    assert (weighing!=NULL);
    assert(csv_numFields(entry) == NUM_FIELDS_WEIGHING);
    
    
    // Get Winegrower data
    csv_getAsString(entry, 0, stringDate, DATE_LENGTH + 1);    
    
    csv_getAsString(entry, 1, code, WEIGHING_CODE_LENGTH + 1);        
    weight=csv_getAsReal(entry, 2);
    grapeVariety=csv_getAsInteger(entry,3);
    csv_getAsString(entry, 4, vineyardCode, MAX_VINEYARD_CODE_LENGTH + 1);   


    date_parse(&(date), stringDate);
    
    weighing_init(weighing, code,weight,date,grapeVariety);

}

