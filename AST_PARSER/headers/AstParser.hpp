#ifndef AST_PARSER_HPP
#define AST_PARSER_HPP

#include "tree.hpp"

char *AstReader(FILE *SourceFile);
node_t *NodeReader(char** cur_ptr, node_t* parent);
void AstWriter(node_t* node, FILE* stream);

#endif