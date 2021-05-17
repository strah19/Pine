#include "../include/token.h"

static int code_token_id_counter = -1;

extern int MoveTokenCounter(int value)
{
    code_token_id_counter += value;
    return code_token_id_counter;
}

extern void ResetTokenCounter()
{
    code_token_id_counter = 0;
}

struct Token *AddToken(enum TokenType type, const char* in_source_token, struct TokenInfo token_info)
{
    struct Token *token;
    token = malloc(sizeof(struct Token));
    if (token == NULL)
    {
        exit(EXIT_FAILURE);
    }

    memset(token, 0, sizeof(struct Token));

    strcpy(token->token_string, in_source_token);
    token->token_info = token_info;

    token->code_id = MoveTokenCounter(1);
    token->type = type;

    return token;
}
