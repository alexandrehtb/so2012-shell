#include "builtincmd.h"
#include "string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

int builtInCommand (char **tokens) {
	if (strcmp(tokens[0], "exit") == 0) {
		exit(EXIT_SUCCESS);
	}
	else if (strcmp(tokens[0], "cd") == 0) {
		return cd(tokens);

		return 1;
	}
	else if (strcmp(tokens[0], "help") == 0) {
		help();

		return 1;
	}

	return 0;
}
