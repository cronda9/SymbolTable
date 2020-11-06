/*--------------------------------------------------------------------*/
/* symtablehash.c                                                     */
/* Author: Christian Ronda                                            */
/*--------------------------------------------------------------------*/
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "symtable.h"
/*--------------------------------------------------------------------*/

/* Const size_t array of sequence of number buckets allowed*/

static const size_t buckets[] = {509, 1021, 2039, 4093, 8191,
                                 16381, 32749, 65521};

/*--------------------------------------------------------------------*/
/* Returns hash index corresponding to which bucket pcKey gets placed
   depending on uBucketCount*/

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount);

/*--------------------------------------------------------------------*/

/* Expands the memory of oSymTable to the next sequence of buckets. 
   Returns 1 (TRUE) if successful, returns 0 (FALSE) if insufficient
   memory availble*/

static int SymTable_expand(SymTable_T oSymTable);

/*--------------------------------------------------------------------*/

/* Each key-value pair is stored in a Binding structure. Bindings 
   are linked to form a list*/

struct Binding{

   /* The key */
   const char *key;

   /* The value */
   void *value;

   /* The address of the next Binding */
   struct Binding *next;
};

/*--------------------------------------------------------------------*/

/* A SymTable is a structure that points to the first bucket in the 
   list of bindings, keepss the length of the SymTable, and the index 
   of the sequence of buckets*/

struct SymTable{

   /* Address of the bucket in the list of bindings */
   struct Binding **bindings;

   /* Number of bindings */
   size_t len;

   /* Index of sequence of buckets array */
   int buckets_i;
};

/*--------------------------------------------------------------------*/

/* Return a hash code for pcKey that is between 0 and uBucketCount-1,
   inclusive. */

static size_t SymTable_hash(const char *pcKey, size_t uBucketCount)
{
   const size_t HASH_MULTIPLIER = 65599;
   size_t u;
   size_t uHash = 0;

   assert(pcKey != NULL);

   for (u = 0; pcKey[u] != '\0'; u++)
      uHash = uHash * HASH_MULTIPLIER + (size_t)pcKey[u];

   return uHash % uBucketCount;
}

/*--------------------------------------------------------------------*/

SymTable_T SymTable_new(void){
   SymTable_T oSymTable;
   oSymTable = (SymTable_T)malloc(sizeof(struct SymTable));
   if (oSymTable == NULL)
      return NULL;

   oSymTable->buckets_i = 0;

   oSymTable->bindings = (struct Binding**)calloc(buckets[oSymTable->buckets_i],
                                                  sizeof(struct Binding*));

   if(oSymTable->bindings == NULL){
      free(oSymTable);
      return NULL;
   }

   oSymTable->len = 0;
   return oSymTable;
}

/*--------------------------------------------------------------------*/

void SymTable_free(SymTable_T oSymTable){
   struct Binding *pcBinding;
   struct Binding *pcNextBinding;
   size_t i;

   assert(oSymTable != NULL);

   for(i = 0; i < buckets[oSymTable->buckets_i]; i++){
      pcBinding = oSymTable->bindings[i];
      while(pcBinding != NULL){
         pcNextBinding = pcBinding->next;
         free((void *)pcBinding->key);
         free(pcBinding);
         pcBinding = pcNextBinding;
      }
   }
   free(oSymTable->bindings);
   free(oSymTable);
}

/*--------------------------------------------------------------------*/

size_t SymTable_getLength(SymTable_T oSymTable){
   return oSymTable->len;
}

/*--------------------------------------------------------------------*/

int SymTable_put(SymTable_T oSymTable, const char *pcKey,
                 const void *pvValue){
   size_t hash_index;
   int contains;
   struct Binding *pcBinding;
   int expansion;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   contains = SymTable_contains(oSymTable, pcKey);

   if(contains == 1)
      return 0;

   /*this block deals with the expansion of the symtable */
   if(oSymTable->len != buckets[7]){ /* checks if oSymTable is 
                                        at max length*/
      if(oSymTable->len == buckets[oSymTable->buckets_i]){
         oSymTable->buckets_i++; /* increase index of the buckets */
         expansion = SymTable_expand(oSymTable);
         if(expansion == 0){ /* if the expansion failed, 
                                keep the same amount of buckets */
            oSymTable->buckets_i--;
         }
      }
   }

   hash_index = SymTable_hash(pcKey, buckets[oSymTable->buckets_i]);

   pcBinding = (struct Binding*)malloc(sizeof(struct Binding));
   if(pcBinding == NULL)
      return 0;

   pcBinding->key = (const char*)malloc(strlen(pcKey)+1);
   if(pcBinding->key == NULL){
      free(pcBinding);
      return 0;
   }

   strcpy((char *)pcBinding->key, pcKey);
   pcBinding->value = (void *)pvValue;
   pcBinding->next = oSymTable->bindings[hash_index];
   oSymTable->bindings[hash_index] = pcBinding;
   oSymTable->len++;

   return 1;
}

/*--------------------------------------------------------------------*/

