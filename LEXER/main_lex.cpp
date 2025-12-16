#include "parser.hpp"


int main()
{
    LogFileOpener(PATH_TO_LOGFILE);


    parser_t *parser = (parser_t*)calloc(1, sizeof(parser_t));
    parserCtor(parser);

    node_t* ast = ParseAST(parser);
    printf("ast: [%p]\n", ast);
    if (ast)
    {
        printf(ANSI_COLOR_GREEN "Successfully parsed!\n" ANSI_COLOR_RESET);
        GenTrees(ast, __func__);
        FreeNodes(ast);
    }
    else
        printf(ANSI_COLOR_RED "Parsing failed\n" ANSI_COLOR_RESET);    
    
    parserDtor(parser);
    LogFileCloser();

    return 0;
}