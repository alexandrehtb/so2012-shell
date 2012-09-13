#include "string.h"
#include "builtincmd.h"
#include "ioredirect.h"

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

#define SHELL_NAME "shell"

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
 * Executa o comando expecificado por argv
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
 * Faz o parser de um comando nao built in. Verifica se ele deve ser executado
 * em foreground ou background e chama createProcess com a devida flag setada.
 */
void nonBuiltIn(char **argv)
{
	int fg = 0;
	//char **argv = parser(cmd, " ");

	createProcess(argv, fg);

	if (argv)
		freeCommand(argv);
	free(argv);
}



int main(void)
{
	int fd_out, fd_in, fd_stdout, fd_stdin, outflag = 0, inflag = 0;
	char *out = NULL, *in = NULL;

	shellPrompt();

	while(TRUE) {
		char *cmd = readLine(stdin);
		char **tokens = parser(cmd, " ");

		if (*cmd == '\0') {
			free(cmd);
			continue;
		}

		outflag = redirectOutput(tokens, &out); // verifica se ha redirecionamento de saida

		if (outflag) {
			// redireciona a saida
			fd_stdout = dup (1);
			close (1);

			if (outflag == 1)
				fd_out = open(out, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
			else
				fd_out = open(out, O_CREAT | O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);

			if (fd_out < 0) {
				fprintf(stderr, "Erro: criando arquivo para redirecionamento de saida.\n");
				exit(EXIT_FAILURE);
			}

			//fd_stdout = dup (1);
			//close (1);
/*
			if (dup (fd_out) < 0) {
				fprintf(stderr, "Erro: redirecionando saida.\n");
				exit(EXIT_FAILURE);
			}

			close (fd_out);
			*/
		}

		inflag = redirectInput(tokens, &in);

		if (inflag) {
			// redireciona a entrada
			fd_stdin = dup (0);
			close (0);

			fd_in = open(in, O_RDONLY);

			if (fd_in < 0) {
				fprintf(stderr, "Erro: abrindo arquivo para derirecionamento de entrada.\n");
				exit(EXIT_FAILURE);
			}
/*
			close (0);

			if (dup (fd_in) < 0) {
				fprintf(stderr, "Erro: redirecionando entrada.\n");
				exit(EXIT_FAILURE);
			}

			close (fd_in);
			*/
		}

		if (builtInCommand(tokens) == 0)
			nonBuiltIn(tokens);

		// redireciona de volta para a saida padrao
		if (outflag) {
			close (fd_out);
			dup (fd_stdout);
			close (fd_stdout);
		}

		// redireciona de volta para a entrada padrao
		if (inflag) {
			close (fd_in);
			dup (fd_stdin);
			close (fd_stdin);
		}

		shellPrompt();
		free(cmd);
	}

	return 0; 
}
