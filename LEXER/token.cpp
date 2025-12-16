#include "token.hpp"


token_t *NewToken(type_t type, hash_t hash, const char* start, int length, int line, int col)
{
    token_t *new_token = (token_t*)calloc(1, sizeof(token_t));
    if (IS_BAD_PTR(new_token)) return NULL;

    new_token->type   = type;
    new_token->hash   = hash;
    new_token->start  = start;
    new_token->length = length;
    new_token->line   = line;
    new_token->col    = col;

    return new_token;
}


tokenErr_t FreeToken(token_t *token)
{
    ON_DEBUG( if (IS_BAD_PTR(token)) return TOKEN_ERROR; )

    token->type   = ARG_NUM;
    token->hash   = 0;
    token->start  = NULL;
    token->length = 0;
    token->line   = 0;
    token->col    = 0;  
    free(token);  

    return TOKEN_SUCCESS;
}