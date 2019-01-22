# CS 442 - Compiler Construction
## Semantics Part I

This repository contains files to be addded to your main project repository to include the semantic processing capabilities to compile programs of the Y language (one of the few letters for which there is not a language). The repository contains

- Y.c
 - This is the test driver for the assignment. This driver takes one argument, the base name of the source file. It appends ".src" and ".asm" "to the base name and opens these files. It then starts the main parse function produced by yacc/bison. 

- YGrammar.y
 - The yacc/bison input describing the grammar rules and actions. This file contains a starting point that covers all of the required things and production rules for declaration statements. You will need to complete this file. 

- YScanner.l
 - The lex/flex input describing the tokens required by the parser. It should include comment nesting capabilities.
 
- YCodeGen.h/.c
 - The contains support functions for creating, manipulating and writing assembly code sequences. The implementation is complete.

- YSemantics.h/.c
 - This will contain the implementation of the semantic actions used in compilation. It has stubs for the semantic actions used for variable declarations as well as some of the required data structures.

- Makefile
- This file has been updated to include rules for this portion of the project. There are targets "y#test" for each of the five test files as well as a target "ytest" that runs all tests. 

- y#.src/.in/.asm, yfactors.src/.in/.asm
 - Various source files for testing.

Detailed information on the MIPS instruction set and SPIM simulator can be found at

* [http://pages.cs.wisc.edu/~larus/HP_AppA.pdf](http://pages.cs.wisc.edu/~larus/HP_AppA.pdf)

Testing will be done using the spim command line simulator. On OS X this can be obtained with "brew install spim". On linux it can be obtained with "sudo apt-get install spim". Downloads at [https://sourceforge.net/projects/spimsimulator/files/](https://sourceforge.net/projects/spimsimulator/files/). Another alternative is the MARS simulator but there are some differences in behavior. 

## The Assignment

- Step 1, 25 pts
  - Implement the necessary semantic actions for variable declarations. y1.src tests this part. 
  - Notes on most of the things you need to do and remember.
        - YGrammar.y contains grammar rules for declarations and corresponding semantic action functions. Variable declarations are of the form "decl x, y, z : int;". It also lists the required "union" fields and the correspondence of nonterminals to "union" fields.
        - YSemantics.h contains the declaration of the semantic action functions, the symbol table identifier attribute struct, a struct IdList to hold symbol table entry references and a struct TypeDesc to represent a type. 
        - YSemantics.c contains some global variables, an init function, a list symbol table function and stubs for the semantic action functions. 
        - struct IdList is used to form a linked list of SymTab entries as identifiers are encountered
        - struct TypeDesc is used to define a declaration type - it may become more complicated in the future
        - struct Attr is used with SymTab entries to carry information about an identifier
        - ProcDecls() takes a linked list of struct IdList along with a base type and updates the symbol table entry's attributes to reflect the delcared type. 
        - AppendIdList() joins a new struct onto to an existing list
        - ProcName() places an identifier in the symbol table, after checking if it is already present, and creates/sets an empty struct attr
        - ProcFunc() creates the instruction sequence for the function including entry and return and attaches it to the function identifiers attr struct

![Decl](SemanticsTrees-Decl.png)

- Step 2, 25 pts
 - Add rules and semantic actions to compile simple expressions and assignment statements. Implement get() and put() functions for I/O of simple data types. Get() should be allowed in place of an identifier in expressions and put() should be considered a statement on its own. These are implemented in assembly with syscalls. Y2.src tests this part. 
 - Notes on most of the things you need to do and remember.
        - Write grammar rules for an assignment statement and make it one of the options for Stmt. AssignStmt should be "Id '=' Expr".
        - Expr should be "Expr AddOp Term" or "Term". Term should be "Term MultOp Factor" or "Factor". Factor can be a parenthesized expression, the negation of a factor, an integer/character literal or an identifier.
        - Create a struct ExprResult which holds both the code sequence that computes the expression and the register number where the result is located when the code executes. This will get added as an option for the "union" and associated with certain nonterminals. You may also want an enumerated type for the various operators or simply use the operator character itself. 
        - The semantic action for compiling the AddOp will receive an ExprResult for the incoming Expr and Term. The incoming code sequences are concatenated and an instruction for the operator is generated and added to the end. The registers for the operator instruction come from the incoming ExprResults and a new temp register obtained from the CodeGen module. 
        - For an integer literal an ExprResult is created containing a load immediate ("li") instruction with a new temp register. An identifier produces a load word ("lw").
        - The AssignStmt receives an Id and an ExprResult, generates a store word ("sw") and appends to the end of the ExprResult code sequence and passes along the instruction sequence (not the whole ExprResult). 
        - CodeGen module usage: AvailTmpReg() returns the number of an available register. TmpRegName(reg#) takes a register number and returns the char string name for use in an instruction (e.g. TmpRegName(3) returns "$t3"). ReleaseTmpReg(reg#) make the register available again. For every call to AvailTmpReg() there must somewhere be a corresponding ReleaseTmpReg() call.
        - In compiling an operator, when the language gets more complex, it will be necessary to check that the operands are of compatible types. This means that the ExprResult struct should also have a field for the result type.
        - The action for get() will generate the instructions for the syscall to read an integer and put the result in a new temp register. It will return an ExprResult.
        - The put() statement will generate the instruction for the syscall to write an integer. There are syscalls to print an integer and to print a character. 
        - ChrLit should allow things like 'a' and '\n'.

![Decl](SemanticsTrees-Expr.png)

- Step 3, 25 pts
 - Add rules and semantic actions for "if/then/else" and "while" loops. Y3.src tests this part. 
 - Notes on most of the things you need to do and remember
        - need a CondResult struct similiar to ExprResult, remembers the code sequence and the label used in the conditional branch instruction
        - conditional branch is opposite of the comparison operator, instructions are bge, bg, ble,bl, bne, beq
        - common syntax does allow for dangling else, yacc gives shift/reduce warning but prefers shift over the reduce which effectively associates an else to the inner if
        - if/then/else uses label generated by condition as first thing following then sequence, when an else clause is present, then sequence ends with branch to new label which follows the else sequence and the condition label comes at the beginning of the else clause
        - while needs label at start of condition instruction sequence, conditional branch label follows the body, the body ends with a branch to the top of the loop

![Decl](SemanticsTrees-If.png)

- Step 4, 25 pts
 - Add rules and semantic actions to allow string literals in put() statements. Y4.src tests this part. 
 - Notes on most of the things you need to do and remember
        - use a separate symbol table to hold string literals, need different attribute record for string literals than the one used for identifiers, for string literals the attribute remembers the label used for the string literal in the data section

- There is a fifth test program, yFactors.src that exercises all of the steps to calcuate the factors of a given number. 

 
## What To Do

- Do not fork this repository on Gitlab, instead
- On your development machine, in a location different than your project repository, clone this project. Your project repository will continue to accumulate all files for the project. 
- Copy the files from this repository to your project repository.
- Discard the clone of this repository.
- Implement the required capabilities. Make sure your author header comment is in YSemantics.c.
- When complete, "make clean" to remove object files and executables, use git to add and commit your changes and push the project back up to your repository. Remember to create an issue in Gitlab with me as the assignee indicating that your submission is ready for grading. 

