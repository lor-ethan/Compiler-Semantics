Letter	[A-Za-z]
Digit   [0-9]

%{
#include "ScanTokens.h"
#include "IOMngr.h"

#define YY_INPUT(buf,result,max_size) \
	{ int c = GetSourceChar(); \
		result = (c == EOF) ? YY_NULL : (buf[0] = c, 1); \
	}

bool shouldLog = false;
unsigned int tokStrt;

#define RETURN(val,offset) \
  { if (shouldLog) fprintf(stderr,"Token#=%d, Length=%zu, Text=\"%s\"\n",val,(unsigned long)yyleng,yytext); \
    tokStrt = GetCurrentColumn() - yyleng + offset; \
    return(val); \
  }

int IN_COMMENT = 0;
int lineComment = 0;

%}

%%
\/\*							{ IN_COMMENT++; }
\*\/ 							{ IN_COMMENT--; }
\@INIT                          { if (!IN_COMMENT && !lineComment) RETURN(INIT_TOK,1); }
\@DUMP                          { if (!IN_COMMENT && !lineComment) RETURN(DUMP_TOK,1); }
{Letter}({Letter}|{Digit})*		{ if (!IN_COMMENT && !lineComment) RETURN(IDENT_TOK,0); }
-?{Digit}+ 						{ if (!IN_COMMENT && !lineComment) RETURN(INT_TOK, 0); }
-?(({Digit}+)\.{Digit}+) 		{ if (!IN_COMMENT && !lineComment) RETURN(FLOAT_TOK, 0); }
\= 								{ if (!IN_COMMENT && !lineComment) RETURN(EQUAL_TOK, 0); }
\== 							{ if (!IN_COMMENT && !lineComment) RETURN(EQUALITY_TOK, 1); }
[ \t]                           { }
\/\/.*							{ lineComment = 1; }
[\n]                            { lineComment = 0; }
.                               { }
%%
