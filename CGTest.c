#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "YCodeGen.h"

int
main(int argc, char * argv[]) {
  
  InitCodeGen("CGTest.asm");
  
  int aNum = AvailTmpReg();
  char * aName = TmpRegName(aNum);
  printf("register %d has name %s\n",aNum,aName);
  int bNum = AvailTmpReg();
  char * bName = TmpRegName(bNum);
  printf("register %d has name %s\n",bNum,bName);
  int cNum = AvailTmpReg();
  char * cName = TmpRegName(cNum);
  printf("register %d has name %s\n",cNum,cName);
  
  char * aLabel = GenLabel();
  char * bLabel = GenLabel();
  printf("label %s\n",aLabel);
  printf("label %s\n",bLabel);
  
  struct InstrSeq * code = GenInstr(NULL,".text",NULL,NULL,NULL);
  code = AppendSeq(code,GenInstr(aLabel,"li",Imm(5),aName,NULL));
  code = AppendSeq(code,GenInstr(NULL,"lw","_x",bName,NULL));
  code = AppendSeq(code,GenInstr(NULL,"add",aName,bName,cName));

  ReleaseTmpReg(aNum);
  ReleaseTmpReg(bNum);
  
  code = AppendSeq(code,GenInstr(bLabel,NULL,NULL,NULL,NULL));
  code = AppendSeq(code,GenInstr(NULL,".data",NULL,NULL,NULL));
  code = AppendSeq(code,GenInstr("_x",".word",Imm(0),NULL,NULL));
  
  ReleaseTmpReg(aNum);
  ReleaseTmpReg(bNum);
  
  WriteSeq(code);
  FreeSeq(code);
  
  CloseCodeGen();
}
