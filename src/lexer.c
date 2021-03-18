#include "../include/lexer.h"

bool AssertLexer(struct Lexer *lexer)
{
    return (lexer != NULL);
}

struct Lexer *CreateLexer(const char *text_input)
{
    struct Lexer *lexer;
    lexer = malloc(sizeof(struct Lexer));
    if (lexer == NULL)
    {
        exit(EXIT_FAILURE);
    }

    memset(lexer, 0, sizeof(struct Lexer));
    if (text_input == NULL)
    {
        exit(EXIT_FAILURE);
    }
    lexer->input_text = text_input;
    lexer->tokens = CreateVector(sizeof(struct Token));

    return lexer;
}

int MakeToken(struct Lexer *lexer, struct TokenInfo token_pos)
{
    if (AssertLexer(lexer))
    {
        for (size_t i = 0; i < sizeof(TOKEN_PAIRS) / sizeof(TOKEN_PAIRS[0]); i++)
        {
            if (PushNewToken(lexer, i, token_pos, TOKEN_PAIRS))
                return CREATING_TOKEN;
        }
        for (size_t i = 0; i < sizeof(TOKEN_OPERATORS) / sizeof(TOKEN_OPERATORS[0]); i++)
        {
            if (PushNewToken(lexer, i, token_pos, TOKEN_OPERATORS))
                return CREATING_TOKEN;
        }

        if (lexer->current_possible_token[0] == '=')
        {
            VectorPushBack(lexer->tokens, AddToken(EQUAL, lexer->current_possible_token, token_pos, lexer->copy_string_token));
            struct Token *token_1 = lexer->tokens->array[lexer->tokens->size - 1];
            struct Token *token_2 = lexer->tokens->array[lexer->tokens->size - 2];

            if (token_1->type == EQUAL && token_2->type == EQUAL)
            {
                if (token_1->token_info.token_pos - token_2->token_info.token_pos == 1)
                {
                    PopVector(lexer->tokens, lexer->tokens->size - 1);
                    PopVector(lexer->tokens, lexer->tokens->size - 2);
                    MoveTokenCounter(-2);
                    VectorPushBack(lexer->tokens, AddToken(DOUBLE_EQUAL, "==", token_pos, lexer->copy_string_token));
                }
            }
            return CREATING_TOKEN;
        }
        else if (IsCharDigit(lexer->current_possible_token[0]) || lexer->current_char == '.')
        {
            if (!IsCharDigit((char)lexer->next_char) && lexer->next_char != '.')
            {
                int is_float = strchr(lexer->current_possible_token, '.') != NULL;
                if (is_float)
                    VectorPushBack(lexer->tokens, AddToken(FLOAT, lexer->current_possible_token, token_pos, lexer->copy_string_token));
                else
                    VectorPushBack(lexer->tokens, AddToken(INTEGER, lexer->current_possible_token, token_pos, lexer->copy_string_token));
                return CREATING_TOKEN;
            }
        }
        else if (IsCharGoodForVariableName(lexer->current_possible_token[0]))
        {
            if (!IsCharGoodForVariableName((char)lexer->next_char))
            {
                VectorPushBack(lexer->tokens, AddToken(ID, lexer->current_possible_token, token_pos, lexer->copy_string_token));
                return CREATING_TOKEN;
            }
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

bool PushNewToken(struct Lexer *lexer, size_t i, struct TokenInfo token_pos, const struct TokenPair token_pairs[])
{
    if (StringCompare(lexer->current_possible_token, token_pairs[i].in_code_name, strlen(token_pairs[i].in_code_name)) == 0)
    {
        VectorPushBack(lexer->tokens, AddToken(token_pairs[i].type, token_pairs[i].in_code_name, token_pos, lexer->copy_string_token));
        return true;
    }
    return false;
}

void RunTokenizer(struct Lexer *lexer)
{
    int get_char = 0;
    int current_character = 0;
    int next_character = 0;
    int token_size = 0;
    bool wait_on_loop = false;
    struct TokenInfo token_position = {0, 0};
    int current_lexer_state = NEED_MORE_STRING_FOR_TOKEN;
    int counter = 0;

    while (counter <= strlen(lexer->input_text))
    {
        get_char = lexer->input_text[counter];
        counter++;

        if (!wait_on_loop)
        {
            current_character = get_char;
        }

        next_character = get_char;

        lexer->current_char = current_character;
        lexer->next_char = next_character;
        if (wait_on_loop)
        {
            switch (current_lexer_state)
            {
            case NEED_MORE_STRING_FOR_TOKEN:
            {
                if (current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE)
                {
                    lexer->current_possible_token[token_size] = (char)current_character;
                    token_size++;
                }
            }
            break;
            case CREATING_TOKEN:
            {
                for (int i = 0; i < token_size; i++)
                {
                    lexer->current_possible_token[i] = ' ';
                }

                token_size = 0;
                if (current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE)
                {
                    lexer->current_possible_token[token_size] = (char)current_character;
                    token_size++;
                }
            }
            break;
            }
        }

        current_lexer_state = MakeToken(lexer, token_position);

        current_character = next_character;
        if (current_character == '\n')
        {
            token_position.token_line++;
            token_position.token_pos = 0;
        }
        token_position.token_pos++;

        wait_on_loop = true;
    }
}

extern void CreateBufferForLexer(struct LexerLoader *loader)
{
    loader->file = fopen(loader->file_path, LEXER_FILE_MODE);
    if (loader->file != NULL)
    {
        if (fseek(loader->file, 0L, SEEK_END) == 0)
        {
            long bufsize = ftell(loader->file);
            if (bufsize == -1)
            { 
            }

            loader->buffer = malloc(sizeof(char) * (bufsize + 1));

            if (fseek(loader->file, 0L, SEEK_SET) != 0)
            { 
            }

            size_t newLen = fread(loader->buffer, sizeof(char), bufsize, loader->file);
            if (ferror(loader->file) != 0)
            {
                fputs("Error reading file", stderr);
            }
            else
            {
                loader->buffer[newLen++] = '\0'; 
            }
        }
        fclose(loader->file);
    }
}

void LogTokenData(struct Lexer *lexer)
{
    for (size_t i = 0; i < lexer->tokens->size; i++)
    {
        struct Token *token = lexer->tokens->array[i];
        printf("Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token->code_id, token->type, token->token_string, token->token_info.token_line, token->token_info.token_pos);
    }
}

void DestroyLexer(struct Lexer *lexer)
{
    FreeVector(lexer->tokens);
    free(lexer->copy_string_token);
    free(lexer);
}

void ClearLexerData(struct Lexer *lexer)
{
    lexer->input_text = "";
    lexer->current_possible_token[0] = '\0';
    ClearVector(lexer->tokens);
    lexer->next_char = ' ';
    lexer->current_char = ' ';
}