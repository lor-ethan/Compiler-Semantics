/* Author:      Ethan Lor
   Created:     11/3.18
   Resources:   Kala Arentz
                Benjamin Klipfel
                Sam Lor
                Benjamin Polzin
                Landon Rudy
*/

Letter	[A-Za-z]
Digit   [0-9]

%{
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

#include "YSemantics.h"
#include "y.tab.h"

%}

%%
\/\*                            { commentLevel++; }
\*\/                            { commentLevel--; }
int                             { if (!IN_COMMENT) RETURN(INT_TOK,1); }
chr                             { if (!IN_COMMENT) RETURN(CHR_TOK,1); }
decl                            { if (!IN_COMMENT) RETURN(DECL_TOK,1); }
impl                            { if (!IN_COMMENT) RETURN(IMPL_TOK,1); }
put                           	{ if (!IN_COMMENT) RETURN(PUT_TOK,1); }
get 							              { if (!IN_COMMENT) RETURN(GET_TOK,1); }
if                            	{ if (!IN_COMMENT) RETURN(IF_TOK,1); }
else                          	{ if (!IN_COMMENT) RETURN(ELSE_TOK,1); }
while 							            { if (!IN_COMMENT) RETURN(WHILE_TOK,1); }
{Letter}({Letter}|{Digit})*		  { if (!IN_COMMENT) RETURN(IDENT_TOK,0); }
-?{Digit}+ 						          { if (!IN_COMMENT) RETURN(INT_LIT_TOK,0); }
'([^'\\\n]|\\.)' 				        { if (!IN_COMMENT) RETURN(CHR_LIT_TOK,0); }
\"(\\.|[^"\\])*\" 				      { if (!IN_COMMENT) RETURN(STR_LIT_TOK,0);}
\(                              { if (!IN_COMMENT) RETURN('(',1); }
\)                              { if (!IN_COMMENT) RETURN(')',1); }
\{                              { if (!IN_COMMENT) RETURN('{',1); }
\}                              { if (!IN_COMMENT) RETURN('}',1); }
\;                              { if (!IN_COMMENT) RETURN(';',1); }
\:                              { if (!IN_COMMENT) RETURN(':',1); }
\,                              { if (!IN_COMMENT) RETURN(',',1); }
\= 								              { if (!IN_COMMENT) RETURN('=',1); }
\+ 								              { if (!IN_COMMENT) RETURN('+',1); }
\- 								              { if (!IN_COMMENT) RETURN('-',1); }
\* 								              { if (!IN_COMMENT) RETURN('*',1); }
\/ 								              { if (!IN_COMMENT) RETURN('/',1); }
\<                             	{ if (!IN_COMMENT) RETURN(LT_TOK,1); }
\<\=                            { if (!IN_COMMENT) RETURN(LTE_TOK,1); }
\>                             	{ if (!IN_COMMENT) RETURN(GT_TOK,1); }
\>\=                            { if (!IN_COMMENT) RETURN(GTE_TOK,1); }
\=\=                            { if (!IN_COMMENT) RETURN(EQUAL_TOK,1); }
\#\#				 			              { if (!IN_COMMENT) RETURN(NEQUAL_TOK,1); }
[ \t]                           { }
\/\/.*                          { lineComment = 1; }
[\n]                            { lineComment = 0; }
.                               { }
%%
