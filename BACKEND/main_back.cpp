#include "translator.hpp"


int main()                  // argc/argv
{
    LogFileOpener(PATH_TO_LOGFILE);

    trans_t *trans = (trans_t*)calloc(1, sizeof(trans_t));
    transCtor(trans, "reports/ast.txt");

    TranslateTree(trans, "src/CompileFiles/source.asm");

    GenTrees(trans->node, __func__);
    GenHTML(PATH_TO_HTML);

    transDtor(trans);
    LogFileCloser();

    return 0;
}