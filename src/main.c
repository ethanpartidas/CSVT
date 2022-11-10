#include <stdio.h>
#include <string.h>
#include "convert.h"

enum filetype{None, Bdd, Sop, Expr, Xbar};

int convert_ext_to_filetype(char *ext) {
	if (strcmp(ext, ".bdd") == 0) {
		return Bdd;
	}
	if (strcmp(ext, ".sop") == 0) {
		return Sop;
	}
	if (strcmp(ext, ".expr") == 0) {
		return Expr;
	}
	if (strcmp(ext, ".xbar") == 0) {
		return Xbar;
	}
	return None;
}

int main(int argc, char **argv) {
	if (argc < 3) {
		printf("Not enough arguments\n");
		return 0;
	}
	char *ext1 = strrchr(argv[1], '.');
	char *ext2 = strrchr(argv[2], '.');
	if (ext1 == NULL || ext2 == NULL) {
		printf("Missing file extension\n");
		return 0;
	}

	int filetype1 = convert_ext_to_filetype(ext1);
	int filetype2 = convert_ext_to_filetype(ext2);
	if (filetype1 == None || filetype2 == None) {
		printf("Incompatible file extension\n");
		return 0;
	}
	if (argc == 3) {
		if (filetype1 == Bdd && filetype2 == Sop) {
			convert_BDD_to_SOP(argv[1], argv[2]);
		} else if (filetype1 == Bdd && filetype2 == Xbar) {
			convert_BDD_to_crossbar(argv[1], argv[2]);
		} else if (filetype1 == Sop && filetype2 == Xbar) {
			convert_SOP_to_crossbar(argv[1], argv[2]);
		} else if (filetype1 == Expr && filetype2 == Xbar) {
			convert_expression_to_crossbar(argv[1], argv[2]);
		} else {
			printf("Unsupported conversion\n");
		}
	} else if (argc == 4) {
		if (strcmp(argv[3], "-v") == 0) {
			if (filetype1 == Bdd && filetype2 == Xbar) {
				check_equivalence_BDD_crossbar(argv[1], argv[2]);
			}else if (filetype1 == Sop && filetype2 == Xbar) {
				check_equivalence_SOP_crossbar(argv[1], argv[2]);
			}else if (filetype1 == Expr && filetype2 == Xbar) {
				check_equivalence_expression_crossbar(argv[1], argv[2]);
			} else {
				printf("Unsupported verification\n");
			}
		} else {
			printf("Invalid argument\n");
			return 0;
		}
	} else {
		printf("Too many arguments\n");
	}

	return 0;
}