#include "literal.h"

int is_zero_l(literal l) {
	return l.val == 0 && l.positive == 0;
}

int literal_to_int(literal l) {
	if (l.val == 0 && l.positive) {
		return ONE;
	}
	return (l.positive ? 1 : -1) * l.val;
}
