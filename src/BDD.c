#include "BDD.h"

BDD *read_BDD(char *filename) {
	FILE *bdd_file = fopen(filename, "r");

	BDD *bdd = malloc(sizeof(BDD));
	fscanf(bdd_file, "vars %d\n", &bdd->vars);
	fscanf(bdd_file, "nodes %d\n", &bdd->nodes);
	bdd->node_array = malloc((bdd->nodes + 1) * sizeof(bdd_node));
	for (int i = 1; i <= bdd->nodes; ++i) {
		fscanf(bdd_file, "%*d %d %d %d\n",
			&bdd->node_array[i].left_child,
			&bdd->node_array[i].right_child,
			&bdd->node_array[i].decision_variable);
	}

	fclose(bdd_file);

	return bdd;
}

void free_BDD(BDD *bdd) {
	free(bdd->node_array);
	free(bdd);
}

void print_BDD(BDD *bdd) {
	printf("vars %d\n", bdd->vars);
	printf("nodes %d\n", bdd->nodes);
	for (int i = 1; i <= bdd->nodes; ++i) {
		printf("%d %d %d %d\n",
			i,
			bdd->node_array[i].left_child,
			bdd->node_array[i].right_child,
			bdd->node_array[i].decision_variable);
	}
}

int evaluate_BDD(BDD *bdd, int *input) {
	int current_node = 1;
	while (bdd->node_array[current_node].left_child != -1) {
		if (input[bdd->node_array[current_node].decision_variable-1]) {
			current_node = bdd->node_array[current_node].left_child;
		} else {
			current_node = bdd->node_array[current_node].right_child;
		}
	} 
	return bdd->node_array[current_node].decision_variable;
}

void print_BDD_truth_table(BDD *bdd) {
	for (int i = 0; i < 1<<bdd->vars; ++i) {
		int *input = malloc(bdd->vars * sizeof(int));
		int i_copy = i;
		for (int j = 0; j < bdd->vars; ++j) {
			input[j] = i_copy % 2;
			i_copy /= 2;

			printf("%d ", input[j]);
		}
		printf("| %d\n", evaluate_BDD(bdd, input));
	}
}
