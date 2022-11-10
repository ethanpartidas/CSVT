#ifndef SOP_H
#define SOP_H

#include <stdio.h>
#include <stdlib.h>

typedef struct SOP {
	int nvars;
	int nterms;
	int **terms;
} SOP;

SOP *read_SOP(char *sop_filename);

void free_SOP(SOP *sop);

void free_SOP_clone(SOP *sop);

void print_SOP(SOP *sop);

int evaluate_SOP(SOP *sop, int *input);

void save_SOP(SOP *sop, char *sop_filename);

typedef struct SOP_list {
	int nvars;
	int *term;
	struct SOP_list *next;
} SOP_list;

void free_SOP_list(SOP_list *sopl);

void save_SOP_list(SOP_list *sopl, char *sop_filename);

SOP_list *copy_SOP_list(SOP_list *sopl);

void add_SOP_list(SOP_list *sopl1, SOP_list *sopl2);

void multiply_SOP_list(SOP_list *sopl, int val);

void SCC_minimal(SOP *sop);
void iterated_consensus(char *sop_filename);

#endif