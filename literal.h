#ifndef LITERAL_H
#define LITERAL_H

#define ONE 99

// 0 represents constant, where positive is 1 and negative is 0
typedef struct literal {
	int val;
	int positive;
} literal;

static const literal ONE_l = (literal) {0, 1};

int is_zero_l(literal l);

int literal_to_int(literal l);

#endif
