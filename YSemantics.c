/* Author:      Ethan Lor
   Created:     11/3.18
   Resources:   Kala Arentz
                Benjamin Klipfel
                Sam Lor
                Benjamin Polzin
                Landon Rudy
*/

/* Semantics.c
 Support and semantic action routines.

 */

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "YSemantics.h"

// Shared Data

struct SymTab * IdentifierTable;
struct SymTab * StringLiteralTable;
enum AttrKinds { VOID_KIND = -1, INT_KIND, STRING_KIND, STRUCT_KIND };

char * BaseTypeNames[2] = { "int", "chr"};
char * TypeNames[2] = { "", "func"};
struct InstrSeq * globalCode;

// corresponds to enum Operators
char * Ops[] = { "add", "sub", "mul", "div"};
// char * BoolOps[] = { "and", "or", "not" };

// corresponds to negation of enum Comparisons
// enum Comparisons { LtCmp, LteCmp, GtCmp, GteCmp, EqCmp, NeqCmp };
char * Branches[] = { "bge", "bgt", "ble", "blt", "bne", "beq"};
// comparison set instructions, in order with enum Comparisons
//char * CmpSetReg[] = { "slt", "sle", "sgt", "sge", "seq", "sne" };


// Supporting Routines
void
PostMessageAndExit(int col, char * message) {
  PostMessage(col,1,message);
  CloseSource();
  exit(0);
}

void
InitSemantics() {
  IdentifierTable = CreateSymTab(100,"global",NULL);
  StringLiteralTable = CreateSymTab(100,"global",NULL);
  globalCode = GenInstr(NULL,".data",NULL,NULL,NULL);
}

char *
StringForType(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
      return strdup(BaseTypeNames[desc->primDesc]);
    } break;
    case FuncType: {
      return strdup(BaseTypeNames[desc->funcDesc->returnType]);
    } break;
  }
}

struct TypeDesc *
MakePrimDesc(enum BaseTypes type) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = PrimType;
  desc->primDesc = type;
  return desc;
}

struct TypeDesc *
MakeFuncDesc(enum BaseTypes returnType) {
  struct TypeDesc * desc = malloc(sizeof(struct TypeDesc));
  desc->declType = FuncType;
  desc->funcDesc = malloc(sizeof(struct FuncDesc));
  desc->funcDesc->returnType = returnType;
  desc->funcDesc->funcCode = NULL;
  return desc;
}

void displayEntry(struct SymEntry * entry, int cnt, void * ignore) {
  char * scope = GetScopePath(GetTable(entry));
  printf("%3d %-20s %-15s",cnt,scope,GetName(entry));
  free(scope);
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        printf("%-10s",attr->reference);
        printf("     ");
        struct TypeDesc * desc = attr->typeDesc;
        char * typeStr = StringForType(desc);
        printf("%-10s ",typeStr);
        free(typeStr);
      }
      else {
        printf("empty");
      }
    } break;
  }
  printf("\n");
}

void
ListIdentifierTable() {
  printf("\nIdentifier Symbol Table\n");
  printf("Num Scope                Name           Ref       Kind Type\n");
  InvokeOnEntries(IdentifierTable,true,displayEntry,0,NULL);
}

void
FreeTypeDesc(struct TypeDesc * desc) {
  switch (desc->declType) {
    case PrimType: {
    } break;
    case FuncType: {
      if (desc->funcDesc->funcCode) FreeSeq(desc->funcDesc->funcCode);
      free(desc->funcDesc);
    } break;
  }
  free(desc);
}

// provided to the SymTab for destroying table contents
void
FreeEntryAttr(struct SymEntry * entry) {
  int attrKind = GetAttrKind(entry);
  struct Attr * attr = GetAttr(entry);
  switch (attrKind) {
    case VOID_KIND: {
    } break;
    case INT_KIND: {
    } break;
    case STRING_KIND: {
    } break;
    case STRUCT_KIND: {
      if (attr) {
        if (attr->typeDesc)  FreeTypeDesc(attr->typeDesc);
        if (attr->reference) free(attr->reference);
        free(attr);
      }
    } break;
  }
}

void processGlobalIdentifier(struct SymEntry * entry, int cnt, void * dataCode) {
  struct Attr * attr = GetAttr(entry);
  switch (attr->typeDesc->declType) {
    case PrimType: {

    } break;
    case FuncType: {
      // nothing to do here
    } break;
  }
}

void processFunctions(struct SymEntry * entry, int cnt, void * textCode) {
  struct Attr * attr = GetAttr(entry);
  switch (attr->typeDesc->declType) {
    case PrimType: {
      // nothing to do here
    } break;
    case FuncType: {
      if (!attr->typeDesc->funcDesc->funcCode) {
        PostMessageAndExit(tokStrt,"function never implemented");
      }

    } break;
  }
}

