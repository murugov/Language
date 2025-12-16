#ifndef LEXER_HPP
#define LEXER_HPP

#include "token.hpp"
#include "GetHash.hpp"
#include "HashOp.hpp"

enum lexerErr_t
{
    LEX_SUCCESS = 0,
    LEX_ERROR   = 1
};

struct lexer_t
{
    stk_t<token_t*>* tokens;
    int              cur_token;
    token_t*         peeked_token;
    char**           lines;
    int              line_count;
    int              cur_line;
    int              cur_col;
    const char*      cur_pos;
    const char*      file_name;
};

struct keyword_t
{
    type_t      type;
    const char* name;
    int         len;
    hash_t      hash;
};

char **DataReader(FILE *SourceFile, char *buffer, int *count_line);

lexerErr_t LexerCtor(lexer_t *lexer, char **lines, int line_count, const char *file_name);
lexerErr_t LexerInit(lexer_t* lexer, char** lines, int line_count, const char* file_name);
lexerErr_t LexerDtor(lexer_t* lexer);

lexerErr_t AdvanceToken(lexer_t* lexer);
lexerErr_t SkipSpaces(lexer_t* lexer);
token_t *NextToken(lexer_t* lexer);
token_t *PeekToken(lexer_t* lexer);

int CmpHashForBinSearch(const void *a, const void *b);


#endif