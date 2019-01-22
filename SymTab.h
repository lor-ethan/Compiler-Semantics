/* SymTab.h

  This file defines the interface to a simple symbol table.
  For the purposes of this module a symbol table is abstractly
  viewed as a collection of entries each associating a name
  with a single attribute which is taken to be a (void *).
  This allows the user to associate a name with an arbitrary
  structure of attribute information which the user creates
  and maintains. A symbol table can optionally reference a
  parent symbol table to support names in nested scopes.

*/

#ifndef SYMTAB_H
#define SYMTAB_H

#include <stdbool.h>

/* The symbol table entry structure. Contains name and attribute information.
   Definition is in the implementation file.
*/
// struct SymEntry;
struct SymEntry {
  struct SymTab * table;
  char *name;
  int attrKind;
  void *attributes;
  struct SymEntry *next;
};

/* The symbol table structure proper. Definition is in the implementation
   file.
*/
// struct SymTab;
struct SymTab {
  struct SymTab * parent;
  char *scopeName;
  int size;
  struct SymEntry **contents;
};

/* CreateSymTab    Create and return a reference to a symbol table of
                   approximately Size many entries. ScopeName is a name to
                   describe the scope of the table and maybe NULL. If
                   parentTable is not NULL, the new table will reference it.
                   Returns a pointer to the table or NULL if unable to create
                   the table.

   DestroySymTab   Destroy all storage associated with a Symbol Table which
                   is under the table's control. This function can only free
                   storage that has been allocated by this module. Any
                   storage allocated for attributes associated with an entry
                   must be freed before the table is destroyed. This can be
                   done by creating an entryFree() function and using the
                   InvokeOnEntries() function described below. destroytable
                   returns the parent table.
*/
struct SymTab *    CreateSymTab(int size, char * scopeName, struct SymTab * parentTable);
struct SymTab *    DestroySymTab(struct SymTab *aTable);

/*  HashName       Hashes name and returns the index into a specific hash table size.
*/
int HashName(int size, const char *name);

/* LookupName      Search aTable, and if necessary parent tables, for an
                   entry for aName. Returns the first entry found or NULL.

   EnterName       Create an new entry for aName in aTable if it does not
                   exist in aTable. In either case return a reference to the
                   entry.
*/
struct SymEntry *  LookupName(struct SymTab *aTable, const char * aName);
struct SymEntry *  EnterName(struct SymTab *aTable, const char * aName);

/* SetAttr         Set the attribute and "kind" associated with anEntry. The kind
                   is a user defined integer which can be used to distinguish
                   the type of data. Attributes is a "void *" and so may be
                   an address or a long int.
   GetAttrKind     Return the "kind" of the attribute.
   GetAttr         Return the attribute associated with anEntry.
   GetName         Return the name string associated with anEntry.
   GetTable        Return the table that contains anEntry.
   GetScopeName    Return the scope name of aTable (specified at creation).
   GetScopePath    Return a new string combining the scope name of aTable
                   and all parent tables. The caller is responsible for
                   freeing the returned string.
   GetParentTable  Return the parent of aTable.
*/
void               SetAttr(struct SymEntry *anEntry, int kind, void *attributes);
int                GetAttrKind(struct SymEntry *anEntry);
void *             GetAttr(struct SymEntry *anEntry);
const char *       GetName(struct SymEntry *anEntry);
struct SymTab *    GetTable(struct SymEntry *anEntry);
const char *       GetScopeName(struct SymTab *aTable);
char *             GetScopePath(struct SymTab *aTable);
struct SymTab *    GetParentTable(struct SymTab *aTable);

/* InvokeOnEntries Invoke workFunc() for each entry contained in aTable. If
                   includeParentTable is true recursively invoke for all entries
                   in parent tables. StartCnt is the enumeration number to be
                   given to the first entry found. WithArgs is a pointer to
                   additional data which will be supplied to workFunc(). WorkFunc()
                   must of of type entryWorkFunc whose calling signature consists of
                   the entry to work on, the enumeration count of the entry of the
                   additional work arguments. The implementation will invoke the
                   function with "entryFunc(someEntry,cnt,withArgs)".
*/
typedef void (*entryWorkFunc)(struct SymEntry * entry, int cnt, void * args);
void               InvokeOnEntries(struct SymTab *aTable, bool includeParentTable,
                    entryWorkFunc workFunc, int startCnt, void * withArgs);

/* Statistics      Return a structure containing statistics about aTable.
                   - minLen is the length of the shortest chain in the hash table
                   - maxLen is the length of the longest chain in the hash table
                   - avgLen is the average chain length
                   - entryCnt is the sum of the chain lengths
*/
struct Stats { int minLen;
               int maxLen;
               int avgLen;
               int entryCnt;
};
struct Stats *     Statistics(struct SymTab *aTable);

#endif
