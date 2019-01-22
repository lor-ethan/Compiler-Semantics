/* Author:      Ethan Lor
   Created:     09/30.18
   Resources:   https://linux.die.net/man/3/fgets
                https://www.geeksforgeeks.org/difference-strlen-sizeof-string-c-reviewed/
                https://stackoverflow.com/questions/1716296/why-does-printf-not-flush-after-the-call-unless-a-newline-is-in-the-format-strin
                https://stackoverflow.com/questions/8107826/proper-way-to-empty-a-c-string
                https://alvinalexander.com/programming/printf-format-cheat-sheet
                Kala Arentz
                Daniel Bartelson
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "IOMngr.h"

FILE *sourceFile;
char buffer[MAXLINE];
int bufLen, nextPos;
int curLine;
bool echoAll;
bool needsDisplay;

struct message {
  int startColumn;
  int endColumn;
  char * message;
};

#define MAX_MESSAGES 26
struct message messages[MAX_MESSAGES];
int messageCnt;

void WriteStartSeq(int messageNum) {
  fprintf(stdout,"\033[7m %c \033[0m\033[4m",'A' + messageNum);
}

void WriteResetSeq() {
  fprintf(stdout,"\033[0m");
}

void WriteMessages() {
  int j = 0;
  for(int i = 0; i < messageCnt; i++) {
    if(messages[i].startColumn >= 0) {
      fprintf(stdout, "%5d: ", curLine);
      while(j < messages[i].startColumn) {
        fprintf(stdout, "%c", buffer[j]);
        j++;
      }

      WriteStartSeq(i);
        
      while(j < messages[i].endColumn) {
        fprintf(stdout, "%c", buffer[j]);
        j++;
      }
        
      WriteResetSeq();
    }
  }
      
  while(j < strlen(buffer)) {
    fprintf(stdout, "%c", buffer[j]);
    j++;
  }

  for(int i = 0; i < messageCnt; i++) {
    fprintf(stdout, "%8s%c %s\n", "-", 'A' + i, messages[i].message);
    free(messages[i].message);
  }
      
  messageCnt = 0;
  needsDisplay = false;
  return;
}

void WriteBuffer() {
  if(echoAll == true) {
    if(needsDisplay == true) {
      WriteMessages();
    } else {
      fprintf(stdout, "%5d: %s", curLine, buffer);
    }
  } else {
    if(needsDisplay == true) {
      WriteMessages();
    }
  }
}

bool OpenSource(const char * aFileName, bool mode) {
  sourceFile = fopen(aFileName,"r");
  if (!sourceFile) {
    fprintf(stdout,"Error: Can not open source file %s.\n", aFileName);
    return false;
  }

  fgets(buffer, MAXLINE, sourceFile);
  bufLen = strlen(buffer);
  nextPos = 0;
  curLine = 1;
  echoAll = mode;
  needsDisplay = false;
  messageCnt = 0;
  return true;
}

void CloseSource() {
  if(strlen(buffer) != 0 || messageCnt != 0) {
    WriteBuffer();
  }
  fclose(sourceFile);
}

char GetSourceChar() {
  int curPos = nextPos;
  if(strlen(buffer) == 0) {
    return EOF;
  }

    if(nextPos >= strlen(buffer)) {
    WriteBuffer();
    if(!fgets(buffer, MAXLINE, sourceFile)) {
      buffer[0] = '\0';
      return EOF;
    }

    curLine++;
    nextPos = 0;
    return GetSourceChar();
  } else {
    nextPos++;
    return buffer[curPos];
  }
}

void PostMessage(int aColumn, int aLength, const char * aMessage) {
  if (messageCnt < MAX_MESSAGES && aColumn < MAXLINE) {
    if(messageCnt >= 1 && aColumn <= messages[messageCnt - 1].endColumn && aColumn >= messages[messageCnt - 1].startColumn) {
      return;
    }

    if(aColumn < 0) {
      messages[messageCnt].startColumn = aColumn;
      messages[messageCnt].message = strdup(aMessage);
      messageCnt++;
      needsDisplay = true;
      return;
    }

    messages[messageCnt].startColumn = aColumn;

    if(aColumn + aLength > strlen(buffer) - 1) {
      messages[messageCnt].endColumn = strlen(buffer) - 1;
    } else {
      messages[messageCnt].endColumn = aColumn + aLength;
    }

    messages[messageCnt].message = strdup(aMessage);
    messageCnt++;
    needsDisplay = true;
  }
}

int GetCurrentLine() {
  return curLine;
}

int GetCurrentColumn() {
  return nextPos - 1;
}
