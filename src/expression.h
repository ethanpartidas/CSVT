#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <stdio.h>
#include <stdlib.h>
#include "literal.h"

typedef struct expr_node {
	int nvars;
	int op;
	literal val;
	struct expr_node *left_child;
	struct expr_node *right_child;
} expr_node;

expr_node *read_expression_helper(FILE *expr_file, int nvars);

expr_node *read_expression(char *filename);

void free_expression(expr_node *en);

void print_expression(expr_node *en);

int evaluate_expression(expr_node *en, int *input);

#endif
