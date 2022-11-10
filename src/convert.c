#include "convert.h"

// The idea behind this algorithm is to create a SOP for each node, and propagate the functions down
// the BDD. The BDD is assumed to be topologically sorted.
void convert_BDD_to_SOP(char *bdd_filename, char *sop_filename) {
	BDD *bdd = read_BDD(bdd_filename);
	SOP_list *sops[bdd->nodes+1];
	memset(sops, 0, (bdd->nodes+1) * sizeof(SOP_list *));
	sops[1] = malloc(sizeof(SOP_list));
	sops[1]->nvars = bdd->vars;
	sops[1]->term = calloc(bdd->nodes, sizeof(int));
	sops[1]->next = NULL;

	for (int node = 1; node <= bdd->nodes; ++node) {
		int l = bdd->node_array[node].left_child;
		int r = bdd->node_array[node].right_child;
		int var = bdd->node_array[node].decision_variable;
		if (l != -1 && bdd->node_array[l].decision_variable != 0) {
			SOP_list *temp = copy_SOP_list(sops[node]);
			multiply_SOP_list(temp, var);
			add_SOP_list(temp, sops[l]);
			sops[l] = temp;
		}
		if (r != -1 && bdd->node_array[r].decision_variable != 0) {
			SOP_list *temp = copy_SOP_list(sops[node]);
			multiply_SOP_list(temp, -var);
			add_SOP_list(temp, sops[r]);
			sops[r] = temp;
		}
	}

	save_SOP_list(sops[bdd->nodes], sop_filename);
	for (int node = 1; node <= bdd->nodes; ++node) {
		free_SOP_list(sops[node]);
	}
	iterated_consensus(sop_filename);
}

void check_equivalence_BDD_SOP(char *bdd_filename, char *sop_filename) {
	BDD *bdd = read_BDD(bdd_filename);
	SOP *sop = read_SOP(sop_filename);

	if (bdd->vars != sop->nvars) {
		printf("Number of variables do not match.\n");
		free_BDD(bdd);
		free_SOP(sop);
		return;
	}

	int input[bdd->vars];
	memset(input, 0, bdd->vars * sizeof(int));

	for (int i = 0; i < 1<<bdd->vars; ++i) {
		int i_copy = i;
		for (int j = 0; j < bdd->vars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;
		}
		if (evaluate_BDD(bdd, input) != evaluate_SOP(sop, input)) {
			printf("The representations are not equivalent!\n");
			free_BDD(bdd);
			free_SOP(sop);
			return;
		}
	}
	printf("The representations are equivalent!\n");
	free_BDD(bdd);
	free_SOP(sop);
}

void convert_BDD_to_crossbar(char *bdd_filename, char *crossbar_filename) {
	BDD *bdd = read_BDD(bdd_filename);
	crossbar *cb = malloc(sizeof(crossbar));
	cb->vars = bdd->vars;
	cb->rows = bdd->nodes - 1;
	// number of cols is number of edges that don't go to 0
	cb->cols = 0;
	for (int i = 1; i <= bdd->nodes; ++i) {
		int l = bdd->node_array[i].left_child;
		int r = bdd->node_array[i].right_child;
		if (l != -1 && bdd->node_array[l].decision_variable != 0) {
			++cb->cols;
		}
		if (r != -1 && bdd->node_array[r].decision_variable != 0) {
			++cb->cols;
		}
	}
	cb->grid = malloc(cb->rows * sizeof(literal *));
	for (int row = 0; row < cb->rows; ++row) {
		cb->grid[row] = calloc(cb->cols, sizeof(literal));
	}
	int current_col = 0;
	for (int i = 1; i <= bdd->nodes-2; ++i) {
		int l = bdd->node_array[i].left_child;
		int r = bdd->node_array[i].right_child;
		int v = bdd->node_array[i].decision_variable;
		int positive = 1;
		if (v < 0) {
			positive = 0;
			v = -v;
		}
		if (l != -1 && bdd->node_array[l].decision_variable != 0) {
			cb->grid[cb->rows-i][current_col] = (literal) {v, positive};
			if (l == bdd->nodes) --l;
			cb->grid[cb->rows-l][current_col++] = ONE_l;
		}
		if (r != -1 && bdd->node_array[r].decision_variable != 0) {
			cb->grid[cb->rows-i][current_col] = (literal) {v, !positive};
			if (r == bdd->nodes) --r;
			cb->grid[cb->rows-r][current_col++] = ONE_l;
		}
	}
	save_crossbar(cb, crossbar_filename);
	free_crossbar(cb);
	free_BDD(bdd);
}

