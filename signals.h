#ifndef SIGNALS_H
#define SIGNALS_H

#include <signal.h>

void initializeSignals(void);
void handle_INT(int sig, siginfo_t *info, void *context);
void CHLD_handler(int sig, siginfo_t *info, void *context);
void TSTP_handler(int sig, siginfo_t *info, void *context);

#endif
