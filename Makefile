SHELL=/bin/bash

#==========================
# include lex and yacc libraries
# enable debugging and use gnu11 standard
#==========================
LOADLIBES = -ll -ly
YFLAGS = -d
CFLAGS = -g  -std=gnu11

# several projects use y.tab.h produced by yacc, to
# avoid using wrong y.tab.h they must each build own,
# this is used to force the application of a rule
FORCE:

#==========================
# Make rule syntax
# target: dependencies
# 	rule to build target from dependencies
#
# - no rule necessary if make already knows (i.e. .o from .c)
# - sometimes no dependencies (e.g. clean target)

#==========================
# Symbol Table Project
SymTab.o: SymTab.c SymTab.h
SymTabDriver.o: SymTabDriver.c SymTab.h
SymTabDriver: SymTabDriver.o SymTab.o

symtest: SymTabDriver
		make -s symtest-0
		make -s symtest-1

symtest-0: SymTabDriver
		echo "Test 0"
		./SymTabDriver SymData-0.in > out
		cat out
		if diff out SymData-0.out; then \
			echo -e "\n\nSUCCESS\n\n" ;\
		else \
			echo -e "\n\nFAILED\n\n" ;\
		fi
		rm out

symtest-1: SymTabDriver
		echo "Test 1"
		./SymTabDriver SymData-1.in > out
		cat out
		if diff out SymData-1.out; then \
			echo -e "\n\nSUCCESS\n\n" ;\
		else \
			echo -e "\n\nFAILED\n\n" ;\
		fi
		rm out

#===========================
# IOMngr Project
IOMngr.o: IOMngr.c IOMngr.h
IOMngrDriver.o: IOMngrDriver.c IOMngr.h
IOMngrDriver: IOMngrDriver.o IOMngr.o

iotest:	IOMngrDriver
		make -s iotest-0
		make -s iotest-1
		make -s iotest-2

iotest-0: IOMngrDriver
		echo "Test 0"
		./IOMngrDriver -e IOMngrSource > out
		cat out
		if diff out IOMngrSource-0.out; then \
			echo -e "\n\nSUCCESS\n\n" ;\
		else \
			echo -e "\n\nFAILED\n\n" ;\
		fi
		rm out

iotest-1: IOMngrDriver
		echo "Test 1"
		./IOMngrDriver IOMngrSource > out
		cat out
		if diff out IOMngrSource-1.out; then \
			echo -e "\n\nSUCCESS\n\n" ;\
		else \
			echo -e "\n\nFAILED\n\n" ;\
		fi
		rm out

iotest-2: IOMngrDriver
		echo "Test 2"
		./IOMngrDriver -eu IOMngrSource > out
		cat out
		if diff out IOMngrSource-2.out; then \
			echo -e "\n\nSUCCESS\n\n" ;\
		else \
			echo -e "\n\nFAILED\n\n" ;\
		fi
		rm out

#===========================
# Scanner Project
ScannerDriver.o: ScannerDriver.c Scanner.h SymTab.h ScanTokens.h
ScannerDriver: 	ScannerDriver.o Scanner.o SymTab.o IOMngr.o

scantest: 	ScannerDriver
	./ScannerDriver

#===========================
# Recursive Descent Project
RDSupport.o:	RDSupport.c RDSupport.h
RecDescent.o: 	RecDescent.c Scanner.h RDTokens.h
RecDescent: 	RecDescent.o RDGrammar.o RDScanner.o RDSupport.o IOMngr.o

rdtest: 	rdtest1 rdtest2 rdtest3 rdtest4 rdtest5

rdtest1: 	RecDescent
	./RecDescent -l RDSrc-1.src
rdtest2: 	RecDescent
	./RecDescent -l RDSrc-2.src
rdtest3: 	RecDescent
	./RecDescent -l RDSrc-3.src
rdtest4: 	RecDescent
	./RecDescent -l RDSrc-4.src
rdtest5: 	RecDescent
	./RecDescent -l RDSrc-5.src


#===========================
# Parser Stage 1 & 2
ParserScanner.o: ParserScanner.l IOMngr.h ParserGrammar.o y.tab.h
ParserGrammar.o: FORCE ParserGrammar.y
Parse.o: Parse.c Grammar.h Scanner.h IOMngr.h
Parse: Parse.o ParserGrammar.o ParserScanner.o IOMngr.o

partest:	parse1 parse2

parse1:	Parse
	./Parse ParSrc-1.src
parse2:	Parse
	./Parse ParSrc-2.src

#===========================
# Semantics
CGTest.o:     CGTest.c YCodeGen.h
CGTest:       CGTest.o YCodeGen.o

YCodeGen.o:	YCodeGen.c YCodeGen.h

YScanner.o: 		YScanner.l IOMngr.h YSemantics.h YGrammar.o
YGrammar.o:			FORCE YGrammar.y
YSemantics.o: 	YSemantics.c YSemantics.h
Y.o: 						Y.c Grammar.h YScanner.l IOMngr.h
Y:							Y.o SymTab.o IOMngr.o YGrammar.o YScanner.o YSemantics.o YCodeGen.o


ytest:	y1test y2test y3test y4test y5test

y1test: Y
		./Y y1

y2test: Y
		./Y y2
		spim -noexception -file y2.asm < y2.in

y3test: Y
		./Y y3
		spim -noexception -file y3.asm < y3.in

y4test: Y
		./Y y4
		spim -noexception -file y4.asm < y4.in

y5test: Y
		./Y yfactors
		spim -noexception -file yfactors.asm < yfactors.in

#===========================
# Semantics 2
sem2test:	feature1

# feature1:    Y
#              echo "Some message identifying the feature"
#              rm -f f1.asm
#              ./Y f1
#              cat f1.asm
#              spim -noexception -file f1.asm < f1.in

feature1:	Y
			echo "Increment and Decrement Test"
			rm -f yIncDec.asm
			./Y yIncDec
			cat yIncDec.asm
			spim -noexception -file yIncDec.asm

feature2:	Y
			echo "Boolean Test"
			rm -f ybool.asm
			./Y ybool
			cat ybool.asm
			spim -noexception -file ybool.asm


# Other
clean:
	rm -f *.o SymTabDriver IOMngrDriver ScannerDriver RecDescent Parse Y y.tab.h
