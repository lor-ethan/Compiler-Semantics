/* Author:      Ethan Lor
   Created:     11/3.18
   Resources:   Kala Arentz
                Benjamin Klipfel
                Sam Lor
                Benjamin Polzin
                Landon Rudy
*/

/* Semantics.h
 The action and supporting routines for performing semantics processing.
 */

#include "SymTab.h"
#include "IOMngr.h"
#include "Scanner.h"
#include "YCodeGen.h"

/* Declaration of semantic record data types used in grammar.y %union */
struct IdList {
  struct SymEntry * entry;
  struct IdList * next;
};

enum BaseTypes { IntBaseType, ChrBaseType, StrBaseType };
enum Operators { Add, Sub, Div, Mult };
enum Comparisons {LessThan, LessEqual, GreaterThan, GreaterEqual, Equal, NotEqual };
// enum IncDecOps { Increment, Decrement };

struct FuncDesc {
  enum BaseTypes returnType;
  struct InstrSeq * funcCode;
};


enum DeclTypes { PrimType, FuncType };
struct TypeDesc {
  enum DeclTypes declType;
  union {
    enum BaseTypes primDesc;
    struct FuncDesc * funcDesc;
  };
};

// Symbol Table Structures
struct Attr {
  struct TypeDesc * typeDesc;
  char * reference;
};

// Expession result
struct ExprResult {
  struct InstrSeq * instrSeq;
  int registers;
  enum BaseTypes type;
};

// Condition result structure
struct CondResult{
  struct InstrSeq * instrSeq;
  char * label;
};

// String literal structure
struct StringLiteral {
  struct InstrSeq * instrSeq;
  char * name;
};

// Supporting Routines

void InitSemantics();
void ListIdentifierTable();

struct TypeDesc * MakePrimDesc(enum BaseTypes type);
struct TypeDesc * MakeFuncDesc(enum BaseTypes returnType);

// Semantics Actions
void                    Finish();
void                    ProcDecls(struct IdList * idList, enum BaseTypes baseType);
struct IdList *         AppendIdList(struct IdList * item, struct IdList * list);
struct IdList *         ProcName(char * id, enum DeclTypes type);
void                    ProcFunc(char * id, struct InstrSeq * instrs);
struct InstrSeq *       Assign(char * id, struct ExprResult * expr);
struct ExprResult *     AddOp(struct ExprResult * leftExpr, enum Operators operators, struct ExprResult * rightExpr );
struct ExprResult *     MultOp(struct ExprResult * leftExpr, enum Operators operators, struct ExprResult * rightExpr );
struct ExprResult *     NegateExpr(struct ExprResult * expr);
struct ExprResult *     LoadInt(char * i);
struct ExprResult *     LoadChar(char * c);
struct ExprResult *     LoadId(char * id);
struct ExprResult *     GetExpr(int type);
struct InstrSeq *       PutExpr(struct ExprResult * expr);
struct InstrSeq *       IfFunc(struct CondResult * condResult, struct InstrSeq * instrSeq);
struct InstrSeq *       IfElseFunc(struct CondResult * condResult, struct InstrSeq * ifInstrSeq, struct InstrSeq * elseInstrSeq);
struct InstrSeq *       WhileFunction(struct CondResult * condResult, struct InstrSeq * instrSeq);
struct CondResult *     CondOperation(struct ExprResult *leftExpr, enum Comparisons condition, struct ExprResult *rightExpr);
struct InstrSeq *       PutStr(char * s);
char *                  StrLiteral(char * c);
