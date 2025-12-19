#include "assembler.hpp"


int main(int argc, char *argv[])
{
    LogFileOpener(PATH_TO_LOGFILE);

    if (argc >= 3)
    {
        FILE *SourceFile = fopen(argv[1], "rb");
        if (IS_BAD_PTR(SourceFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, argv[1]); return EXIT_FAILURE; }

        FILE *ByteCode = fopen(argv[2], "wb");
        if (IS_BAD_PTR(ByteCode)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, argv[2]); fclose(SourceFile); return EXIT_FAILURE; }

        if (VerifyAsmInstrSetSort() != ASM_SUCCESS)
        {
            printf(ANSI_COLOR_RED "Not sorted asm_instr_set!\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "TODO: generation AsmInstrSet.cpp with using 'make run-gen'!\n" ANSI_COLOR_RESET);
            ON_DEBUG( LOG(ERROR, "Not sorted op_instr_set in AsmInstrSet.cpp!"); )
            fclose(SourceFile);
            fclose(ByteCode);
            return EXIT_FAILURE;
        }
    
        char *buffer = NULL;
        int count_line = 0;
        char **arr_ptr = ArrPtrCtor(SourceFile, buffer, &count_line);
    
        AsmErr_t asm_verd = CodeCtor(ByteCode, arr_ptr, count_line);
        AsmErrPrint(argv[1], argv[2], asm_verd);
    
        AsmDtor(buffer, arr_ptr);

        fclose(SourceFile);
        fclose(ByteCode);   
        printf(ANSI_COLOR_GREEN "Assembly generation successful: %s created\n" ANSI_COLOR_RESET, argv[2]);
    }
    else
        printf(ANSI_COLOR_RED "Incorrect transfer of input files!\n" ANSI_COLOR_RESET);

    LogFileCloser();
    return 0;
}