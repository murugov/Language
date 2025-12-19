#include "assembler.hpp"
#include "AsmInstrSet.cpp"


AsmErr_t VerifyAsmInstrSetSort()
{
    for (size_t elem = 1; elem < LEN_INSTR_SET; ++elem)
    {
        if (asm_instr_set[elem].hash < asm_instr_set[elem - 1].hash)
            return ASM_ERROR;
    }

    return ASM_SUCCESS;
}