// Semantics Actions

void
Finish() {
  printf("Finish\n");
  ListIdentifierTable();

  struct InstrSeq * textCode = GenInstr(NULL,".text",NULL,NULL,NULL);
  struct InstrSeq * dataCode = GenInstr(NULL,".data",NULL,NULL,NULL);

  // form module preamble
  struct SymEntry * mainEntry = LookupName(IdentifierTable,"main");
  if (!mainEntry) {
    PostMessageAndExit(tokStrt,"no main function for module");
  }
  // should make sure main takes no arguments
  struct Attr * mainAttr = GetAttr(mainEntry);
  // need to keep spim happy
  AppendSeq(textCode,GenInstr(NULL,".globl","__start",NULL,NULL));
  AppendSeq(textCode,GenInstr("__start",NULL,NULL,NULL,NULL));
  AppendSeq(textCode,GenInstr(NULL,"jal",mainAttr->reference,NULL,NULL));
  AppendSeq(textCode,GenInstr(NULL,"li","$v0","10",NULL));
  AppendSeq(textCode,GenInstr(NULL,"syscall",NULL,NULL,NULL));

  // run SymTab with InvokeOnEntries putting globals in data seg
  for(int i = 0; i < IdentifierTable->size; i++){
    struct SymEntry * idEntry = IdentifierTable->contents[i];
    while (idEntry){
      struct Attr * attr = GetAttr(idEntry);
      if(attr->typeDesc->declType == PrimType){
        AppendSeq(dataCode, GenInstr(attr->reference,".word","0",NULL,NULL));
      }
      else{
        AppendSeq(textCode, attr->typeDesc->funcDesc->funcCode);
      }
      idEntry = idEntry->next;
    }
  }

  for(int i = 0; i < StringLiteralTable->size; i++){
    struct SymEntry * content = StringLiteralTable->contents[i];
    while (content){
      struct StringLiteral * attr = GetAttr(content);
      AppendSeq(dataCode, attr->instrSeq);
      free(attr->name);
      free(attr);
      content = content->next;
    }
  }

  // run SymTab with InvokeOnEntries putting functions in code seq

  // combine and write
  struct InstrSeq * moduleCode = AppendSeq(textCode,dataCode);
  WriteSeq(moduleCode);

  // free code
  FreeSeq(moduleCode);
  CloseCodeGen();
  fprintf(stderr,"Finish \n");
}

void
ProcDecls(struct IdList * idList, enum BaseTypes baseType) {
  // walk IdList items
  // switch for prim or func type
  // names on IdList are only specified as PrimType or FuncType
  // set type desc
  // for Sem1 everthing is in global scope, otherwise check scope
  // attr reference string is id name with prepended "_"
  struct IdList * tempIdList = idList;
  while(tempIdList){
    struct Attr * newAttr = GetAttr(tempIdList->entry);
    if(GetAttrKind(tempIdList->entry) == PrimType){
      newAttr->typeDesc = MakePrimDesc(baseType);
      char * name = calloc(strlen(tempIdList->entry->name)+2, sizeof(char));
      sprintf(name,"_%s",tempIdList->entry->name);
      AppendSeq(globalCode,GenInstr(name,".word","0",NULL,NULL));
    } else {
      newAttr->typeDesc = MakeFuncDesc(baseType);
    }
    newAttr->reference = calloc(strlen(tempIdList->entry->name)+2, sizeof(char));
    sprintf(newAttr->reference, "_%s", tempIdList->entry->name);

    SetAttr(tempIdList->entry, STRUCT_KIND, newAttr);
    tempIdList = tempIdList->next;
  }
}

struct IdList *
AppendIdList(struct IdList * item, struct IdList * list) {
  item->next = list;
  return item;
}

struct IdList *
ProcName(char * id, enum DeclTypes type) {
  // get entry for id, error if it exists
  // enter id in symtab
  // create IdList node for entry
  // create and partially init type descriptor, completed in ProcDecls
  // create, init and set attr struct
  if(LookupName(IdentifierTable, id)){
    printf("Entry already exists: %s", id);
  }

  struct SymEntry * newEntry = EnterName(IdentifierTable, id);
  newEntry->attrKind = type;
  struct IdList * newIdList = calloc(1, sizeof(struct IdList));
  struct Attr * newAttr = calloc(1, sizeof(struct Attr));
  struct TypeDesc * newTypeDesc = calloc(1, sizeof(struct TypeDesc));
  newAttr->typeDesc = newTypeDesc;
  SetAttr(newEntry, type, newAttr);
  newIdList->entry = newEntry;

  free(id);

  return newIdList;
}

