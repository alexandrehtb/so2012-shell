#include "string.h"
#include "builtincmd.h"
#include "ioredirect.h"
#include "jobs.h"
#include "signals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>

#define TRUE 1
#define FALSE !TRUE

#define SHELL_NAME "chevron"

job_node *jobs;
const char *history[HISTORY_MAX_SIZE];
unsigned int history_count = 0;

/**
 * Exibe o prompt.
 */
void shellPrompt(void)
{
	char currentDirectory[512];

	getcwd(currentDirectory, sizeof (currentDirectory));
	printf("%s:%s> ", SHELL_NAME, currentDirectory);
}

/**
 * Adiciona o ultimo comando executado ao historico de comandos.
 */
void add_command_to_history(const char *command) {
	unsigned int index;
	if (history_count < HISTORY_MAX_SIZE) {
	        history[history_count++] = strdup( command );
	}
	else {
	        free((void *) history[0]);
	        for (index = 1; index < HISTORY_MAX_SIZE; index++)
			history[index - 1] = history[index];
		history[HISTORY_MAX_SIZE - 1] = strdup( command );
		history_count++;
	}
}

/**
 * Executa o comando expecificado por argv.
 */
void execute(char **argv) {
	execvp(argv[0], argv);
}


/**
 * Cria um novo processo. No processo filho chama execute que roda o comando em argv
 * enquanto no processo pai verifica se o novo processo deve rodar em foreground.
 * Se sim, espera pelo seu termino.
 */
void createProcess(char **argv, int fg)
{
	pid_t pid;

	if ((pid = fork()) == -1) {
		perror("Erro: fork()\n");
		exit(EXIT_FAILURE);	
	}
	else if (pid == 0) {
		execute(argv);

		fprintf(stderr, "Comando nao encontrado.\n");
		exit(EXIT_FAILURE);
	}
	else {
		waitpid(pid, NULL, 0);
	}
}

/**
 * De acordo com o comando executado no momento (counter) retorna o indice
 * da proxima posicao do vetor de comando de acordo com a ordem no pipe.
 *
 * O vetor sempre tera 3 posicoes: o comando atual, o anterior e o proximo.
 */
static int crr_pipe_index(int counter)
{
    return counter % 2;
}


/**
 * Retorna o indice anterior ao atual.
 */
static int prv_pipe_index(int counter)
{
    return crr_pipe_index(counter) == 0 ? 1 : 0;
}


/*
 * Faz a syscall pipel, passando o indice do comando sendo executado.
 */
static void pipe2(int (*pipefd)[2], int currentindex)
{
    int curr = crr_pipe_index(currentindex);

    if (pipe(pipefd[curr]) == -1)
	//errexit("pipe2");
	exit(1);
}

/**
 * Faz o parser de um comando nao built in. Verifica se ele deve ser executado
 * em foreground ou background e chama createProcess com a devida flag setada.
 */
void nonBuiltIn(char *cmd)
{
	char **commands = parser(cmd, "|");
	int pipefd[2][2], i;
	//char **argv = parser(cmd, " ");

	for (i = 0; commands[i]; i++) {
		pipe2(pipefd, i);

		int fg;
		char **tokens = get_tokens(commands[i], &fg);
/*
		createProcess(tokens, fg);

		if (argv)
			freeCommand(argv);
		free(argv);
*/
		pid_t pid = fork();
		if (pid == -1) {
			fprintf(stderr, "\n\tErro no fork().\n\n");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0) {

			int curr = crr_pipe_index(i), prev = prv_pipe_index(i);
			if (commands[i + 1])
				// Write to pipe 
				ensure_dup2(dup2(pipefd[curr][1], STDOUT_FILENO));

			if (i != 0)
				// Read from last process' reading end
				ensure_dup2(dup2(pipefd[prev][0], STDIN_FILENO)); 

			execute(tokens);

			fprintf(stderr, "\n\tComando nao reconhecido.\n\n");
			exit(EXIT_FAILURE);
		}
		else {
			setpgid(pid, 0);

			jobs = add_job(jobs, make_job(pid, *tokens, running, fg)); // Adiciona a 'job' atual na lista de jobs.

			// Se o processo foi iniciado em 'foreground' processo pai aguarda sua finalizacao
			if (fg) {
				waitpid(pid, NULL, 0);
				ensure_close( close( pipefd[ crr_pipe_index(i) ][1] ) );
			}
		}
		freeCommand(tokens);
	}
	freeCommand(commands);
}



int main(void)
{
	int fd_out, fd_in, fd_stdout, fd_stdin, outflag = 0, inflag = 0;
	char *out = NULL, *in = NULL;

	initializeSignals();

	shellPrompt();

	while(TRUE) {
		char *cmd = readLine(stdin);
		//char **tokens = parser(cmd, " ");

		if (*cmd == '\0') {
			free(cmd);
			continue;
		}

		add_command_to_history(cmd);
		if (builtInCommand(cmd) == 0)
			nonBuiltIn(cmd);

		shellPrompt();
		free(cmd);
	}

	return 0; 
}

