#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define LEXER_FILE_MODE "r"

#define NEED_MORE_STRING_FOR_TOKEN 0
#define CREATING_TOKEN 1
#define MAX_TOKEN_SIZE 256

struct vec {
    unsigned char* _mem;
    unsigned long _elems;
    unsigned long _elemsize;
    unsigned long _capelems;
    unsigned long _reserve;
};

struct vec* vec_new(unsigned long elemsize)
{
    struct vec* pvec = (struct vec*)malloc(sizeof(struct vec));
    pvec->_reserve = 10;
    pvec->_capelems = pvec->_reserve;
    pvec->_elemsize = elemsize;
    pvec->_elems = 0;
    pvec->_mem = (unsigned char*)malloc(pvec->_capelems * pvec->_elemsize);
    return pvec;
}

void vec_delete(struct vec* pvec)
{
    free(pvec->_mem);
    free(pvec);
}

void vec_grow(struct vec* pvec)
{
    unsigned char* mem = (unsigned char*)malloc((pvec->_capelems + pvec->_reserve) * pvec->_elemsize);
    memcpy(mem, pvec->_mem, pvec->_elems * pvec->_elemsize);
    free(pvec->_mem);
    pvec->_mem = mem;
    pvec->_capelems += pvec->_reserve;
}

void vec_push_back(struct vec* pvec, void* data, unsigned long elemsize)
{
    assert(elemsize == pvec->_elemsize);
    if (pvec->_elems == pvec->_capelems) {
        vec_grow(pvec);
    }
    memcpy(pvec->_mem + (pvec->_elems * pvec->_elemsize), (unsigned char*)data, pvec->_elemsize);
    pvec->_elems++;    
}

unsigned long vec_length(struct vec* pvec)
{
    return pvec->_elems;
}

void* vec_get(struct vec* pvec, unsigned long index)
{
    assert(index < pvec->_elems);
    return (void*)(pvec->_mem + (index * pvec->_elemsize));
}

void vec_copy_item(struct vec* pvec, void* dest, unsigned long index)
{
    memcpy(dest, vec_get(pvec, index), pvec->_elemsize);
}

enum TokenType {
    INTEGER, ID, IF, END_EXPRESSION, LPAR, RPAR, EQUAL, DOUBLE_EQUAL, ADD, SUBTRACT, MULTIPLE, DIVIDE
};

struct Token {
    enum TokenType type;
    const char* in_source_code_string;
    unsigned int token_line;
    unsigned int token_pos;
    int code_id;
};

struct Lexer {
    FILE* source_file;
    char current_possible_token[MAX_TOKEN_SIZE];
    struct vec* tokens;
};

bool
AssertLexer( struct Lexer* lexer ){
    return (lexer != NULL);
}

int
IsCharDigit ( char character ) {
    for( char i = '0'; i < '9'; i++) {
        if(character == i) {
            return ( int ) i;
        }
    }
    return -1;
}

int StringCompare ( const char *ptr0, const char *ptr1, int len ) {
  int fast = len/sizeof(size_t) + 1; 
  int offset = (fast-1)*sizeof(size_t);
  int current_block = 0;

  if( len <= sizeof(size_t)){ fast = 0; }


  size_t *lptr0 = (size_t*)ptr0;
  size_t *lptr1 = (size_t*)ptr1;

  while( current_block < fast ){
    if( (lptr0[current_block] ^ lptr1[current_block] )){
      int pos;
      for(pos = current_block*sizeof(size_t); pos < len ; ++pos ){
        if( (ptr0[pos] ^ ptr1[pos]) || (ptr0[pos] == 0) || (ptr1[pos] == 0) ){
          return  (int)((unsigned char)ptr0[pos] - (unsigned char)ptr1[pos]);
          }
        }
      }

    ++current_block;
    }

  while( len > offset ){
    if( (ptr0[offset] ^ ptr1[offset] )){ 
      return (int)((unsigned char)ptr0[offset] - (unsigned char)ptr1[offset]); 
      }
    ++offset;
    }
	
	
  return 0;
  }

