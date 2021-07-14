#include "../include/sem.h"
#include "../include/err.h"

bool check_if_conversion_possible(enum TokenType current, enum TokenType new_type) {
	return false;
}

void convert_node(struct ASTNode* node, enum TokenType new_type) {
	if (check_if_conversion_possible(node->op, new_type)) {
		node->op = new_type;
	}
	else {
		fprintf(stderr, "Failed to convert %d to %d.\n", node->op, new_type);
		exit(EXIT_FAILURE);
	}
}