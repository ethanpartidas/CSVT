#ifndef NDD_H
#define NDD_H

#include <stdio.h>
#include <stdlib.h>
#include "literal.h"

typedef struct NDD {
	int nvars;
	literal val;
	struct NDD *child;
	struct NDD *sibling;
} NDD;

void print_NDD(NDD *ndd, int depth);

void free_NDD(NDD *ndd);

#endif