struct Lexer*
CreateLexer( const char* source_file_path ) {
    struct Lexer* lexer;
    lexer = malloc( sizeof( struct Lexer ) );
    if ( lexer == NULL ) {
        exit( EXIT_FAILURE );
    }

    memset( lexer, 0, sizeof( struct Lexer ) );
    if (source_file_path == NULL ) {
        exit( EXIT_FAILURE );
    }

    lexer->source_file = fopen( source_file_path, LEXER_FILE_MODE );
    if (lexer->source_file == NULL) {
        exit( EXIT_FAILURE );
    }

    lexer->tokens = vec_new(sizeof(struct Token));

    return lexer;
}

struct Token* 
AddToken ( enum TokenType type, const char* in_source_token, unsigned int line_num, unsigned int token_pos ) {
    struct Token* token;
    static int code_token_id_counter = 0;
    token = malloc( sizeof( struct Token ) );
    if ( token == NULL ) {
        exit( EXIT_FAILURE );
    }

    memset( token, 0, sizeof( struct Token ) );
    token->token_line = line_num;
    
    if (in_source_token != NULL) {
        token->in_source_code_string = in_source_token;
    }

    token->code_id = code_token_id_counter;
    token->token_pos = token_pos;
    code_token_id_counter++;
    token->type = type;

    return token;
}

int
MakeToken ( struct Lexer* lexer, unsigned int line_num, unsigned int token_pos ) {
    if ( AssertLexer( lexer ) ) {
        if ( lexer->current_possible_token[0] == '=' ) {
            vec_push_back( lexer->tokens, AddToken( EQUAL, lexer->current_possible_token, line_num, token_pos ) , sizeof( struct Token ));
            return CREATING_TOKEN;
        }
    }

    return NEED_MORE_STRING_FOR_TOKEN;
}

void
RunTokenize( struct Lexer* lexer ) {
    if (AssertLexer( lexer ) ) {
        int current_character = 0;
        int current_lexer_state = 0;
        unsigned int num_lines = 0;
        unsigned int pos = 0;

        int token_size = NEED_MORE_STRING_FOR_TOKEN;

        while( ( current_character = fgetc( lexer->source_file ) )  != EOF ) {
            if ( current_character == '\n' ) {
                num_lines++;
                pos = -1;
            }  

            switch ( current_lexer_state ) {
                case NEED_MORE_STRING_FOR_TOKEN: {
                    if ( current_character != ' ' && current_character != '\n' && token_size < MAX_TOKEN_SIZE ) {
                        lexer->current_possible_token[token_size] = ( char ) current_character;
                        token_size++;
                    }
                }
                break;
                case CREATING_TOKEN: {
                    for (int i = 0; i < token_size; i++) {
                        lexer->current_possible_token[i] = ' ';
                    }
                    token_size = 0;
                }
                break;
            }

            current_lexer_state = MakeToken( lexer, num_lines, pos );
            pos++;                
        }
    }
}

void LogTokenData( struct Lexer* lexer ) {
    for (unsigned long index = 0; (int)index < (int)vec_length( lexer->tokens); index++) {
        struct Token token;
        vec_copy_item(lexer->tokens, &token, index);
        printf( "Token[%d], Type: %d, { %s } ln: %d, pos: %d\n", token.code_id, token.type, token.in_source_code_string, token.token_line, token.token_pos );
    }
}

void 
DestroyLexer( struct Lexer* lexer ) {
    free( lexer );
    vec_delete( lexer->tokens );
}

int main(int argc, char* argv[])
{
    struct Lexer* lexer;
    lexer = CreateLexer( "main.pine" );

    RunTokenize( lexer );

    LogTokenData( lexer );
    DestroyLexer( lexer );
    return 0;
}