#ifndef __UOCVINEYARD_ERROR__H
#define __UOCVINEYARD_ERROR__H

// Define error codes
enum _tApiError
{
    E_SUCCESS = 0, // No error
    E_NOT_IMPLEMENTED = -1, // Called method is not implemented
    E_FILE_NOT_FOUND = -2, // File not found
    E_PERSON_NOT_FOUND = -3, // Person not found
    E_INVALID_ENTRY_TYPE = -4, // Invalid entry type
    E_INVALID_ENTRY_FORMAT = -5, // Invalid entry format
    E_DUPLICATED_PERSON = -6, // Duplicated person
    E_MEMORY_ERROR = -7, // Memory error
    E_INVALID_VINEYARD_CODE = -8, // vineyardplot code is not valid
    E_DUPLICATED_VINEYARD= -9, // Duplicated winegrower
    E_VINEYARD_NOT_FOUND = -10, // vineyard not found
    E_WINEGROWER_NOT_FOUND = -11, // winegrower not found
    E_DUPLICATED_DO = -12, // Duplicated DO
    E_DUPLICATED_WEIGHING = -13, // Duplicated Weighing
};

// Define an error type
typedef enum _tApiError tApiError;

#endif // __UOCVINEYARD_ERROR__H