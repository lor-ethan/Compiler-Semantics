/* Main of Parser */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Scanner.h"
#include "IOMngr.h"
#include "Grammar.h"

int
main(int argc, char * argv[])
{
  if (argc != 2 || strlen(argv[1]) == 0) {
    fprintf(stderr,"usage: Parse filename\n");
    return 1;
  }

  if (!OpenSource(argv[1],true)) {
    printf("Source File Not Found.\n");
    return 1;
  }

  if (yyparse() == 0) {
    PostMessage(-1,0,"Parse Successful");
  }
  else {
    PostMessage(-1,0,"Parse Unsuccessful");
  }

  CloseSource();

  return 0;
}
