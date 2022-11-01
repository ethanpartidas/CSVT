#include "crossbar.h"

void free_crossbar(crossbar *cb) {
	for (int i = 0; i < cb->rows; ++i) {
		free(cb->grid[i]);
	}
	free(cb->grid);
	free(cb);
}

void save_crossbar(crossbar *cb, char *filename) {
	FILE *save_file = fopen(filename, "w");

	fprintf(save_file, "vars %d\n", cb->vars);
	fprintf(save_file, "rows %d\n", cb->rows);
	fprintf(save_file, "cols %d\n", cb->cols);
	for (int i = 0; i < cb->rows; ++i) {
		for (int j = 0; j < cb->cols; ++j) {
			fprintf(save_file, "%d", literal_to_int(cb->grid[i][j]));
			if (j != cb->cols-1) {
				fprintf(save_file, "\t");
			}
		}
		fprintf(save_file, "\n");
	}

	fclose(save_file);
}

crossbar *read_crossbar(char *filename) {
	FILE *crossbar_file = fopen(filename, "r");

	crossbar *cb = malloc(sizeof(crossbar));
	fscanf(crossbar_file, "vars %d\n", &cb->vars);
	fscanf(crossbar_file, "rows %d\n", &cb->rows);
	fscanf(crossbar_file, "cols %d\n", &cb->cols);
	cb->grid = malloc(cb->rows * sizeof(literal *));
	for (int i = 0; i < cb->rows; ++i) {
		cb->grid[i] = malloc(cb->cols * sizeof(literal));
		for (int j = 0; j < cb->cols; ++j) {
			int n;
			fscanf(crossbar_file, "%d", &n);
			if (n == ONE) {
				cb->grid[i][j] = ONE_l;
			} else if (n > 0){
				cb->grid[i][j] = (literal) {n, 1}; 
			} else {
				cb->grid[i][j] = (literal) {-n, 0};
			}
		}
	}

	fclose(crossbar_file);

	return cb;
}

void print_crossbar(crossbar *cb) {
	printf("vars %d\n", cb->vars);
	printf("rows %d\n", cb->rows);
	printf("cols %d\n", cb->cols);
	for (int i = 0; i < cb->rows; ++i) {
		for (int j = 0; j < cb->cols; ++j) {
			printf("%d", cb->grid[i][j]);
			if (j != cb->cols-1) {
				printf("\t");
			}
		}
		printf("\n");
	}
}

void free_edge(edge *e) {
	if (e == NULL) {
		return;
	}
	if (e->next != NULL) {
		free_edge(e->next);
	}
	free(e);
}

void free_graph(graph *g) {
	for (int i = 0; i < g->nodes; ++i) {
		free_edge(g->edges[i]);
	}
	free(g->edges);
	free(g);
}

void print_graph(graph *g) {
	void print_list(edge *e) {
		if (e == NULL) {
			return;
		}
		printf("%d %d -> ", e->val.positive ? e->val.val : -e->val.val, e->destination);
		print_list(e->next);
	}
	for (int node = 0; node < g->nodes; ++node) {
		print_list(g->edges[node]);
		printf("\n");
	}
}

graph *convert_crossbar_to_graph(crossbar *cb) {
	graph *g = malloc(sizeof(graph));
	g->nodes = cb->rows + cb->cols;
	g->start_node = cb->rows-1;
	g->edges = calloc(g->nodes, sizeof(edge *));
	for (int row = 0; row < cb->rows; ++row) {
		for (int col = 0; col < cb->cols; ++col) {
			literal var = cb->grid[row][col];
			if (!is_zero_l(var)) {
				int row_node = row;
				int col_node = cb->rows + col;

				edge *e1 = malloc(sizeof(edge));
				e1->val = var;
				e1->destination = col_node;
				e1->next = g->edges[row_node];
				g->edges[row_node] = e1;

				edge *e2 = malloc(sizeof(edge));
				e2->val = var;
				e2->destination = row_node;
				e2->next = g->edges[col_node];
				g->edges[col_node] = e2;
			}
		}
	}
	return g;
}

int evaluate_graph(graph *g, int *input) {
	int visited[g->nodes];
	memset(visited, 0, g->nodes * sizeof(int));

	int queue[g->nodes];
	int fptr = 0, bptr = 0;
	queue[bptr++] = g->start_node;
	visited[g->start_node] = 1;

	while (fptr < bptr) {
		int node = queue[fptr++];
		for (edge *e = g->edges[node]; e != NULL; e = e->next) {
			literal var = e->val;
			if (var.val == 0 && var.positive || var.positive && input[var.val-1] || !var.positive && !input[var.val-1]) {
				if (e->destination == 0) {
					return 1;
				}
				if (!visited[e->destination]) {
					queue[bptr++] = e->destination;
					visited[e->destination] = 1;
				}
			}
		}
	}
	return 0;
}

int evaluate_crossbar(crossbar *cb, int *input) {
	graph *g = convert_crossbar_to_graph(cb);
	return evaluate_graph(g, input);
}

void print_crossbar_truth_table(crossbar *cb) {
	graph *g = convert_crossbar_to_graph(cb);
	// print_graph(g);
	for (int i = 0; i < 1<<cb->vars; ++i) {
		int *input = malloc(cb->vars * sizeof(int));
		int i_copy = i;
		for (int j = 0; j < cb->vars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;

			printf("%d ", input[j]);
		}
		printf("| %d\n", evaluate_graph(g, input));
	}
}
