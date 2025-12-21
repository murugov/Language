#include "./parser.hpp"
#include "AstParser.hpp"


int main()                  // argc/argv
{
    LogFileOpener(PATH_TO_LOGFILE);

    parser_t *parser = (parser_t*)calloc(1, sizeof(parser_t));
    parserCtor(parser, "src/data.txt");

    node_t* ast = ParseAST(parser);
    if (ast)
    {
        printf(ANSI_COLOR_GREEN "Successfully parsed\n" ANSI_COLOR_RESET);
        GenTrees(ast, __func__);
        FILE* report = fopen("reports/ast.txt", "w");
        AstWriter(ast, report);
        fclose(report);
        FreeNodes(ast);
    }
    else
        printf(ANSI_COLOR_RED "Parsing failed!\n" ANSI_COLOR_RESET);    
    
    parserDtor(parser);
    LogFileCloser();

    return 0;
}