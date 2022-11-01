#include "NDDD.h"

void print_NDDD(NDDD *nddd, int depth) {
	if (nddd == NULL) {
		return;
	}

	for (int i = 0; i < depth; ++i) {
		printf("\t");
	}
	printf("%d\n", literal_to_int(nddd->val));
	print_NDDD(nddd->child, depth + 1);
	print_NDDD(nddd->sibling, depth);
}

// broken; needs to account for shared children
void free_NDDD(NDDD *nddd) {
	if (nddd == NULL) {
		return;
	}
	free_NDDD(nddd->child);
	free_NDDD(nddd->sibling);
	free(nddd);
}
