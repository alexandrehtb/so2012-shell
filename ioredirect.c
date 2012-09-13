#include "ioredirect.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int redirectOutput (char **tokens, char **out) {
	int i = 0;

	while (tokens[i] != NULL) {
		if (strcmp(tokens[i], ">") == 0) {
			*out = tokens[i + 1];
			free(tokens[i]);
			tokens[i] = NULL;

			return 1;
		}
		else if (strcmp(tokens[i], ">>") == 0) {
			*out = tokens[i + 1];
			free(tokens[i]);
			tokens[i] = NULL;

			return 2;
		}
		i++;
	}
	
	return 0;
}

int redirectInput (char **tokens, char **in) {
	int i = 0;

	while (tokens[i] != NULL) {
		if (strcmp(tokens[i], "<") == 0) {
			*in = tokens[i + 1];
			free(tokens[i]);
			tokens[i] = NULL;

			return 1;
		}
		i++;
	}

	return 0;
}
