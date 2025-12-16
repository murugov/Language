#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "stack.hpp"
#include "tree.hpp"
#include "TXTreader.hpp"

enum tokenErr_t
{
    TOKEN_SUCCESS = 0,
    TOKEN_ERROR   = 1
};

struct token_t
{
    type_t      type;
    hash_t      hash;
    const char* start;
    int         length;
    int         line;
    int         col;
};

token_t *NewToken(type_t type, hash_t hash, const char* start, int length, int line, int col);
tokenErr_t FreeToken(token_t *token);


#endif