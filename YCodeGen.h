/* CodeGen.h
   Routines to support the generation of assembly code.
 
   Code is maintained as linked list of struct InstrSeq. 
 
   InitCodeGen    - does module initialization and sets filename for writing
                    out assembly code
   GenInstr       - create a struct InstrSeq node from supplied fields
   AppendSeq      - concatenates two instruction sequences into one
   WriteSeq       - write instruction sequence to output file
 
   GenLabel       - return string of form "L####" to use as label
 
   AvailTmpReg    - return the index of an available register
   TmpRegName     - returns string to use for temporary register RegNum, string
                    is of the form "$t#"
   ReleaseTmpReg  - marks register as being available
   ResetAllTmpReg - marks all registers as being available
 
   SaveSeq        - creates the instruction sequence to save all in use 
                    registers prior to a function call
   RestoreSeq     - creates the instruction sequence to restore all in use
                    registers after a function call
 
   Imm            - string to use as the immediate of value
   RegOff         - string to use for register plus offset, of the form
                    "%d(%s)"
*/

#include <stdio.h>

struct InstrSeq {
  struct InstrSeqNode * head;
  struct InstrSeqNode * tail;
};

void							 InitCodeGen(char *AFilename);
void               CloseCodeGen();

struct InstrSeq *  GenInstr(char *Label, char *OpCode,
                            char *Oprnd1, char *Oprnd2, char *Oprnd3);
struct InstrSeq *  AppendSeq(struct InstrSeq *Seq1,
                                   struct InstrSeq *Seq2);
void               FreeSeq(struct InstrSeq * seq);
void 						   WriteSeq(struct InstrSeq *ASeq);

// returned value must be freed
char *						 GenLabel();

// functions for working with registers
int  						   AvailTmpReg();
char *						 TmpRegName(int RegNum);
void							 ReleaseTmpReg(int ANum);
void 	             ResetAllTmpReg();
void               DebugRegs();

// must be strdup'd immediately since private buffer is returned
// should these be const?
char *						 Imm(int Val);
char *						 RegOff(int Offset, char * Reg);

// function calling conventions
//int                FrameSize();
struct InstrSeq *  CalleeProlog();
struct InstrSeq *  CalleeEpilog();