void check_equivalence_BDD_crossbar(char *bdd_filename, char *crossbar_filename) {
	BDD *bdd = read_BDD(bdd_filename);
	crossbar *cb = read_crossbar(crossbar_filename);

	if (bdd->vars != cb->vars) {
		printf("Number of variables do not match.\n");
		free_BDD(bdd);
		free_crossbar(cb);
		return;
	}

	graph *g = convert_crossbar_to_graph(cb);
	free_crossbar(cb);

	int input[bdd->vars];
	memset(input, 0, bdd->vars * sizeof(int));

	for (int i = 0; i < 1<<bdd->vars; ++i) {
		int i_copy = i;
		for (int j = 0; j < bdd->vars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;
		}
		if (evaluate_BDD(bdd, input) != evaluate_graph(g, input)) {
			printf("The representations are not equivalent!\n");
			free_BDD(bdd);
			free_graph(g);
			return;
		}
	}
	printf("The representations are equivalent!\n");
	free_BDD(bdd);
	free_graph(g);
}

// the basic principle: find the most common literal, and split the problem into 2 smaller SOPs
// ex: SOP -> a(SOP1) + SOP2. SOP1 is a's child and SOP2 is its sibling.
NDDD *convert_SOP_to_NDDD(SOP *sop) {
	// base case: empty SOP returns a null pointer
	if (sop->nterms == 0) {
		return NULL;
	}

	// base case: tautology returns a null pointer
	int count = 0;
	for (int var = 0; var < sop->nvars; ++var) {
		if (sop->terms[0][var] != 0) {
			++count;
			break;
		}
	}
	if (count == 0) {
		return NULL;
	}

	// 2 rows to store positive and negative literal counts
	int literal_counts[sop->nvars][2];
	memset(literal_counts, 0, sop->nvars * 2 * sizeof(int));
	for (int term = 0; term < sop->nterms; ++term) {
		for (int var = 0; var < sop->nvars; ++var) {
			if (sop->terms[term][var] == 1) {
				++literal_counts[var][1];
			}
			if (sop->terms[term][var] == -1) {
				++literal_counts[var][0];
			}
		}
	}
	
	// find most common literal
	int max_var = 0;
	int max_i = 0;
	int max_count = 0;
	for (int var = 0; var < sop->nvars; ++var) {
		for (int i = 0; i < 2; ++i) {
			if (literal_counts[var][i] > max_count) {
				max_var = var;
				max_i = i;
				max_count = literal_counts[var][i];
			}
		}
	}
	
	// split SOP into SOP1 and SOP2
	SOP *sop1 = malloc(sizeof(SOP));
	SOP *sop2 = malloc(sizeof(SOP));
	sop1->nvars = sop->nvars;
	sop2->nvars = sop->nvars;
	sop1->nterms = max_count;
	sop2->nterms = sop->nterms - max_count;
	sop1->terms = malloc(sop1->nterms * sizeof(int *));
	sop2->terms = malloc(sop2->nterms * sizeof(int *));
	int sop1_current_term = 0;
	int sop2_current_term = 0;
	for (int term = 0; term < sop->nterms; ++term) {
		if (sop->terms[term][max_var] == (max_i ? 1 : -1)) {
			sop1->terms[sop1_current_term] = sop->terms[term];
			sop1->terms[sop1_current_term++][max_var] = 0;
		} else {
			sop2->terms[sop2_current_term++] = sop->terms[term];
		}
	}

	// final recursive step
	NDDD *nddd = malloc(sizeof(NDDD));
	nddd->nvars = sop->nvars;
	nddd->val = (literal) {max_var + 1, max_i};
	nddd->child = convert_SOP_to_NDDD(sop1);
	nddd->sibling = convert_SOP_to_NDDD(sop2);

	free_SOP_clone(sop1);
	free_SOP_clone(sop2);

	return nddd;
}

crossbar_workspace *create_crossbar_workspace(NDDD *nddd) {
	crossbar_workspace *cbws = malloc(sizeof(crossbar_workspace));	
	// inital rows: input and output
	cbws->rows = 2;
	cbws->cols = 0;
	cbws->grid = malloc(MAX_CROSSBAR_SIZE * sizeof(literal *));
	for (int i = 0; i < MAX_CROSSBAR_SIZE; ++i) {
		cbws->grid[i] = calloc(MAX_CROSSBAR_SIZE, sizeof(literal));
	}
	cbws->row_pointers = malloc(MAX_CROSSBAR_SIZE * sizeof(NDDD *));
	cbws->col_pointers = malloc(MAX_CROSSBAR_SIZE * sizeof(NDDD *));
	cbws->row_pointers[0] = nddd;
	cbws->row_pointers[1] = NULL;
	return cbws;
}

