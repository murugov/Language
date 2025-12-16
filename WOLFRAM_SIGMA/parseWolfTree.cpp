#include "wolfram.hpp"


lexerErr_t parseWolfTree(const char* src, tree_t *tree)
{
    FILE *SourceFile = fopen(src, "r");
    if (!SourceFile) { printf("Error: Cannot open file %s\n", src); return LEX_ERROR; }

    char *buffer = NULL;
    int count_lines = 0;
    char **lines = DataReader(SourceFile, buffer, &count_lines);
    fclose(SourceFile);
    if (IS_BAD_PTR(lines)) { printf("Error: Failed to read file\n"); return LEX_ERROR; }

    lexer_t *lexer = LexerCtor(lines, 1, __FILE__);
    if (IS_BAD_PTR(lexer))
    {
        printf("Error: Failed to initialize lexer\n");
        return LEX_ERROR;
    }

    StackDump(lexer->tokens, __FILE__, __func__, __LINE__);

    tree->root = ParseGeneral(lexer);
    printf("root: [%p]\n", tree->root);

    GenTrees(tree->root, __func__);

    LexerDtor(lexer);
    return LEX_SUCCESS;
}