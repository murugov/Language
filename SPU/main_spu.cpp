#include "spu.h"
#include "colors.h"

FILE *LogFile = fopen(LOG_FILE, "wb");


int main()
{
    FILE *ByteCode = fopen(BYTE_CODE, "rb");

    if (IS_BAD_PTR(ByteCode))
    {
        perror(ANSI_COLOR_RED "Bad pointer ByteCode" ANSI_COLOR_RESET);
        return 1;
    }

    if (VerifySpuInstrSetSort() != SUCCESS)
    {
        printf(ANSI_COLOR_RED "Not sorted asm_instr_set!\n" ANSI_COLOR_RESET);
        printf(ANSI_COLOR_RED "TODO: generation SpuInstrSet.cpp with using \"make run-gen\"!\n" ANSI_COLOR_RESET);
        return 1;
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
    fclose(LogFile);

    return 0;
}