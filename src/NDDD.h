#ifndef NDDD_H
#define NDDD_H

#include <stdio.h>
#include <stdlib.h>
#include "literal.h"

typedef struct NDDD {
	int nvars;
	literal val;
	struct NDDD *child;
	struct NDDD *sibling;
} NDDD;

void print_NDDD(NDDD *nddd, int depth);

void free_NDDD(NDDD *nddd);

#endif
