#include "expression.h"

expr_node *read_expression_helper(FILE *expr_file, int nvars) {
	expr_node *en = calloc(1, sizeof(expr_node));
	en->nvars = nvars;

	char buffer[8];
	fscanf(expr_file, "%s", buffer);
	if (buffer[0] != '+' && buffer[0] != '*') {
		int n;
		sscanf(buffer, "%d", &n);
		if (n == ONE) {
			en->val = ONE_l;
		} else if (n > 0) {
			en->val = (literal) {n, 1};
		} else {
			en->val = (literal) {-n, 0};
		}
	} else {
		en->op = buffer[0];
		en->left_child = read_expression_helper(expr_file, nvars);
		en->right_child = read_expression_helper(expr_file, nvars);
	}

	return en;
}

expr_node *read_expression(char *filename) {
	FILE *expr_file = fopen(filename, "r");

	int nvars;
	fscanf(expr_file, "vars %d\n", &nvars);
	expr_node *en = read_expression_helper(expr_file, nvars);

	fclose(expr_file);

	return en;
}

void free_expression(expr_node *en) {
	if (en == NULL) {
		return;
	}
	free_expression(en->left_child);
	free_expression(en->right_child);
	free(en);
}

void print_expression(expr_node *en) {
	if (en->op != 0) {
		printf("%c ", en->op);
		print_expression(en->left_child);
		print_expression(en->right_child);
	} else {
		printf("%d ", literal_to_int(en->val));
	}
}

int evaluate_expression(expr_node *en, int *input) {
	if (en->op == '+') {
		return evaluate_expression(en->left_child, input) || evaluate_expression(en->right_child, input);
	} else if (en->op == '*') {
		return evaluate_expression(en->left_child, input) && evaluate_expression(en->right_child, input);
	} else {
		if (en->val.val == 0) {
			return en->val.positive;
		} else {
			return input[en->val.val-1] == en->val.positive;
		}
	}
}