void free_crossbar_workspace(crossbar_workspace *cbws, crossbar *cb) {
	// we will leave some rows for the subsequent crossbar
	for (int row = cbws->rows; row < MAX_CROSSBAR_SIZE; ++row) {
		free(cbws->grid[row]);
	}
	// resize leftover rows
	for (int row = 0; row < cb->rows; ++row) {
		cb->grid[row] = realloc(cb->grid[row], cb->cols * sizeof(literal));
	}
	free(cbws->row_pointers);
	free(cbws->col_pointers);
	free(cbws);
}

void convert_NDDD_to_crossbar_helper(NDDD *nddd, crossbar_workspace *cbws, int row, int col) {
	// base case: we've reached the output
	if (nddd == NULL) {
		return;
	}

	// Two main scenarios: we are on a row or column. For now, the code will be copy-pasted for the
	// second half
	if (col == -1) {
		// search columns for child node pointer and empty intersection
		for (int c = 0; c < cbws->cols; ++c) {
			if (cbws->col_pointers[c] == nddd->child && is_zero_l(cbws->grid[row][c])) {
				cbws->grid[row][c] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, row, -1);
				return;
			}
		}

		// search rows for child node pointer and merge that row with a new column
		for (int r = 0; r < cbws->rows; ++r) {
			if (cbws->row_pointers[r] == nddd->child) {
				cbws->col_pointers[cbws->cols] = nddd->child;
				cbws->grid[r][cbws->cols] = ONE_l;
				cbws->grid[row][cbws->cols++] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, row, -1);
				return;
			}
		}

		// search columns for child node pointer and merge that column with a new column
		for (int c = 0; c < cbws->cols; ++c) {
			if (cbws->col_pointers[c] == nddd->child) {
				cbws->col_pointers[cbws->cols] = nddd->child;
				cbws->row_pointers[cbws->rows] = nddd->child;
				cbws->grid[cbws->rows][cbws->cols] = ONE_l;
				cbws->grid[cbws->rows++][c] = ONE_l;
				cbws->grid[row][cbws->cols++] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, row, -1);
				return;
			}
		}

		// child hasn't been implemented yet; make it a new column
		cbws->col_pointers[cbws->cols] = nddd->child;
		cbws->grid[row][cbws->cols] = nddd->val;
		convert_NDDD_to_crossbar_helper(nddd->child, cbws, -1, cbws->cols++);
		convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, row, -1);
	} else {
		// search rows for child node pointer and empty intersection
		for (int r = 0; r < cbws->rows; ++r) {
			if (cbws->row_pointers[r] == nddd->child && is_zero_l(cbws->grid[r][col])) {
				cbws->grid[r][col] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, -1, col);
				return;
			}
		}

		// search cols for child node pointer and merge that column with a new row
		for (int c = 0; c < cbws->cols; ++c) {
			if (cbws->col_pointers[c] == nddd->child) {
				cbws->row_pointers[cbws->rows] = nddd->child;
				cbws->grid[cbws->rows][c] = ONE_l;
				cbws->grid[cbws->rows++][col] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, -1, col);
				return;
			}
		}

		// search rows for child node pointer and merge that row with a new row
		for (int r = 0; r < cbws->rows; ++r) {
			if (cbws->row_pointers[r] == nddd->child) {
				cbws->row_pointers[cbws->rows] = nddd->child;
				cbws->col_pointers[cbws->cols] = nddd->child;
				cbws->grid[cbws->rows][cbws->cols] = ONE_l;
				cbws->grid[r][cbws->cols++] = ONE_l;
				cbws->grid[cbws->rows++][col] = nddd->val;
				convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, -1, col);
				return;
			}
		}

		// child hasn't been implemented yet; make it a new row
		cbws->row_pointers[cbws->rows] = nddd->child;
		cbws->grid[cbws->rows][col] = nddd->val;
		convert_NDDD_to_crossbar_helper(nddd->child, cbws, cbws->rows++, -1);
		convert_NDDD_to_crossbar_helper(nddd->sibling, cbws, -1, col);
	}
}

