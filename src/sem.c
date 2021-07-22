#include "../include/sem.h"
#include "../include/err.h"

static enum TokenType current_id_type = T_NONE;

void begin_sem() {
	current_id_type = T_NONE;
}

void validate_ast(struct ASTNode* root) {
	if (!root) return;

	if (root->op == T_EQUAL) {
		current_id_type = root->left->type;
	}

	validate_ast(root->left);
	validate_ast(root->right);

	if (root->type != current_id_type) {
		if (check_if_conversion_possible(root->type, current_id_type)) {
			root->type = current_id_type;
		}
		else {
			printf("Failed to convert type '%d' to '%d'.\n", root->type, current_id_type);
			exit(EXIT_FAILURE);
		}
	}
}

bool check_if_conversion_possible(enum TokenType current, enum TokenType new_type) {
	if (current == T_INTEGER && new_type == T_INTEGER)
		return true;
	else if (current == T_INTEGER && new_type == T_CHAR || current == T_CHAR && new_type == T_INTEGER)
		return true;
	else if (current == T_CHAR && new_type == T_CHAR)
		return true;
	else if (current == T_NONE)
		return true;
	return false;
}