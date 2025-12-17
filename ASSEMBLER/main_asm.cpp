#include "assembler.hpp"


int main(int argc, char *argv[])
{
    if (argc >= 3)
    {
        FILE *SourceFile = fopen(argv[1], "rb");
        if (IS_BAD_PTR(SourceFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, argv[1]); { return EXIT_FAILURE; } }

        FILE *ByteCode = fopen(argv[2], "wb");
        if (IS_BAD_PTR(ByteCode)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, argv[2]); { fclose(SourceFile); return EXIT_FAILURE; } }

        if (VerifyAsmInstrSetSort() != ASM_SUCCESS)
        {
            printf(ANSI_COLOR_RED "Not sorted asm_instr_set!\n" ANSI_COLOR_RESET);
            printf(ANSI_COLOR_RED "TODO: generation AsmInstrSet.cpp with using \"make run-gen\"!\n" ANSI_COLOR_RESET);
            ON_DEBUG( LOG(ERROR, "Not sorted op_instr_set in AsmInstrSet.cpp!"); )
            return EXIT_FAILURE;
        }
    
        char *buffer = NULL;
        size_t count_cmd = 0;
        line_with_num_t *arr_ptr = ArrPtrCtor(SourceFile, buffer, &count_cmd);
    
        AsmErr_t asm_verd = CodeWriter(ByteCode, arr_ptr, count_cmd);
        AsmErrPrint(argv[1], argv[2], asm_verd);
    
        AsmDtor(buffer, arr_ptr);           // mistake

        fclose(SourceFile);
        fclose(ByteCode);
    }
    else
        printf(ANSI_COLOR_RED "Incorrect transfer of input files\n" ANSI_COLOR_RESET);

    return 0;
}