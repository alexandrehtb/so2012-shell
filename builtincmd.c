#include "builtincmd.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int cd (char **tokens) {
	//char **tokens = parser(argv, "  ");

	if (tokens[1] == NULL) {
		chdir(getenv("HOME"));
	}
	else {
		if (chdir(tokens[1]) == -1) {
			fprintf(stderr, "cd: %s: Diretorio nao encontrado\n", tokens[1]);
		}
	}

	return 1;
}

int builtInCommand (char **tokens) {
	if (strcmp(tokens[0], "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
	else if (strcmp(tokens[0], "cd") == 0) {
		return cd(tokens);

		return 1;
	}

	return 0;
}
