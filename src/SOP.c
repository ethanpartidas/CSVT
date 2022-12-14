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

void save_SOP(SOP *sop, char *sop_filename) {
	FILE *sop_file = fopen(sop_filename, "w");

	fprintf(sop_file, "vars %d\n", sop->nvars);
	fprintf(sop_file, "terms %d\n", sop->nterms);
	for (int term = 0; term < sop->nterms; ++term) {
		for (int var = 0; var < sop->nvars; ++var) {
			int val = sop->terms[term][var];
			if (val != 0) {
				fprintf(sop_file, "%d ", val * (var + 1));
			}
		}
		fprintf(sop_file, "0\n");
	}

	fclose(sop_file);
}

void free_SOP_list(SOP_list *sopl) {
	if (sopl == NULL) {
		return;
	}
	free(sopl->term);
	free_SOP_list(sopl->next);
	free(sopl);
}

void save_SOP_list(SOP_list *sopl, char *sop_filename) {
	FILE *sop_file = fopen(sop_filename, "w");

	fprintf(sop_file, "vars %d\n", sopl->nvars);
	int nterms = 0;
	for (SOP_list *node = sopl; node != NULL; node = node->next) {
		++nterms;
	}
	fprintf(sop_file, "terms %d\n", nterms);
	for (SOP_list *node = sopl; node != NULL; node = node->next) {
		for (int var = 0; var < sopl->nvars; ++var) {
			int val = node->term[var];
			if (val != 0) {
				fprintf(sop_file, "%d ", val * (var + 1));
			}
		}
		fprintf(sop_file, "0\n");
	}

	fclose(sop_file);
}

SOP_list *copy_SOP_list(SOP_list *sopl) {
	SOP_list *copy = NULL;
	for (SOP_list *node = sopl; node != NULL; node = node->next) {
		SOP_list *temp = copy;
		copy = malloc(sizeof(SOP_list));
		copy->nvars = node->nvars;
		copy->term = malloc(node->nvars * sizeof(int));
		for (int var = 0; var < node->nvars; ++var) {
			copy->term[var] = node->term[var];
		}
		copy->next = temp;
	}
	return copy;
}

void add_SOP_list(SOP_list *sopl1, SOP_list *sopl2) {
	while (sopl1->next != NULL) {
		sopl1 = sopl1->next;
	}
	sopl1->next = sopl2;
}

void multiply_SOP_list(SOP_list *sopl, int val) {
	int var, positive;
	if (val > 0) {
		var = val-1;
		positive = 1;
	} else {
		var = -val-1;
		positive = -1;
	}
	for (SOP_list *node = sopl; node != NULL; node = node->next) {
		node->term[var] = positive;
	}
}

void SCC_minimal(SOP *sop) {
	int change;
	do {
		change = 0;
		for (int term1 = 0; term1 < sop->nterms; ++term1) {
			for (int term2 = 0; term2 < sop->nterms; ++term2) {
				if (term1 == term2) {
					continue;
				}
				int contained = 1;
				for (int var = 0; var < sop->nvars; ++var) {
					int a = sop->terms[term1][var];
					int b = sop->terms[term2][var];
					if (a == 0 && b != 0 || a * b == -1) {
						contained = 0;
						break;
					}
				}
				if (contained) {
					change = 1;
					free(sop->terms[term1]);
					sop->terms[term1] = sop->terms[--sop->nterms];
					break;
				}
			}
			if (change) {
				break;
			}
		}
	} while (change);
}

void iterated_consensus(char *sop_filename) {
	SOP *sop = read_SOP(sop_filename);
	SCC_minimal(sop);
	int change;
	do {
		change = 0;
		for (int term1 = 0; term1 < sop->nterms-1; ++term1) {
			for (int term2 = term1+1; term2 < sop->nterms; ++term2) {
				int delta = 0;
				for (int var = 0; var < sop->nvars; ++var) {
					if (sop->terms[term1][var] * sop->terms[term2][var] == -1) {
						++delta;
					}
				}
				if (delta != 1) {
					continue;
				}
				int *new_term = malloc(sop->nvars * sizeof(int));
				for (int var = 0; var < sop->nvars; ++var) {
					int a = sop->terms[term1][var];
					int b = sop->terms[term2][var];
					if (a * b == -1) {
						new_term[var] = 0;
					} else if (a == 0) {
						new_term[var] = b;
					} else {
						new_term[var] = a;
					}
				}
				int skip = 0;
				for (int term = 0; term < sop->nterms; ++term) {
					int contained = 1;
					for (int var = 0; var < sop->nvars; ++var) {
						int a = new_term[var];
						int b = sop->terms[term][var];
						if (a == 0 && b != 0 || a * b == -1) {
							contained = 0;
							break;
						}
					}
					if (contained) {
						skip = 1;
						break;
					}
				}
				if (skip) {
					continue;
				}
				change = 1;
				sop->terms = realloc(sop->terms, (sop->nterms+1) * sizeof(int *));
				sop->terms[sop->nterms++] = new_term;
				SCC_minimal(sop);
			}
		}
	} while (change);
	save_SOP(sop, sop_filename);
	free_SOP(sop);
}
