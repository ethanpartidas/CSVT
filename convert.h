#ifndef CONVERT_H
#define CONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BDD.h"
#include "crossbar.h"
#include "SOP.h"
#include "NDDD.h"
#include "expression.h"

void convert_BDD_to_crossbar(char *bdd_filename, char *crossbar_filename);

void check_equivalence(char *bdd_filename, char *crossbar_filename);

NDDD *factor_SOP(SOP *sop);

#define MAX_CROSSBAR_SIZE 256

typedef struct crossbar_workspace {
	int rows;
	int cols;
	literal **grid;
	NDDD **row_pointers;
	NDDD **col_pointers;
} crossbar_workspace;

crossbar_workspace *create_crossbar_workspace(NDDD *nddd);

void free_crossbar_workspace(crossbar_workspace *cbws, crossbar *cb);

void convert_NDDD_to_crossbar_helper(NDDD *nddd, crossbar_workspace *cbws, int row, int col);

crossbar *convert_NDDD_to_crossbar(NDDD *nddd);

void convert_SOP_to_crossbar(char *sop_filename, char *crossbar_filename);

NDDD *convert_expression_to_NDDD(expr_node *en);

void convert_expression_to_crossbar(char *expr_filename, char *crossbar_filename);

#endif
