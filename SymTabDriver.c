/* SymTabDriver.c

   This program minimally exercises the Symbol Table implementation.

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

#include "SymTab.h"

#define MAX(a,b) ( (a) < (b) ? (b) : (a) )

// Identifier table uses only one kind of attribute
enum AttrKinds { IGNORE_KIND, INT_KIND };
// An identifiers attributes consist of two integers.
struct Attributes {
  int value1; // read from input
  int value2; // used to count # of times a name is seen in the input
};

// =======================
// Utility Functions
void
ErrorExit(char * msg) {
  fprintf(stderr,"%s\n",msg);
  exit(1);
}

void
ConditionalErrorExit(bool cond, char * msg) {
  if (!cond) return;
  fprintf(stderr,"%s\n",msg);
  exit(1);
}

// =======================
// Functions related to building and using a table of input commands.
#define MAX_LINE 64
char inputBuf[MAX_LINE];

enum commands {UNKNOWN_CMD = 0, CREATETABLE_CMD, INSERT_CMD, LOOKUP_CMD, DISPLAYTABLE_CMD, TRANSFERTABLE_CMD, DESTROYTABLE_CMD, CMD_LIST_END};
char * cmdNames[] = {"unused", "createtable", "insert", "lookup", "displaytable", "transfertable", "destroytable"};
struct SymTab * cmdTable;

void
InitCmdTable() {
  cmdTable = CreateSymTab(7,NULL,NULL);
  ConditionalErrorExit(!cmdTable,"CreateSymTab did not return a command table.\n");
  for (int i = 1; i < CMD_LIST_END; i++) {
    struct SymEntry * entry = EnterName(cmdTable,cmdNames[i]);
    SetAttr(entry,INT_KIND,(void *)(long)i);
  }
}

int
LookupCmd(char * cmd) {
  struct SymEntry * entry = LookupName(cmdTable,cmd);
  if (!entry) return UNKNOWN_CMD;
  return (long) GetAttr(entry);
}

// =======================
// Functions for processing and implementing various action commands.
void
ExtractArgs(char * buf, int cnt, char * args[]) {
  char * seps = " \n";
  char * arg;
  int i = 0;
  for (arg = strtok(buf,seps); (arg && i < cnt); arg = strtok(NULL,seps)) {
    args[i++] = arg;
  }
}

struct SymTab *
TableForName(struct SymTab * tablesTable, char * name) {
  return (name) ? GetAttr(LookupName(tablesTable,name)) : NULL;
}

void
DisplayEntry(struct SymEntry * entry, int cnt, void * ignore) {
  struct Attributes * attr = GetAttr(entry);
  char * scope = GetScopePath(GetTable(entry));
  printf("%3d %-25s %20s %5d %5d\n",cnt,scope,GetName(entry),attr->value1,attr->value2);
  free(scope);
}

void
TransferEntry(struct SymEntry * entry, int cnt, void * destTable) {
  struct Attributes * attr = GetAttr(entry);
  char * scope = GetScopePath(GetTable(entry));
  const char * name = GetName(entry);
  char * newName = malloc(strlen(scope) + strlen(name) + 2);
  strcpy(newName,scope);
  strcat(newName,":");
  strcat(newName,name);
  printf("transfer %s\n",newName);
  struct SymEntry * newEntry = EnterName((struct SymTab * )destTable,newName);
  struct Attributes * newAttr = malloc(sizeof(struct Attributes));
  newAttr->value1 = attr->value1;
  newAttr->value2 = 0;
  SetAttr(newEntry,IGNORE_KIND,newAttr);
  cnt++;
  free(scope);
  free(newName);
}

void
FreeEntryAttr(struct SymEntry * entry, int cnt, void * ignore) {
  // in a more complicated situation this function might
  // first get the attrKind of the entry and then do the
  // appropriate thing based upon the kind
  struct Attribute * attr = GetAttr(entry);
  if (attr) free(attr);
}

// =======================
// Function to implement sanity test. If this fails main testing cannot succeed.
char * testNames[] = {"zero", "one", "two", "three", "four", "five"};

void
SanityTest() {
  fprintf(stderr,"Start Minimal Sanity Checks\n");
  struct SymTab * table = CreateSymTab(5,NULL,NULL);
  ConditionalErrorExit(!table,"CreateSymTab did not return a table.\n");
  struct SymEntry * entry = EnterName(table,"#DummyTable");
  ConditionalErrorExit(!entry,"EnterName did not return entry.\n");
  ConditionalErrorExit(entry != LookupName(table,"#DummyTable"),"LookupName does not return entered entry\n");
  for (int i = 0; i < sizeof(testNames)/sizeof(char *); i++) {
    struct SymEntry * entry = EnterName(table,testNames[i]);
    SetAttr(entry,INT_KIND,(void *)(long)i);
  }
  for (int i = 0; i < sizeof(testNames)/sizeof(char *); i++) {
    struct SymEntry * entry = LookupName(table,testNames[i]);
    long v = (long)GetAttr(entry);
    ConditionalErrorExit(v != i, "Attribute not correct for entered name\n");
  }
  fprintf(stderr,"Sanity Checks Complete\n");
}

// =======================
// Function to implement full testing.
void
ProcessTestFile(const char * testFileName) {
	FILE * fd = fopen(testFileName,"r");
  if (!fd) ErrorExit("Can't open input file.\n");

  InitCmdTable();

  // create table to hold the names of all the tables created through the
  // createtable command
  struct SymTab * tablesTable = CreateSymTab(10,NULL,NULL);

  // the current table for all inserts
  struct SymTab * insertTable = NULL;

  printf("Start Processing Input\n");

  while (fgets(inputBuf,MAX_LINE,fd)) {
    char * args[4] = {NULL, NULL, NULL, NULL};
    ExtractArgs(inputBuf,4,args);
    switch(LookupCmd(args[0])) {
      case CREATETABLE_CMD: {
        // createtable table-size table-name [ parent-name ]
        //printf("createtable %d %s %s\n",atoi(args[1]),args[2],args[3]);
        ConditionalErrorExit(!args[1],"createtable: no first arg");
        ConditionalErrorExit(!args[2],"createtable: no second arg");
        ConditionalErrorExit(TableForName(tablesTable,args[2]),"createTable: scope already exists");
        struct SymTab * parentTable = TableForName(tablesTable,args[3]);
        struct SymTab * table = CreateSymTab(atoi(args[1]),args[2],parentTable);
        struct SymEntry * entry = EnterName(tablesTable,args[2]);
        ConditionalErrorExit(!entry,"createtable: EnterName failed");
        ConditionalErrorExit(entry != LookupName(tablesTable,args[2]),"createtable: LookupName failed to give entered name");
        SetAttr(entry,IGNORE_KIND,table);
        insertTable = table;
        printf("created %s\n",GetScopeName(table));
      } break;
      case INSERT_CMD: {
        // insert name attr-value
        //printf("insert %s %s\n",args[1],args[2]);
        struct SymEntry * entry = LookupName(insertTable,args[1]);
        if (entry) {
          struct Attributes * attr = GetAttr(entry);
          attr->value2++;
          char * scope = GetScopePath(GetTable(entry));
          printf("%s present in scope %s with attr %d %d\n",args[1],scope,attr->value1,attr->value2);
          free(scope);
        }
        if (!entry || GetTable(entry) != insertTable) {
          entry = EnterName(insertTable,args[1]);
          struct Attributes * attr = malloc(sizeof(struct Attributes));
          attr->value1 = atoi(args[2]);
          attr->value2 = 0;
          SetAttr(entry,IGNORE_KIND,attr);
          printf("entered %s with %d\n",args[1],attr->value1);
        }
      } break;
      case LOOKUP_CMD: {
        // lookup name
        //printf("lookup %s \n",args[1]);
        struct SymTab * table = insertTable;
        while (table) {
          struct SymEntry * entry = LookupName(table,args[1]);
          if (entry) {
            char * scope = GetScopePath(GetTable(entry));
            printf("%s found in table %s\n",args[1],scope);
            free(scope);
            table = GetParentTable(GetTable(entry));
          }
          else {
            table = NULL;
          }
        }
      } break;
      case DISPLAYTABLE_CMD: {
        // displaytable table-name
        //printf("displaytable %s\n",args[1]);
        ConditionalErrorExit(!args[1],"displaytable: no first arg");
        struct SymTab * table = TableForName(tablesTable,args[1]);
        ConditionalErrorExit(!table,"displaytable: table does not exist");
        if (!table) break;
        struct SymEntry * entry = NULL;
        int cnt = 0;
        char * tableScope = GetScopePath(table);
        printf("Table: %s\n",tableScope);
        free(tableScope);
        printf("Cnt Scope                                     Name  Attr\n");
        InvokeOnEntries(table,true,DisplayEntry,0,NULL);
      } break;
      case TRANSFERTABLE_CMD: {
        // transfertable old-table new-size new-table-name
        //printf("transfertable %s %s %s\n",args[1],args[2],args[3]);
        ConditionalErrorExit(!args[1],"transfertable: no first arg");
        ConditionalErrorExit(!args[2],"transfertable: no second arg");
        ConditionalErrorExit(!args[3],"transfertable: no third arg");
        struct SymTab * srcTable = TableForName(tablesTable,args[1]);
        ConditionalErrorExit(!srcTable,"transfertable: source table does not exist");
        ConditionalErrorExit(TableForName(tablesTable,args[3]),"transfertable: destination table name already exists");
        struct SymTab * destTable = CreateSymTab(atoi(args[2]),args[3],NULL);
        ConditionalErrorExit(!destTable,"transfertable: failed to create destination table");
        struct SymEntry * tableEntry = EnterName(tablesTable,args[3]);
        SetAttr(tableEntry,IGNORE_KIND,destTable);
        struct SymEntry * entry = NULL;
        int cnt = 0;
        InvokeOnEntries(srcTable,true,TransferEntry,0,destTable);
      } break;
      case DESTROYTABLE_CMD: {
        // destroytable table-name
        ConditionalErrorExit(!args[1],"destroytable: no first arg");
        struct SymTab * table = TableForName(tablesTable,args[1]);
        ConditionalErrorExit(!table,"destroytable: table does not exist");
        struct Stats * stats = Statistics(table);
        printf("Statistics min: %d max: %d avg: %d total: %d\n",
               stats->minLen,stats->maxLen,stats->avgLen,stats->entryCnt);
        free(stats);
        InvokeOnEntries(table,false,FreeEntryAttr,0,NULL);
        DestroySymTab(table);
        printf("destroyed %s\n",args[1]);
      } break;
      case UNKNOWN_CMD:
        printf("unknown\n");
        break;
    }
  }

  DestroySymTab(tablesTable);
  DestroySymTab(cmdTable);
  fclose(fd);

  return;
}

int
main(int argc, char * argv[]) {
  char *testFileName;
  FILE *fd;

  switch (argc) {
    case 1:
      SanityTest();
      break;
    case 2:
      ProcessTestFile(argv[1]);
      break;
    default:
      fprintf(stderr,"usage: SymTabDriver [data-file-name]");
      break;
  }
}
