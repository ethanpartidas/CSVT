#include "SOP.h"

SOP *read_SOP(char *sop_filename) {
	FILE *sop_file = fopen(sop_filename, "r");
	SOP *sop = malloc(sizeof(SOP));

	fscanf(sop_file, "vars %d\n", &sop->nvars);
	fscanf(sop_file, "terms %d\n", &sop->nterms);
	sop->terms = malloc(sop->nterms * sizeof(int *));
	for (int i = 0; i < sop->nterms; ++i) {
		sop->terms[i] = calloc(sop->nvars, sizeof(int));
		int literal;
		fscanf(sop_file, "%d", &literal);
		do {
			if (literal > 0) {
				sop->terms[i][literal-1] = 1;
			} else {
				sop->terms[i][-literal-1] = -1;
			}

			fscanf(sop_file, "%d", &literal);
		} while (literal != 0);
	}

	fclose(sop_file);

	return sop;
}

void free_SOP(SOP *sop) {
	for (int i = 0; i < sop->nterms; ++i) {
		free(sop->terms[i]);
	}
	free(sop->terms);
	free(sop);
}

// used for when the sop terms clone the terms of a different sop
void free_SOP_clone(SOP *sop) {
	free(sop->terms);
	free(sop);
}

void print_SOP(SOP *sop) {
	printf("vars %d\n", sop->nvars);
	printf("terms %d\n", sop->nterms);
	for (int term = 0; term < sop->nterms; ++term) {
		for (int var = 0; var < sop->nvars; ++var) {
			int val = sop->terms[term][var];
			if (val != 0) {
				printf("%d ", val * (var + 1));
			}
		}
		printf("0\n");
	}
}

int evaluate_SOP(SOP *sop, int *input) {
	for (int term = 0; term < sop->nterms; ++term) {
		int satisfied = 1;
		for (int var = 0; var < sop->nvars; ++var) {
			if (sop->terms[term][var] == 1 && input[var] == 0
			|| sop->terms[term][var] == -1 && input[var] == 1) {
				satisfied = 0;
				break;
			}
		}
		if (satisfied) {
			return 1;
		}
	}
	return 0;
}
