/* Author:      Ethan Lor
   Created:     11/2.18
   Resources:   Kala Arentz
                Benjamin Klipfel
*/

Letter	[A-Za-z]
Digit   [0-9]

%{
#include "y.tab.h"
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

int commentLevel = 0;
int lineComment = 0;
#define IN_COMMENT (lineComment || commentLevel > 0)

%}

%%

  /* Combine comment nesting from Scanner.l with RDScanner.l regular
     expressions except for real literal. Use RETURN(token-symbolic-name)
     where token-symbolic-name appears in yacc file.
   */

\/\*                            { commentLevel++; }
\*\/                            { commentLevel--; }
int                             { if (!IN_COMMENT && !lineComment) RETURN(INT_TOK,1); }
chr                             { if (!IN_COMMENT && !lineComment) RETURN(CHR_TOK,1); }
\:=                             { if (!IN_COMMENT && !lineComment) RETURN(ASSIGN_TOK,1); }
\;                              { if (!IN_COMMENT && !lineComment) RETURN(';',1); }
\(                              { if (!IN_COMMENT && !lineComment) RETURN('(',1); }
\)                              { if (!IN_COMMENT && !lineComment) RETURN(')',1); }
\+                              { if (!IN_COMMENT && !lineComment) RETURN('+',1); }
\-                              { if (!IN_COMMENT && !lineComment) RETURN('-',1); }
\*                              { if (!IN_COMMENT && !lineComment) RETURN('*',1); }
\/                              { if (!IN_COMMENT && !lineComment) RETURN('/',1); }
\{                              { if (!IN_COMMENT && !lineComment) RETURN('{',1); }
\}                              { if (!IN_COMMENT && !lineComment) RETURN('}',1); }
\,                              { if (!IN_COMMENT && !lineComment) RETURN(',',1); }
{Letter}+                       { if (!IN_COMMENT && !lineComment) RETURN(IDENT_TOK,0); }
-?{Digit}+                      { if (!IN_COMMENT && !lineComment) RETURN(INTLIT_TOK,0); }
\/\/.*                          { lineComment = 1; }
[\n]                            { lineComment = 0; }
[ \t]                           { }
.                               { }
%%
