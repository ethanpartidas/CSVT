#ifndef BDD_H
#define BDD_H

#include <stdio.h>
#include <stdlib.h>

typedef struct bdd_node {
	int left_child;
	int right_child;
	int decision_variable;
} bdd_node;

typedef struct BDD {
	int vars;
	int nodes;
	bdd_node *node_array;
} BDD;

BDD *read_BDD(char *filename);

void free_BDD(BDD *bdd);

void print_BDD(BDD *bdd);

int evaluate_BDD(BDD *bdd, int *input);

void print_BDD_truth_table(BDD *bdd);

#endif
