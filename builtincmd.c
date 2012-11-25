#include "builtincmd.h"
#include "string.h"
#include "jobs.h"
#include "signals.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

extern job_node *jobs;
extern const char *history[HISTORY_MAX_SIZE];
extern unsigned int history_count;


/**
 * Executa o comando 'change-directory'. Se um parametro tiver sido passado tenta
 * mudar o diretorio para este caso contrario muda para o diretorio 'HOME'. Se o diretorio
 * nao existir exibe uma mensagem de erro e finaliza.
 */

int cd (char *cmd) {
	char **tokens = parser(cmd, "  ");

	if (tokens[1] == NULL)
		chdir(getenv("HOME"));
	else {
		if (chdir(tokens[1]) == -1)
			fprintf(stderr, "cd: %s: Diretorio nao encontrado\n", tokens[1]);
	}

	return 1;
}


/**
 * Abre e exibe o arquivo de 'help'.
 */

void help (void) {
	int fd;
	char ptr;

	fd = open("help", O_RDONLY);

	putchar('\n');
	while (read(fd, &ptr, 1))
		printf("%c", ptr);
	putchar('\n');

	close(fd);
}


/**
 * Chama a funcao 'print_jobs' definida no arquivo jobs.c.
 */

void show_jobs(void) {
	print_jobs(jobs);
}


/**
 * Coloca a job com o pid especificado em foreground.
 */

void fg(char *cmd) {
	char **tokens = parser(cmd, " ");

	if (!tokens[1]) {
		fprintf(stderr, "\n\n Erro de sintaxe: fg espera como argumento um pid.\n");
		return;
	}

	pid_t target = strtol(tokens[1], NULL, 10);
	invoke_fg(target);
}


/**
 * Coloca a job com pid especificado em background.
 */

void bg(char *cmd)
{
    char **tokens = parser(cmd, " ");

    if (!tokens[1]) {
	fprintf(stderr, "\n\n Erro de sintaxe: bg espera como argumento um pid.\n");
	return;
    }

    pid_t target = strtol(tokens[1], NULL, 10);
    invoke_bg(target);
}


/**
 * Rotina para finalizar um processo: envia um SIGKILL para o processo especificado.
 */

void _kill(char *cmd) {
	char **tokens = parser(cmd, " ");

	if (!tokens[1]) {
		fprintf(stderr, "\n\n Erro de sintaxe: kill espera como argumento um pid.\n");
		return;
	}

	pid_t target = strtol(tokens[1], NULL, 10);
	kill(target, SIGKILL);
	delete_job(target);
}


/**
 * Exibe o historico de comandos digitados. HISTORY_MAX_SIZE definido em 'builtincmd.h'.
 */

void _history(void) {
	int i = 0, count = history_count - HISTORY_MAX_SIZE;
	if (count < 0)
		count = 0;
	while (history[i]) {
		printf(" %d %s\n", count, history[i]);
		i++; count++;
	}
}


/**
 * Verifica se o comando digitado pelo usuario eh um built-in. Em caso positivo chama a rotina
 * que irah trata-lo e retorna 1. Caso contrario retorna 0.
 */

int builtInCommand (char *cmd) {
	if (strcmp(cmd, "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
	else if (strncmp(cmd, "cd", 2) == 0) {
		return cd(cmd);

		return 1;
	}
	else if (strcmp(cmd, "help") == 0) {
		help();

		return 1;
	}
	else if (strcmp(cmd, "jobs") == 0) {
		show_jobs();

		return 1;
	}
	else if (strncmp(cmd, "fg", 2) == 0) {
		fg(cmd);

		return 1;
	}
	else if (strncmp(cmd, "bg", 2) == 0) {
		bg(cmd);

		return 1;
	}
	else if (strcmp(cmd, "history") == 0) {
		_history();

		return 1;
	}
	else if (strncmp(cmd, "kill", 4) == 0) {
		_kill(cmd);

		return 1;
	}

	return 0;
}

