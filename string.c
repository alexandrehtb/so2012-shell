#include "string.h"

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define LINE_LENGTH 512


/**
 * Libera memoria ocupada por argumentos de um dado
 * comando.
 */

void freeCommand(char **argv) {
	int i;

	for (i = 0; argv[i]; i++)
		free(argv[i]);

	free(argv);
}


/**
 * Conta o numero de comandos de um array.
 */

int commandLength(char **buffer) {
	int i;

	for (i = 0; buffer[i]; i++);

	return i;
}


/**
 * Le uma linha de comando da entrada especificada por stream.
 */

char *readLine(FILE *stream)
{
	char *ret = (char *) malloc(LINE_LENGTH);
	unsigned int i = 0, size = LINE_LENGTH;
	int input = 0;

	while ((input = fgetc(stream)) != '\n' && input != EOF) {
		if (i + 1 >= size) {
			size *= 2;
			ret = (char *) realloc(ret, size);
		}
		ret[ i++ ] = input;
	}

	ret[i] = '\0';
	return (char *) realloc(ret, i + 1); 
}


/**
 * Divide uma string em tokens de acordo com o separador especificado.
 * Retorna um ponteiro para a lista de tokens.
 */

char **parser(char *string, char *sep) {
	char **argv = (char **) malloc(((strlen(string) /2 ) + 2) * sizeof (char *)),
		 *token = strtok(string, sep);
	unsigned int i = 0;

	while (token) {
		unsigned int len = strlen(token);
		argv[i] = (char *) malloc(len + 1);
		strcpy(argv[i], token);
		token = strtok(NULL, sep);
		i++;
	}
	argv[i] = NULL;
	return (char **) realloc(argv, (i + 1) * sizeof (char *));
}


/**
 * Faz chamada a funcao **parser(char *string, char *sep) passando o separados
 * 'espaco' como parametro, que por sua vez fara o parser da string e seta a flag
 * 'fg' de acordo com o comando recebido.
 */

char **get_tokens(char *cmd, int *fg) {	
	char **tokens = NULL;

	if (*cmd == '&') {
		*fg = 0;
		tokens = parser(cmd + 2, " ");
	}
	else {
		*fg = 1;
		tokens = parser(cmd, " ");
	}

	return tokens;
}