void
ProcFunc(char * id, struct InstrSeq * instrs) {
  // lookup name
  // get attr
  // gen instr for function entry
  // include function body code
  // function exit code, i.e. jump return
  struct SymEntry * entry = LookupName(IdentifierTable, id);
  if( !entry ){
    printf("error in ProcFunc()");
  }
  struct Attr * attr = GetAttr(entry);
  struct InstrSeq * instr = GenInstr(attr->reference, NULL, NULL, NULL, NULL);
  AppendSeq(instr, instrs);
  AppendSeq(instr, GenInstr(NULL,"jr","$ra",NULL,NULL));
  attr->typeDesc->funcDesc->funcCode = instr;
}

struct InstrSeq *
Assign(char * id, struct ExprResult * expr){
  char * name = calloc(strlen(id) + 2, sizeof(char));
  sprintf(name, "_%s", id);

  struct InstrSeq * instr = expr->instrSeq;
  AppendSeq(instr, GenInstr(NULL,"sw",TmpRegName(expr->registers),name,NULL));
  ReleaseTmpReg(expr->registers);

  free(id);
  return instr;
}

struct ExprResult *
AddOp(struct ExprResult * leftExpr,enum Operators operators,struct ExprResult * rightExpr ){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));

  result->registers = AvailTmpReg();
  struct InstrSeq * instr =  leftExpr->instrSeq;
  AppendSeq(instr, rightExpr->instrSeq);

  if ( operators == Add) {
    AppendSeq(instr,GenInstr(NULL,"add",TmpRegName(result->registers),TmpRegName(leftExpr->registers),TmpRegName(rightExpr->registers)));
  } else {
    AppendSeq(instr,GenInstr(NULL,"sub",TmpRegName(result->registers),TmpRegName(leftExpr->registers),TmpRegName(rightExpr->registers)));
  }
  ReleaseTmpReg(leftExpr->registers);
  ReleaseTmpReg(rightExpr->registers);
  result->instrSeq = instr;
  return result;
}

struct ExprResult *
MultOp(struct ExprResult * leftExpr,enum Operators operators,struct ExprResult * rightExpr ){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));
  result->registers = AvailTmpReg();
  struct InstrSeq * instr = leftExpr->instrSeq;

  AppendSeq(instr, rightExpr->instrSeq);

  if ( operators == Mult ) {
    AppendSeq(instr, GenInstr(NULL,"mul",TmpRegName(result->registers),TmpRegName(leftExpr->registers),TmpRegName(rightExpr->registers)));
  } else {
    AppendSeq(instr, GenInstr(NULL,"div",TmpRegName(result->registers),TmpRegName(leftExpr->registers),TmpRegName(rightExpr->registers)));
  }
  ReleaseTmpReg(leftExpr->registers);
  ReleaseTmpReg(rightExpr->registers);
  result->instrSeq = instr;
  return result;
}

struct ExprResult *
NegateExpr(struct ExprResult * expr){
  int tempReg1 = AvailTmpReg();
  int tempReg2 = AvailTmpReg();

  AppendSeq(expr->instrSeq, GenInstr(NULL,"li",TmpRegName(tempReg1),"-1",NULL));
  AppendSeq(expr->instrSeq, GenInstr(NULL,"mult",TmpRegName(tempReg2),TmpRegName(tempReg1),TmpRegName(expr->registers)));

  expr->registers = tempReg2;
  ReleaseTmpReg(tempReg1);

  return expr;
}

struct ExprResult *
LoadInt(char * i){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->registers = AvailTmpReg();
  result->instrSeq = GenInstr(NULL,"li",TmpRegName(result->registers),i,NULL);
  result->type = IntBaseType;

  return result;
}

struct ExprResult *
LoadChar(char * c){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));
  result->registers = AvailTmpReg();
  result->type = ChrBaseType;
  int value = (int)c[1];

  char str[4];

  if(c[2] == 'n'){
    sprintf(str, "%d", 10);
  } else {
    sprintf(str, "%d", value);
  }

  result->instrSeq = GenInstr(NULL,"li",TmpRegName(result->registers),str, NULL);

  return result;
}

struct ExprResult *
LoadId(char * id){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));
  result->registers = AvailTmpReg();

  struct SymEntry * entry = LookupName(IdentifierTable,id);
  if ( !entry ){
    printf("error! in LoadId()");
  }
  struct Attr * attr = GetAttr(entry);
  result->type = attr->typeDesc->primDesc;

  char * name = calloc(strlen(id) + 2, sizeof(char));
  sprintf(name, "_%s", id);

  result->instrSeq = GenInstr(NULL,"lw",TmpRegName(result->registers),name,NULL) ;

  free(id);
  return result;
}

