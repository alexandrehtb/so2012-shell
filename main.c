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
	int fd, outflag = 0;
	char *out = NULL;

	shellPrompt();

	while(TRUE) {
		char *cmd = readLine(stdin);
		char **tokens = parser(cmd, " ");

		if (*cmd == '\0') {
			free(cmd);
			continue;
		}

		outflag = redirectOutput(tokens, out);

		if (outflag) {
			// redireciona a saida
			int i = 0;
			while (tokens[i]) {
				printf("%s\n", tokens[i]);
				i++;
			}

			fd = open ("out.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);

			if (fd < 0) {
				fprintf(stderr, "Erro: criando arquivo para redirecionamento.\n");
				exit(EXIT_FAILURE);
			}

			close (1);

			if (dup (fd) < 0) {
				fprintf(stderr, "Erro: redirecionando saida.\n");
				exit(EXIT_FAILURE);
			}

			close (fd);
		}

		if (builtInCommand(tokens) == 0)
			nonBuiltIn(tokens);

		if (outflag) {
			close (1);
			dup (open("/dev/tty", O_WRONLY));
		}

		shellPrompt();
		free(cmd);
	}

	return 0; 
}
