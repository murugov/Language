#include "spu.hpp"

#define PATH_TO_BYTECODE "src/CompileFiles/bytecode.asm"


int main()
{
    LogFileOpener(PATH_TO_LOGFILE);

    FILE *ByteCode = fopen(PATH_TO_BYTECODE, "rb");
    if (IS_BAD_PTR(ByteCode)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_BYTECODE); return EXIT_FAILURE; }

    if (VerifySpuInstrSetSort() != SPU_SUCCESS)
    {
        printf(ANSI_COLOR_RED "Not sorted asm_instr_set!\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_RED "TODO: generation SpuInstrSet.cpp with using 'make run-gen'!\n" ANSI_COLOR_RESET);
        ON_DEBUG( LOG(ERROR, "Not sorted op_instr_set in SpuInstrSet.cpp!"); )
        fclose(ByteCode);
        return EXIT_FAILURE;
    }

    spu_t spu = {};

    spuErr_t ctor_verd = spuCtor(&spu, ByteCode);
    if(ctor_verd)
        spuErrPrint(ctor_verd);

    spuErr_t execut_verd = spuExecutor(&spu);
    if(execut_verd)
        spuErrPrint(execut_verd);

    spuDtor(&spu);

    fclose(ByteCode);   
    printf(ANSI_COLOR_GREEN "SPU working successful: %s created\n" ANSI_COLOR_RESET, PATH_TO_BYTECODE);

    LogFileCloser();
    return 0;
}