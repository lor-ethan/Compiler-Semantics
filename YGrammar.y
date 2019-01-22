/* Author:      Ethan Lor
   Created:     11/3.18
   Resources:   Kala Arentz
                Benjamin Klipfel
                Sam Lor
                Benjamin Polzin
                Landon Rudy
*/

%{
  #include <stdio.h>
  #include <string.h>
  #include <stdlib.h>
  #include "IOMngr.h"
  #include "Scanner.h"
  #include "YSemantics.h"

  void yyerror(char *s);
  %}

/* Union structure of data that can be attached to non-terminals   */
%union     {
// real-type union-field-name
  int Integer;
  char * Text;
  struct IdList * IdList;
  enum BaseTypes BaseType;
  struct InstrSeq * InstrSeq;
  enum Operators Operators;
  struct ExprResult * ExprResult;
  enum Comparisons Comparisons;
  struct CondResult * CondResult;
}

/* Type declaration for data attached to non-terminals. Allows     */
/* $# notation to be correctly type casted for function arguments. */
//    <union-field-name> non-terminal                           
%type <Text> Id
%type <Text> String

%type <IdList> DeclList
%type <IdList> DeclItem

%type <BaseType> Type

%type <InstrSeq> DeclImpls
%type <InstrSeq> FuncBody
%type <InstrSeq> FuncStmts
%type <InstrSeq> Stmt
%type <InstrSeq> AssignStmt
%type <InstrSeq> PutStmt
%type <InstrSeq> IfStmt
%type <InstrSeq> IfElseStmt
%type <InstrSeq> WhileStmt

%type <ExprResult> Factor
%type <ExprResult> Expr
%type <ExprResult> Term

%type <Operators> MultOp
%type <Operators> AddOp

%type <CondResult> Cond

%type <Comparisons> BoolOps

/* List of token name and corresponding numbers */
/* y.tab.h will be generated from these */
%token IDENT_TOK  	1
%token DECL_TOK   	2
%token IMPL_TOK   	3
%token INT_TOK      4
%token CHR_TOK      5
%token PUT_TOK      6
%token GET_TOK      7
%token INT_LIT_TOK  8
%token CHR_LIT_TOK  9
%token IF_TOK       10
%token ELSE_TOK     11
%token WHILE_TOK    12
%token LT_TOK       13
%token LTE_TOK      14
%token GT_TOK       15
%token GTE_TOK      16
%token EQUAL_TOK    17
%token NEQUAL_TOK   18
%token STR_LIT_TOK  19
// can't go past 32 without conflicting with single char tokens
// could use larger token numbers


%%

Module        : DeclImpls                                       { Finish(); };

DeclImpls     : Decl DeclImpls                                  { };
DeclImpls     : Impl DeclImpls                                  { };
DeclImpls     :                                                 { };

Decl          : DECL_TOK DeclList ':' Type  ';'                 { ProcDecls($2,$4); };
DeclList      : DeclItem ',' DeclList                           { $$ = AppendIdList($1,$3); };
DeclList      : DeclItem                                        { $$ = $1; };

DeclItem      : Id                                              { $$ = ProcName($1,PrimType); };
DeclItem      : Id FuncArgTypes                                 { $$ = ProcName($1,FuncType); };

Id            : IDENT_TOK                                       { $$ = strdup(yytext); };
FuncArgTypes  : '(' ')'                                         { };

Type          : INT_TOK                                         { $$ = IntBaseType; };
Type          : CHR_TOK                                         { $$ = ChrBaseType; };

Impl          : IMPL_TOK Id FuncArgNames FuncBody ';'           { ProcFunc($2,$4); };
FuncArgNames  : '(' ')'                                         { };
FuncBody      : '{' FuncStmts '}'                               { $$ = $2; };

FuncStmts     : Stmt ';' FuncStmts                              { $$ = AppendSeq($1,$3); };
FuncStmts     :                                                 { $$ = NULL; };

Stmt          : AssignStmt                                      { $$ = $1; };
Stmt          : PutStmt                                         { $$ = $1; };
Stmt          : IfStmt                                          { $$ = $1; };
Stmt          : IfElseStmt                                      { $$ = $1; };
Stmt          : WhileStmt                                       { $$ = $1; };

IfStmt        : IF_TOK '(' Cond ')' FuncBody                    { $$ = IfFunc($3,$5); };
IfElseStmt    : IF_TOK '(' Cond ')' FuncBody ELSE_TOK FuncBody  { $$ = IfElseFunc($3,$5,$7); };
WhileStmt     : WHILE_TOK '(' Cond ')' FuncBody                 { $$ = WhileFunction($3,$5);};

Cond          : Expr BoolOps Expr                               { $$ = CondOperation($1,$2,$3); };

BoolOps       : LT_TOK                                          { $$ = LessThan; };
BoolOps       : LTE_TOK                                         { $$ = LessEqual; };
BoolOps       : GT_TOK                                          { $$ = GreaterThan; };
BoolOps       : GTE_TOK                                         { $$ = GreaterEqual; };
BoolOps       : EQUAL_TOK                                       { $$ = Equal; };
BoolOps       : NEQUAL_TOK                                      { $$ = NotEqual; };

AssignStmt    : Id '=' Expr                                     { $$ = Assign($1,$3); };

Expr          : Expr AddOp Term                                 { $$ = AddOp($1, $2, $3); };
Expr          : Term                                            { $$ = $1; };

Term          : Term MultOp Factor                              { $$ = MultOp($1, $2, $3); };
Term          : Factor                                          { $$ = $1; };

Factor        : '(' Expr ')'                                    { $$ = $2; };
Factor        : '-' '(' Expr ')'                                { $$ = NegateExpr($3); };
Factor        : INT_LIT_TOK                                     { $$ = LoadInt(yytext); };
Factor        : CHR_LIT_TOK                                     { $$ = LoadChar(yytext); };
Factor        : Id                                              { $$ = LoadId($1); };
Factor        : GET_TOK '(' Type ')'                            { $$ = GetExpr($3); };

AddOp         : '+'                                             { $$ = Add; };
AddOp         : '-'                                             { $$ = Sub; };

MultOp        : '*'                                             { $$ = Mult; };
MultOp        : '/'                                             { $$ = Div; };

PutStmt       : PUT_TOK '(' Expr ')'                            { $$ = PutExpr($3); };
PutStmt       : PUT_TOK '(' String ')'                          { $$ = PutStr($3); };

String        : STR_LIT_TOK                                     { $$ = StrLiteral(yytext); };

%%

void
yyerror( char *s)     {
  char msg[MAXLINE];
  sprintf(msg,"ERROR \"%s\" token: \"%s\"",s,yytext);
  PostMessage(GetCurrentColumn(),strlen(yytext),msg);
}
