/* Author:      Ethan Lor
   Created:     10/07.18
   Resources:   http://epaperpress.com/lexandyacc/pry1.html
                http://theholeintheace.blogspot.com/2012/11/how-to-run-lex-and-yacc-programs-on.html
                Kala Arentz
                Travis Waldvogel
*/
/* ScannerDriver.c

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ScanTokens.h"
#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"

#define MESSAGE_SIZE 256
#define ACTION_MESSAGE_SIZE 32

struct Attributes {
  int firstLine;
  int firstColumn;
  int cnt;
};
enum AttrKinds { IGNORE_KIND };

// used with InvokeOnEntries()
void collectEntries(struct SymEntry * entry, int cnt, void * entryArray) {
  ((struct SymEntry **)entryArray)[cnt] = entry;
}

// used with qsort to sort list of symbol table entries
int entryCmp(const void * A, const void * B) {
  // A is pointer to element of array which contains a pointer to a struct SymEntry
  const char * strA = GetName(*(struct SymEntry **)A);
  const char * strB = GetName(*(struct SymEntry **)B);
  return strcmp(strA,strB);
}

int main(int argc, char **argv) {
  int Token;
  char message[MESSAGE_SIZE];
  char actionMessage[ACTION_MESSAGE_SIZE];
  struct SymTab * table = NULL;

  bool ret = OpenSource("ScannerSource",true);
  if (!ret) {
    printf("Could not open source and listing files\n");
    exit(1);
  }

  int intSum = 0;
  float floatSum = 0;

  while ((Token = yylex()) != 0) {
    snprintf(actionMessage,ACTION_MESSAGE_SIZE," ");
    switch(Token) {
      case INIT_TOK: {
        // create a symbol table
        if (!table) table = CreateSymTab(20,"main",NULL);
      } break;
      case IDENT_TOK: {
        // place the identifier in the table (if it exists), if new then create and init
        // attributes structure, if already in table then update attributes cnt field, in
        // each case set actionMessage with one of
        // sprintf(actionMessage," -- No SymbolTable");
        // sprintf(actionMessage," -- occurrence %d",attr->cnt);
        // sprintf(actionMessage," -- new ident");
        if (!table) {
          snprintf(actionMessage,ACTION_MESSAGE_SIZE," -- No SymbolTable");
          break;
        }
        struct SymEntry * entry = LookupName(table, yytext);
        if (entry) {
          struct Attributes * attr = GetAttr(entry);
          attr->cnt++;
          sprintf(actionMessage," -- occurrence %d",attr->cnt);
          break;
        } else {
          struct SymEntry * entry = EnterName(table, yytext);
          struct Attributes * attr = malloc(sizeof(struct Attributes));
          attr->firstLine = GetCurrentLine();
          attr->firstColumn = GetCurrentColumn() - yyleng;
          attr->cnt = 1;
          SetAttr(entry, IGNORE_KIND, attr);
          sprintf(actionMessage," -- new ident");
        }
      } break;
      case INT_TOK: {
        intSum = intSum + atoi(yytext);
      } break;
      case FLOAT_TOK: {
        floatSum = floatSum + atof(yytext);
      } break;
      case DUMP_TOK: {
        fprintf(stderr,"---------\n");
        fprintf(stderr,"intSum = %d\n",intSum);
        fprintf(stderr,"floatSum = %f\n",floatSum);
        // get table statistics, alloc an array to hold entry pointers
        struct Stats * stats = Statistics(table);
        struct SymEntry ** entries = malloc(stats->entryCnt * sizeof(struct SymEntry *));

        // enumerate the table collecting entry pointers into the array
        InvokeOnEntries(table,false,collectEntries,0,entries);

        // sort the entries
        qsort(entries,stats->entryCnt,sizeof(struct SymEntry *),entryCmp);

        // list the contents of the table in sorted order
        fprintf(stderr,"\nContents of Symbol Table\n");
        fprintf(stderr,"                    name  line   col   cnt \n");
        for (int i = 0; i < stats->entryCnt; i++) {
          fprintf(stderr,"%3d %20s %5d %5d %5d\n", i,
                 GetName(entries[i]),
                 ((struct Attributes *) GetAttr(entries[i]))->firstLine,
                 ((struct Attributes *) GetAttr(entries[i]))->firstColumn,
                 ((struct Attributes *) GetAttr(entries[i]))->cnt);
        }
        free(stats);
      } break;
    }
    sprintf(message,"Token#=%d, Length=%lu, Text=\"%s\"%*c%s",Token,yyleng,yytext,(int)(15-yyleng),' ',actionMessage);
    PostMessage(tokStrt,yyleng,message);
  }
  CloseSource();
}
