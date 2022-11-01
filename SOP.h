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

#endif