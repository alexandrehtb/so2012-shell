#ifndef BUILTINCMD_H
#define BUILTINCMD_H

#define HISTORY_MAX_SIZE 512

int cd (char *cmd);
void help (void);
void show_jobs(void);
void fg(char *cmd);
void bg(char *cmd);
void _kill(char *cmd);
void _history(void);
int builtInCommand (char *cmd);

#endif

