#ifndef CROSSBAR_H
#define CROSSBAR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "literal.h"

typedef struct crossbar {
	int vars;
	int rows;
	int cols;
	literal **grid;
} crossbar;

void free_crossbar(crossbar *cb);

void save_crossbar(crossbar *cb, char *filename);

crossbar *read_crossbar(char *filename);

void print_crossbar(crossbar *cb);

typedef struct edge {
	literal val;
	int destination;
	struct edge *next;
} edge;

void free_edge(edge *e);

typedef struct graph {
	int nodes;
	int start_node;
	// end node is always 0
	edge **edges;
} graph;

void free_graph(graph *g);

void print_graph(graph *g);

graph *convert_crossbar_to_graph(crossbar *cb);

int evaluate_graph(graph *g, int *input);

int evaluate_crossbar(crossbar *cb, int *input);

void print_crossbar_truth_table(crossbar *cb);

#endif
