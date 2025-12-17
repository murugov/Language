#ifndef PARSER_HPP
#define PARSER_HPP

#include "tree.hpp"
#include "token.hpp"
#include "lexer.hpp"
#include "hash_table.hpp"
#include "colors.hpp"
#include "dump.hpp"


struct parser_t
{
    lexer_t*      lexer;
    stk_t<ht_t*>* name_tables;
    int           cur_name_table;
};

lexerErr_t parserCtor(parser_t *parser);
lexerErr_t parserDtor(parser_t *parser);

int MatchToken(parser_t* parser, hash_t hash);
int CheckToken(parser_t* parser, hash_t hash);
token_t* ConsumeToken(parser_t* parser, hash_t hash, const char* error_msg);
void PrintError(parser_t* parser, token_t* token, const char* message);

node_t* ParseAST(parser_t *parser);
node_t* ParseFunc(parser_t* parser);
node_t* ParseParams(parser_t *parser);
node_t* ParseStatement(parser_t *parser);
node_t* ParseExpression(parser_t* parser);
node_t* ParseTerm(parser_t* parser);
node_t* ParseFactor(parser_t* parser);
node_t* ParsePrimary(parser_t* parser);
node_t* ParseIf(parser_t* parser);
node_t* ParseWhile(parser_t* parser);
node_t* ParseVarInit(parser_t* parser);
node_t* ParseAssignment(parser_t* parser);
node_t* ParseReturn(parser_t* parser);
node_t* ParseFuncCall(parser_t* parser);
node_t* ParseCondition(parser_t* parser);
node_t* ParseBlock(parser_t* parser);
node_t* ParseVar(parser_t* parser);
node_t* ParseNum(parser_t* parser);

#define CUR_TOKEN (parser->lexer->tokens->data[parser->lexer->cur_token])
#define CUR_TYPE  (CUR_TOKEN->type)
#define CUR_HASH  (CUR_TOKEN->hash)
#define CUR_START (CUR_TOKEN->start)
#define CUR_LEN   (CUR_TOKEN->length)
#define CUR_POS   (parser->lexer->cur_token)

#define PREV_TOKEN (parser->lexer->tokens->data[parser->lexer->cur_token - 1])
#define PREV_TYPE  (PREV_TOKEN->type)
#define PREV_HASH  (PREV_TOKEN->hash)
#define PREV_START (PREV_TOKEN->start)
#define PREV_LEN   (PREV_TOKEN->length)
#define PREV_POS   (parser->lexer->cur_token - 1)

#define CUR_NAME_TABLE (parser->name_tables->data[parser->cur_name_table])


#endif