struct ExprResult *
GetExpr(int type){
  struct ExprResult * result = calloc(1, sizeof(struct ExprResult));
  result->instrSeq = calloc(1, sizeof(struct InstrSeq));
  result->registers = AvailTmpReg();

  if ( type == IntBaseType ){
    result->instrSeq = GenInstr(NULL,"li","$v0","5",NULL);
  } else if ( type == ChrBaseType ){
    result->instrSeq = GenInstr(NULL,"li","$v0","12",NULL);
  }
  AppendSeq(result->instrSeq, GenInstr(NULL,"syscall",NULL,NULL,NULL));
  AppendSeq(result->instrSeq, GenInstr(NULL,"move",TmpRegName(result->registers),"$v0",NULL));

  return result;
}

struct InstrSeq *
PutExpr(struct ExprResult * expr){
  struct InstrSeq * instr = expr->instrSeq;
  if ( expr->type == IntBaseType ) {
    AppendSeq(instr, GenInstr(NULL,"li","$v0","1",NULL));
  } else {
    AppendSeq(instr, GenInstr(NULL,"li","$v0","11",NULL));
  }

  AppendSeq(instr, GenInstr(NULL,"move","$a0",TmpRegName(expr->registers),NULL));
  AppendSeq(instr, GenInstr(NULL,"syscall",NULL,NULL,NULL));

  ReleaseTmpReg(expr->registers);
  return instr;
}

struct InstrSeq *
IfFunc(struct CondResult * condResult, struct InstrSeq * instrSeq){
  struct InstrSeq * instr = condResult->instrSeq;
  AppendSeq(instr, instrSeq);
  AppendSeq(instr, GenInstr(condResult->label,NULL,NULL,NULL,NULL));
  return instr;
}

struct InstrSeq *
IfElseFunc(struct CondResult * condResult, struct InstrSeq * ifInstrSeq, struct InstrSeq * elseInstrSeq){
  char * ifLabel = GenLabel();
  struct InstrSeq * instr = condResult->instrSeq;
  AppendSeq(instr, ifInstrSeq);
  AppendSeq(instr, GenInstr(NULL,"b",ifLabel,NULL,NULL));
  AppendSeq(instr, GenInstr(condResult->label,NULL,NULL,NULL,NULL));
  AppendSeq(instr, elseInstrSeq);
  AppendSeq(instr, GenInstr(ifLabel, NULL,NULL,NULL,NULL));

  free(condResult->label);
  free(condResult);
  free(ifLabel);

  return instr;
}

struct InstrSeq *
WhileFunction(struct CondResult * condResult, struct InstrSeq * instrSeq){
    char * newLabel = GenLabel();
    struct InstrSeq * instr = GenInstr(newLabel,NULL,NULL,NULL,NULL);
    AppendSeq(instr, condResult->instrSeq);
    AppendSeq(instr, instrSeq);
    AppendSeq(instr, GenInstr(NULL,"b",newLabel, NULL,NULL));
    AppendSeq(instr, GenInstr(condResult->label ,NULL,NULL,NULL,NULL));
    free(newLabel);
    free(condResult->label);
    free(condResult);

    return instr;
}

struct CondResult *
CondOperation(struct ExprResult *leftExpr, enum Comparisons condition, struct ExprResult *rightExpr){
  struct CondResult * result = calloc(1,sizeof(struct CondResult));
  result->instrSeq = leftExpr->instrSeq;
  AppendSeq(result->instrSeq, rightExpr->instrSeq);

  result->label = GenLabel();
  AppendSeq(result->instrSeq, GenInstr(NULL,Branches[condition],TmpRegName(leftExpr->registers),TmpRegName(rightExpr->registers),result->label));

  ReleaseTmpReg(leftExpr->registers);
  ReleaseTmpReg(rightExpr->registers);

  return result;
}

struct InstrSeq *
PutStr(char * s){
  struct InstrSeq * instr = calloc(1, sizeof(struct InstrSeq));
  instr = GenInstr(NULL,"li","$v0","4",NULL);
  AppendSeq(instr, GenInstr(NULL,"la","$a0",s,NULL));
  AppendSeq(instr, GenInstr(NULL,"syscall",NULL,NULL,NULL));

  return instr;
}

char *
StrLiteral(char * c){
  struct StringLiteral * str;
  struct SymEntry * entry = LookupName(StringLiteralTable, c);

  if ( entry ) {
    str = GetAttr(entry);
  } else {
    struct SymEntry * newEntry = EnterName(StringLiteralTable, c);
    str = calloc(1,sizeof(struct StringLiteral));
    str->name = GenLabel();
    str->instrSeq = GenInstr(str->name, ".asciiz", c, NULL,NULL);
    SetAttr(newEntry, StrBaseType, str);
  }

   return str->name;
}
