/*--------------------------------------------------------------------*/
/* symtablelist.c                                                     */
/* Author: Christian Ronda                                            */
/*--------------------------------------------------------------------*/

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "symtable.h"

/*--------------------------------------------------------------------*/

/* Each item is stored in a Node. Nodes are linked to form a list */

struct Node{

   /* The key */
   const char *key;

   /* The value */
   void *value;

   /* The address of the next Node in the list */
   struct Node *next;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first Node */

struct SymTable{

   /* Address of the first Node */
   struct Node *first;

   /* Number of Nodes in the list */
   size_t len;
};

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if(oSymTable == NULL)
      return NULL;

   oSymTable->len = 0;
   oSymTable->first = NULL;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable){
   struct Node *psCurrentNode;
   struct Node *psNextNode;

   assert(oSymTable != NULL);

   psCurrentNode = oSymTable->first;
   while(psCurrentNode != NULL){
      psNextNode = psCurrentNode->next;
      free((void *)psCurrentNode->key);
      free(psCurrentNode);
      psCurrentNode = psNextNode;
   }

   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->len;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   int contains;
   struct Node *pcNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   contains = SymTable_contains(oSymTable, pcKey);

   if(contains == 1)
      return 0;

   pcNode = (struct Node*)malloc(sizeof(struct Node));

   if(pcNode == NULL)
      return 0;

   pcNode->key = (const char*)malloc(strlen(pcKey)+1);

   if(pcNode->key == NULL){
      free(pcNode);
      return 0;
   }

   strcpy((char *)pcNode->key,pcKey);
   pcNode->value = (void *)pvValue;
   pcNode->next = oSymTable->first;
   oSymTable->first = pcNode;

   oSymTable->len++;
   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   struct Node *pcNode;
   const void *pvOldValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for(pcNode = oSymTable->first; pcNode != NULL; pcNode = pcNode->next){
      if(strcmp(pcNode->key,pcKey)==0){
         pvOldValue = pcNode->value;
         pcNode->value = (void *)pvValue;
         return (void *)pvOldValue;
      }
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct Node *pcNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for(pcNode = oSymTable->first; pcNode != NULL; pcNode = pcNode->next){
      if(strcmp(pcNode->key, pcKey) == 0)
         return 1;
   }

   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Node *pcNode;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   for(pcNode = oSymTable->first; pcNode != NULL; pcNode = pcNode->next){
      if(strcmp(pcNode->key,pcKey) == 0)
         return (void *)pcNode->value;
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct Node *pcNode;
   struct Node *pcPrevNode;
   const void *pvValue;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   pcPrevNode = oSymTable->first;

   for(pcNode = oSymTable->first; pcNode != NULL; pcNode = pcNode->next){
      if(strcmp(pcNode->key,pcKey)==0){
         if(pcNode == oSymTable->first){
            pcPrevNode = pcNode->next;
            pvValue = pcNode->value;
            free((void *)pcNode->key);
            free(pcNode);
            oSymTable->first = pcPrevNode;
            oSymTable->len--;
            return (void *)pvValue;
         }
         pcPrevNode->next = pcNode->next;
         pvValue = pcNode->value;
         free((void *)pcNode->key);
         free(pcNode);
         oSymTable->len--;
         return (void *)pvValue;
      }
      pcPrevNode = pcNode;
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue,void *pvExtra),
                  const void *pvExtra){
   struct Node *pcNode;

   assert(oSymTable != NULL);
   assert((*pfApply) != NULL);

   for(pcNode = oSymTable->first; pcNode != NULL; pcNode = pcNode->next)
      (*pfApply)(pcNode->key, pcNode->value, (void *)pvExtra);
}