crossbar *convert_NDDD_to_crossbar(NDDD *nddd) {
	crossbar_workspace *cbws = create_crossbar_workspace(nddd);
	convert_NDDD_to_crossbar_helper(nddd, cbws, 0, -1);
	crossbar *cb = malloc(sizeof(crossbar));
	cb->vars = nddd->nvars;
	cb->rows = cbws->rows;
	cb->cols = cbws->cols;
	cb->grid = cbws->grid;
	free_crossbar_workspace(cbws, cb);

	// reorder rows
	for (int row = 1; row < (cb->rows+1)/2; ++row) {
		literal *temp = cb->grid[row];
		cb->grid[row] = cb->grid[cb->rows-row];
		cb->grid[cb->rows-row] = temp;
	}
	literal *temp = cb->grid[0];
	cb->grid[0] = cb->grid[cb->rows-1];
	cb->grid[cb->rows-1] = temp;

	return cb;
}

void convert_SOP_to_crossbar(char *sop_filename, char *crossbar_filename) {
	SOP *sop = read_SOP(sop_filename);
	NDDD *nddd = convert_SOP_to_NDDD(sop);
	free_SOP(sop);
	crossbar *cb = convert_NDDD_to_crossbar(nddd);
	free_NDDD(nddd);
	save_crossbar(cb, crossbar_filename);
	free_crossbar(cb);
}

NDDD *convert_expression_to_NDDD(expr_node *en) {
	if (en->op == 0) {
		NDDD *nddd = calloc(1, sizeof(NDDD));
		nddd->nvars = en->nvars;
		nddd->val = en->val;
		return nddd;
	}
	
	NDDD *left = convert_expression_to_NDDD(en->left_child);
	NDDD *right = convert_expression_to_NDDD(en->right_child);
	
	if (en->op == '+') {
		NDDD *tail = left;
		while (tail->sibling != NULL) {
			tail = tail->sibling;
		}
		tail->sibling = right;
		return left;
	}

	// inefficient; visited hashmap will prevent exponential runtime
	void dfs(NDDD *nddd) {
		if (nddd == NULL || nddd == right) {
			return;
		}
		if (nddd->child == NULL) {
			nddd->child = right;
		} else {
			dfs(nddd->child);
		}
		dfs(nddd->sibling);
	}
	dfs(left);

	return left;
}

void convert_expression_to_crossbar(char *expr_filename, char *crossbar_filename) {
	expr_node *en = read_expression(expr_filename);
	// print_expression(en);
	NDDD *nddd = convert_expression_to_NDDD(en);
	free_expression(en);
	crossbar *cb = convert_NDDD_to_crossbar(nddd);
	save_crossbar(cb, crossbar_filename);
	free_crossbar(cb);
}

void check_equivalence_expression_crossbar(char *expr_filename, char *crossbar_filename) {
	expr_node *en = read_expression(expr_filename);
	crossbar *cb = read_crossbar(crossbar_filename);

	if (en->nvars != cb->vars) {
		printf("Number of variables do not match.\n");
		free_expression(en);
		free_crossbar(cb);
		return;
	}

	graph *g = convert_crossbar_to_graph(cb);
	free_crossbar(cb);

	int input[en->nvars];
	memset(input, 0, en->nvars * sizeof(int));

	for (int i = 0; i < 1<<en->nvars; ++i) {
		int i_copy = i;
		for (int j = 0; j < en->nvars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;
		}
		if (evaluate_expression(en, input) != evaluate_graph(g, input)) {
			printf("The representations are not equivalent!\n");
			free_expression(en);
			free_graph(g);
			return;
		}
	}
	printf("The representations are equivalent!\n");
	free_expression(en);
	free_graph(g);
}

void check_equivalence_SOP_crossbar(char *sop_filename, char *crossbar_filename) {
	SOP *sop = read_SOP(sop_filename);
	crossbar *cb = read_crossbar(crossbar_filename);

	if (sop->nvars != cb->vars) {
		printf("Number of variables do not match.\n");
		free_SOP(sop);
		free_crossbar(cb);
		return;
	}

	graph *g = convert_crossbar_to_graph(cb);
	free_crossbar(cb);

	int input[sop->nvars];
	memset(input, 0, sop->nvars * sizeof(int));

	for (int i = 0; i < 1<<sop->nvars; ++i) {
		int i_copy = i;
		for (int j = 0; j < sop->nvars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;
		}
		if (evaluate_SOP(sop, input) != evaluate_graph(g, input)) {
			printf("The representations are not equivalent!\n");
			free_SOP(sop);
			free_graph(g);
			return;
		}
	}
	printf("The representations are equivalent!\n");
	free_SOP(sop);
	free_graph(g);
}