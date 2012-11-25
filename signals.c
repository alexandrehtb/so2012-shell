#include "jobs.h"
#include "signals.h"

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

extern job_node *jobs;
extern void shellPrompt(void);

static struct sigaction act_TSTP;
static struct sigaction oldact_TSTP;

static struct sigaction act_CHLD;
static struct sigaction oldact_CHLD;

static struct sigaction action_INT;
static struct sigaction old_INT;

void handle_INT(int sig, siginfo_t *info, void *context) {
	job_type *fgjob = get_fg_job();

	if (fgjob != NULL) {
		kill(fgjob->pid, SIGKILL);
		delete_job(fgjob->pid);
		putchar('\n');
		return;
	}
	putchar('\n');
	shellPrompt();
}

/**
* Trata sinais TSTP.
*/

void TSTP_handler(int sig, siginfo_t *info, void *context) {
	job_type *fgjob = get_job(1, BY_FOREGROUND_JOB);

	if (!fgjob) {
		putchar('\n');
		shellPrompt();
		return;
	}
	else {
		fgjob->foreground = 0;

		if (fgjob->status != finished) {
			fgjob->status = paused;
			kill(fgjob->pid, SIGSTOP);
		}
		return;
	}
	putchar('\n');
	shellPrompt();
}


/**
* Trata sinais CHLD.
*/

void CHLD_handler(int sig, siginfo_t *info, void *context) {
	int status;
	waitpid(info->si_pid, &status, WUNTRACED | WCONTINUED);
	job_type *j = get_job(info->si_pid, BY_PROCESS_ID);
	if (j)
		update_status(j, status);
	//changeStatus(get_job(info->si_pid, BY_PROCESS_ID), status);
}

/**
* Redireciona os sinais para seus devidos handlers.
*/

void initializeSignals(void) {
	sigemptyset(&action_INT.sa_mask);
	action_INT.sa_flags = SA_SIGINFO;
	action_INT.sa_sigaction = handle_INT;

	if (sigaction(SIGINT, &action_INT, &old_INT) == -1)
		fprintf(stderr, "\n\tErro ao tentar instalar handler para SIGINT."); 

	sigemptyset(&act_TSTP.sa_mask);
	act_TSTP.sa_flags = SA_SIGINFO;
	act_TSTP.sa_sigaction = TSTP_handler;

	if (sigaction(SIGTSTP, &act_TSTP, &oldact_TSTP) == -1)
		fprintf(stderr, "\n\tErro ao redirecionar SIGTSTP.");


	sigemptyset(&act_CHLD.sa_mask);
	act_CHLD.sa_flags = SA_SIGINFO;
	act_CHLD.sa_sigaction = CHLD_handler;

	if (sigaction(SIGCHLD, &act_CHLD, &oldact_CHLD) == -1)
		fprintf(stderr, "Erro ao redirecionar SIGCHLD.");
}


/**
* Redireciona sinais do processo filho.
*/

void initializeChildrenSetup(void) {
	sigaction(SIGTSTP, &oldact_TSTP, NULL);
	sigaction(SIGCHLD, &oldact_CHLD, NULL);
	sigaction(SIGINT, &old_INT, NULL);
}