/* Expands the memory in oSymTable to the next sequence of buckets. 
   Returns 1 (TRUE) if successful, returns 0 (FALSE) if insufficient 
   memory is available */

static int SymTable_expand(SymTable_T oSymTable){
   struct Binding **bindings;
   struct Binding *pcBinding;
   struct Binding *add;
   struct Binding *temp;
   size_t hash_index;
   size_t bucket;

   bindings = (struct Binding**)calloc(buckets[oSymTable->buckets_i],
                                       sizeof(struct Binding*));

   if(bindings == NULL)
      return 0;

   for(bucket = 0; bucket < buckets[oSymTable->buckets_i-1]; bucket++){
      pcBinding = oSymTable->bindings[bucket];
      while(pcBinding != NULL){
         hash_index = SymTable_hash(pcBinding->key,
                                    buckets[oSymTable->buckets_i]);
         temp = pcBinding->next;

         add = (struct Binding*)malloc(sizeof(struct Binding));
         if(add == NULL){
            free(bindings);
            return 0;
         }

         add->key = (const char*)malloc(strlen(pcBinding->key)+1);
         if(add->key == NULL){
            free(bindings);
            free(add);
            return 0;
         }

         strcpy((char *)add->key, pcBinding->key);
         add->value = pcBinding->value;
         add->next = bindings[hash_index];
         bindings[hash_index] = add;
         free((void *)pcBinding->key);
         free(pcBinding);
         pcBinding = temp;

      }
   }
   free(oSymTable->bindings);
   oSymTable->bindings = bindings;

   return 1;
}

/*--------------------------------------------------------------------*/

void *SymTable_replace(SymTable_T oSymTable, const char *pcKey,
                       const void *pvValue){
   size_t hash_index;
   struct Binding *pcBinding;
   void *value;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash_index = SymTable_hash(pcKey, buckets[oSymTable->buckets_i]);

   for(pcBinding = oSymTable->bindings[hash_index];
       pcBinding != NULL; pcBinding = pcBinding->next){
      if(strcmp(pcBinding->key, pcKey) == 0){
         value = pcBinding->value;
         pcBinding->value = (void *)pvValue;
         return value;
      }
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

int SymTable_contains(SymTable_T oSymTable, const char *pcKey){
   struct Binding *pcBinding;
   size_t hash_index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash_index = SymTable_hash(pcKey, buckets[oSymTable->buckets_i]);

   for(pcBinding = oSymTable->bindings[hash_index];
       pcBinding != NULL; pcBinding = pcBinding->next) {
      if (strcmp(pcBinding->key, pcKey) == 0)
         return 1;
   }

   return 0;
}

/*--------------------------------------------------------------------*/

void *SymTable_get(SymTable_T oSymTable, const char *pcKey){
   struct Binding *pcBinding;
   size_t hash_index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash_index = SymTable_hash(pcKey, buckets[oSymTable->buckets_i]);

   for(pcBinding = oSymTable->bindings[hash_index];
       pcBinding != NULL; pcBinding = pcBinding->next) {
      if (strcmp(pcBinding->key, pcKey) == 0)
         return pcBinding->value;
   }

   return NULL;
}

/*--------------------------------------------------------------------*/

void *SymTable_remove(SymTable_T oSymTable, const char *pcKey){
   struct Binding *pcBinding;
   struct Binding *pcPrevBinding;
   void *value;
   size_t hash_index;

   assert(oSymTable != NULL);
   assert(pcKey != NULL);

   hash_index = SymTable_hash(pcKey, buckets[oSymTable->buckets_i]);

   pcPrevBinding = oSymTable->bindings[hash_index];

   for(pcBinding = oSymTable->bindings[hash_index]; pcBinding != NULL;
       pcBinding = pcBinding->next) {
      if(strcmp(pcBinding->key, pcKey) == 0){
         if(pcBinding == oSymTable->bindings[hash_index]){
            value = pcBinding->value;
            oSymTable->bindings[hash_index] = pcBinding->next;

            free((void *)pcBinding->key);
            free(pcBinding);

            oSymTable->len--;
            return value;
         }
         value = pcBinding->value;
         pcPrevBinding->next = pcBinding->next;

         free((void *)pcBinding->key);
         free(pcBinding);

         oSymTable->len--;
         return value;
      }
      pcPrevBinding = pcBinding;
   }
   return NULL;
}

/*--------------------------------------------------------------------*/

void SymTable_map(SymTable_T oSymTable,
                  void (*pfApply)(const char *pcKey, void *pvValue, void *pvExtra),
                  const void *pvExtra){
   struct Binding *pcBinding;
   size_t index;

   assert(oSymTable != NULL);
   assert((*pfApply) != NULL);

   for(index = 0; index<buckets[oSymTable->buckets_i]; index++){
      pcBinding = oSymTable->bindings[index];
      while(pcBinding != NULL){
         (*pfApply)(pcBinding->key, pcBinding->value, (void *)pvExtra);
         pcBinding = pcBinding->next;
      }
   }
}
