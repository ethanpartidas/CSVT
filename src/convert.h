#ifndef CONVERT_H
#define CONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BDD.h"
#include "crossbar.h"
#include "SOP.h"
#include "NDD.h"
#include "expression.h"

// Conversion

void convert_BDD_to_SOP(char *bdd_filename, char *sop_filename);

void convert_BDD_to_crossbar(char *bdd_filename, char *crossbar_filename);

#define MAX_CROSSBAR_SIZE 256
typedef struct crossbar_workspace {
	int rows;
	int cols;
	literal **grid;
	NDD **row_pointers;
	NDD **col_pointers;
} crossbar_workspace;
crossbar_workspace *create_crossbar_workspace(NDD *ndd);
void free_crossbar_workspace(crossbar_workspace *cbws, crossbar *cb);
void convert_NDD_to_crossbar_helper(NDD *ndd, crossbar_workspace *cbws, int row, int col);
crossbar *convert_NDD_to_crossbar(NDD *ndd);

NDD *convert_SOP_to_NDD(SOP *sop);
void convert_SOP_to_crossbar(char *sop_filename, char *crossbar_filename);

NDD *convert_expression_to_NDD(expr_node *en);
void convert_expression_to_crossbar(char *expr_filename, char *crossbar_filename);

// Verification

void check_equivalence_BDD_SOP(char *bdd_filename, char *sop_filename);

void check_equivalence_BDD_crossbar(char *bdd_filename, char *crossbar_filename);

void check_equivalence_SOP_crossbar(char *sop_filename, char *crossbar_filename);

void check_equivalence_expression_crossbar(char *expr_filename, char *crossbar_filename);

#endif
