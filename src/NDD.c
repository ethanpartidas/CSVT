#include "NDD.h"

void print_NDD(NDD *ndd, int depth) {
	if (ndd == NULL) {
		return;
	}

	for (int i = 0; i < depth; ++i) {
		printf("\t");
	}
	printf("%d\n", literal_to_int(ndd->val));
	print_NDD(ndd->child, depth + 1);
	print_NDD(ndd->sibling, depth);
}

// broken; needs to account for shared children
void free_NDD(NDD *ndd) {
	if (ndd == NULL) {
		return;
	}
	free_NDD(ndd->child);
	free_NDD(ndd->sibling);
	free(ndd);
}
