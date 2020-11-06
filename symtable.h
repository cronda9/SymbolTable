/*--------------------------------------------------------------------*/
/* symtable.h                                                         */
/* Author: Christian Ronda                                            */
/*--------------------------------------------------------------------*/
#include <stddef.h>

#ifndef SYMTABLE_INCLUDED
#define SYMTABLE_INCLUDED

/**********************************************************************/
/* A SymTable_T object is a collection of key/value pairs */
typedef struct SymTable *SymTable_T;

/**********************************************************************/
/* Returns a new SymTable_T object, or NULL if insufficient memory is 
   available*/
SymTable_T SymTable_new(void);

/**********************************************************************/
/* Frees  all memory in oSymTable*/
void SymTable_free(SymTable_T oSymTable);

/**********************************************************************/
/* Returns the number of key/value pairs in oSymTable of return type
   size_t */
size_t SymTable_getLength(SymTable_T oSymTable);

/**********************************************************************/
/*Puts key-value pair of pcKey and pvValue into oSymTable at correct 
location. Return 1 (TRUE) if successful, or 0 (FALSE) if insuficient 
memory is avaliable, or pcKey is already in oSymTable*/
int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue);

/**********************************************************************/
/* If oSymTable contains pcKey, return replace the current value with
pvValue and return the replaced value.  Return NULL if pcKey is not in
oSymTable*/
void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue);
/**********************************************************************/
/* Returns 1 if oSymTable contains pcKey, and returns 0 if oSymTable
   does not contain pcKey*/
int SymTable_contains(SymTable_T oSymTable, const char *pcKey);

/**********************************************************************/
/* If oSymTable contains pcKey, returns the value associated with 
   pcKey. Returns NULL if oSymTable does not contain pcKey.*/
void *SymTable_get(SymTable_T oSymTable, const char *pcKey);

/**********************************************************************/
/* If oSymTable contains pcKey, removes the key/value pair associated 
with pcKey, frees the removed memory, decrements the length of 
oSymTable and returns the value of the removed value. If oSymTable
does not contain pcKey, return NULL*/
void *SymTable_remove(SymTable_T oSymTable, const char *pcKey);

/**********************************************************************/
/* Applies pfApply to every pcKey and associated pvValue in oSymTable,
   passing pvExtra as an extra argument*/
void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra);

#endif
