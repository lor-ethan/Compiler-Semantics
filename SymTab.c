/* Author:      Ethan Lor
   Created:     09/24.18
   Resources:   https://www.tutorialspoint.com/data_structures_algorithms/hash_table_program_in_c.htm
                https://research.cs.vt.edu/AVresearch/hashing/strings.php
                https://www.geeksforgeeks.org/few-bytes-on-null-pointer-in-c/
                https://www.geeksforgeeks.org/double-pointer-pointer-pointer-c/
                https://www.mkssoftware.com/docs/man3/calloc.3.asp
                https://www.geeksforgeeks.org/g-fact-66/
                Kala Arentz
                Benjamin Polzin
                Adam Yates
                Prof. Steven Senger
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "SymTab.h"

/* The symbol table entry structure proper.
*/
// struct SymEntry {
//   struct SymTab * table;
//   char *name;
//   int attrKind;
//   void *attributes;
//   struct SymEntry *next;
// };

/* The symbol table structure proper. The hash table array Contents
   is dynamically allocated according to size
*/
// struct SymTab {
//   struct SymTab * parent;
//   char *scopeName;
//   int size;
//   struct SymEntry **contents;
// };

struct SymTab *
CreateSymTab(int size, char * scopeName, struct SymTab * parentTable) {
  struct SymTab * table = malloc(sizeof(struct SymTab));

  if(parentTable != NULL) {
    table->parent = parentTable;
  } else {
    table->parent = NULL;
  }

  if(scopeName != NULL) {
    table->scopeName = strdup(scopeName);
  } else {
    table->scopeName = NULL;
  }

  if(size <= 3) {
    table->size = 3;
  } else {
    table->size = size;
  }

  table->contents = malloc(size * sizeof(struct SymEntry *));
  for(int i = 0; i < table->size; i++) {
    table->contents[i] = NULL;
  }

  return table;
}

struct SymTab *
DestroySymTab(struct SymTab *aTable) {
  struct SymTab * parent = aTable->parent;

  for(int i = 0; i < aTable->size; i++) {
    struct SymEntry * entry = aTable->contents[i];
    while (entry != NULL) {
      struct SymEntry * temp = entry->next;
      free(entry->name);
      free(entry);
      entry = temp;
    }
  }

  free(aTable->scopeName);
  free(aTable->contents);
  free(aTable);
  return parent;
}

int
HashName(int size, const char *name) {
  int i, sum;

  sum = 0;
  for(i = 0; i < strlen(name); i++) {
    sum += (int)name[i];
  }

  return sum % size;
}

struct SymEntry *
LookupName(struct SymTab *aTable, const char * name) {
  if(aTable == NULL || name == NULL) {
    return NULL;
  }

  struct SymTab * parent = aTable->parent;
  struct SymEntry * current = aTable->contents[HashName(aTable->size, name)];

  while(current != NULL) {
    if(strcmp(current->name,name) == 0) {
      return current;
    }

    current = current->next;
  }

  if(parent != NULL) {
    return LookupName(parent, name);
  } else {
    return NULL;
  }
}

struct SymEntry *
EnterName(struct SymTab *aTable, const char *name) {
  if(aTable == NULL || name == NULL) {
    return NULL;
  }

  int index = HashName(aTable->size, name);
  struct SymEntry * entry;

  struct SymEntry * current = aTable->contents[index];

  if(current == NULL) {
    entry = malloc(sizeof(struct SymEntry));
    entry->table = aTable;
    entry->name = strdup(name);
    entry->attributes = NULL;
    aTable->contents[index] = entry;
    entry->next = NULL;
    return entry;
  }

  while(current != NULL) {
    if(strcmp(current->name,name) == 0) {
      return current;
    }
    current = current->next;
  }

  entry = malloc(sizeof(struct SymEntry));
  entry->table = aTable;
  entry->name = strdup(name);
  entry->attributes = NULL;
  entry->next = aTable->contents[index];
  aTable->contents[index] = entry;
  return entry;
}

void
SetAttr(struct SymEntry *anEntry, int kind, void *attributes) {
  if(anEntry == NULL) {
    return;
  }

  anEntry->attrKind = kind;
  anEntry->attributes = attributes;
}

int
GetAttrKind(struct SymEntry *anEntry) {
  if(anEntry == NULL) {
    return 0;
  }

  return anEntry->attrKind;
}

void *
GetAttr(struct SymEntry *anEntry) {
  if(anEntry == NULL) {
    return NULL;
  }

  return anEntry->attributes;
}

const char *
GetName(struct SymEntry *anEntry) {
  if(anEntry == NULL) {
    return NULL;
  }

  return anEntry->name;
}

struct SymTab *
GetTable(struct SymEntry *anEntry) {
  if(anEntry == NULL) {
    return NULL;
  }

  return anEntry->table;
}

const char *
GetScopeName(struct SymTab *aTable) {
  if(aTable == NULL) {
    return NULL;
  }

  return aTable->scopeName;
}

char *
GetScopePath(struct SymTab *aTable) {
  if(aTable == NULL) {
    return NULL;
  }

  char *name;
  char *path;

  if(aTable->scopeName == NULL) {
    path = strdup("");
  } else {
    path = strdup(aTable->scopeName);
  }

  struct SymTab *parent = aTable->parent;
  while(parent != NULL) {
    if(parent->scopeName == NULL) {
      name = strdup("");
    } else {
      name = strdup(parent->scopeName);
    }

    char *temp = strdup(path);
    path = (char *)realloc(path, strlen(path) + strlen(name) + 2);
    strcpy(path, name); 
    strcat(path, ">");
    strcat(path, temp);
    parent = parent->parent;
  }

  return path;
}

struct SymTab *
GetParentTable(struct SymTab *aTable) {
  if(aTable == NULL) {
    return NULL;
  }

  return aTable->parent;
}

void
InvokeOnEntries(struct SymTab *aTable, bool includeParentTable,
             entryWorkFunc workFunc, int startCnt, void * withArgs) {
  struct SymTab *parent;

  if(aTable == NULL) {
    return;
  } else {
    parent = aTable->parent;
  }

  for(int i = 0; i < aTable->size; i++) {
    struct SymEntry * content = aTable->contents[i];
    if(content != NULL) {
      struct SymEntry * entry = content;
      while (entry != NULL) {
        struct SymEntry * next = entry->next; 
        workFunc(entry, startCnt++, withArgs);
        entry = next;
      }
    }
  }

  if(includeParentTable == true) {
    InvokeOnEntries(parent, true, workFunc, startCnt, withArgs);
  }
}

struct Stats *
Statistics(struct SymTab *aTable) {
  if(aTable == NULL) {
    return NULL;
  }

  int min;
  int max;
  int totalLen = 0;
  int avgLen = 0;
  int totalCount = 0;

  struct SymEntry * entry = aTable->contents[0];
  int count = 0;
  while(entry != NULL) {
    count++;
    entry = entry->next;
  }

  min = count;
  max = count;
  totalLen = count;

  for(int i = 1; i < aTable->size; i++) {
    entry = aTable->contents[i];
    count = 0;
    while(entry != NULL) {
    count++;
    entry = entry->next;
    }

    if(count < min) {
      min = count;
    }

    if(count > max) {
      max = count;
    }

    totalLen = totalLen + count;
  }

  avgLen = totalLen/aTable->size;

  struct Stats * stats = malloc(sizeof(struct Stats));
  stats->minLen = min;
  stats->maxLen = max;
  stats->avgLen = avgLen;
  stats->entryCnt = totalLen;

  return stats;
}
