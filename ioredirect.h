#ifndef IOREDIRECT_H
#define IOREDIRECT_H

void ensure_close(int closeret);
void ensure_dup2(int dup2ret);
int redirectOutput (char **tokens, char **out);
int redirectInput (char **tokens, char **in);
void ensure_close(int closeret);
void ensure_dup2(int dup2ret);

#endif
