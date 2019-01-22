Letter	[A-Za-z]
Digit   [0-9]

%{
#include "RDTokens.h"
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

%}

%%
int                { RETURN(INT_TOK,1); }
chr                { RETURN(CHR_TOK,1); }
\:=                { RETURN(ASSIGN_TOK,1); }
\;                 { RETURN(SEMI_TOK,1); }
\(				   { RETURN(LPAREN_TOK,1); }
\)				   { RETURN(RPAREN_TOK,1); }
\+				   { RETURN(PLUS_TOK,1); }
\-				   { RETURN(MINUS_TOK,1); }
\*				   { RETURN(TIMES_TOK,1); }
\/				   { RETURN(DIV_TOK,1); }
\{				   { RETURN(LBRACE_TOK,1); }
\}                 { RETURN(RBRACE_TOK,1); }
\,                 { RETURN(COMMA_TOK,1); }
{Letter}+	       { RETURN(IDENT_TOK,0); }
{Digit}+	       { RETURN(INTLIT_TOK,0); }
{Digit}*\.{Digit}* { RETURN(FLOATLIT_TOK,0); }
[ \n\t]		       { }
.				   { }
%%
