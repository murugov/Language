#include "wolfram.hpp"


int main(int argc, char *argv[])
{
    LogFileOpener(PATH_TO_LOGFILE);

    if (argc >= 3)
    {
        FILE *SourceFile = fopen(argv[1], "r");
        if (IS_BAD_PTR(SourceFile))
        {
            printf(ANSI_COLOR_RED "Bad pointer \"%s\"\n" ANSI_COLOR_RESET, argv[1]);
            LOG(ERROR, "Bad pointer \"%s\"", argv[1]);
            return EXIT_FAILURE;
        }

        FILE *LatexFile  = fopen(argv[2], "w");
        if (IS_BAD_PTR(LatexFile))
        {
            printf(ANSI_COLOR_RED "Bad pointer \"%s\"\n" ANSI_COLOR_RESET, argv[2]);
            LOG(ERROR, "Bad pointer \"%s\"", argv[2]);
            fclose(SourceFile);
            return EXIT_FAILURE;
        }

        if (VerifyOpInstrSetSort() != WOLF_SUCCESS)
        {
            printf(ANSI_COLOR_RED "Not sorted op_instr_set!\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "TODO: generation OpInstrSet.cpp with using \"make run-gen\"!\n" ANSI_COLOR_RESET);
            LOG(ERROR, "Not sorted op_instr_set in OpInstrSet.cpp!");
            return EXIT_FAILURE;
        }

        // LatexFileOpener(PATH_TO_LATEX);

        tree_t *wolf_tree = NULL;
        TREE_CTOR(wolf_tree);
        
        parseWolfTree(argv[1], wolf_tree);


        GenTrees(wolf_tree->root, __func__);


        // TaylorSeries(wolf_tree, "x", 0.0, 7);

        TREE_DTOR(wolf_tree);    
        GenHTML("WolfDump");

        // LatexFileCloser();

        fclose(SourceFile);
        fclose(LatexFile);
    }
    else
        printf(ANSI_COLOR_RED "Incorrect transfer of input files\n" ANSI_COLOR_RESET);


    LogFileCloser();
    return 0;
}