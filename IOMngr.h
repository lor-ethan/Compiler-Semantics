/* IOMngr.h

   Implements a simple interface for obtaining source file characters one at
   a time. It also produces a listing of the source file, together
   with any messages, to stdout. Depending on the configured mode, either all
   source lines are listed or only lines with associated messages.

*/
#ifndef IOMNGR_H
#define IOMNGR_H

#include <stdbool.h>

#define MAXLINE 1023

/* OpenSource       Opens aFileName as the source and sets the echoAll mode.
                    If echoAll is true then all source lines are written,
                    along with associated messages, to stdout as they are
                    completed. If echoAll is false then only source lines
                    that have associated messages are written to stdout.

   CloseSource      Closes the source file and performs other final actions.
*/
bool OpenSource(const char * aFileName, bool echoAll);
void CloseSource();

/* GetSourceChar    Returns the next character in the source file. If this
                    results in reading a new line then the current line and
                    associated messages being written to stdout. The EOF
                    character is returned if the source file is at the end
                    of file when this function is called.
*/
char GetSourceChar();

/* PostMessage      Registers a message to be associated with the current line
                    and indicated by a marker at aColumn with aLength many
                    characters written in underline mode. The letters 'A' ... 'Z'
                    are used as the markers, consequently, a maximum of 26
                    message lines are allowed for each source line. Calls to
                    PostMessage beyond this limit are silently discarded.
                    Combinations of aColumn and aLength that would extend
                    past the end of the line are capped at the line length.
                    If aColumn falls within the range of the prior message's marked
                    region it is silently discarded. Messages which are not
                    associated with specific tokens (i.e. have no column or
                    length), and consequently do not display a marker or
                    underlined text, are indicated with a negative column
                    number.
*/
void PostMessage(int aColumn, int aLength, const char * aMessage);

/* GetCurrentLine   Returns the current line number in the source file.

   GetCurrentColumn Returns the column number of the most recently returned
                    character.
*/
int GetCurrentLine();
int GetCurrentColumn();

#endif
