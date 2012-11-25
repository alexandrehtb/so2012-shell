#ifndef STR_H
#define STR_H

#include <stdio.h>

void freeCommand(char **argv);
int commandLength(char **buffer);
char *readLine(FILE *);
char **parser(char *string, char *sep);
char **get_tokens(char *cmd, int *fg);

#endif
