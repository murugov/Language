#include "spu.h"

#include "SpuInstrSet.cpp"

spuErr_t VerifySpuInstrSetSort()
{
    for (size_t elem = 1; elem < LEN_INSTR_SET; ++elem)
    {
        if (spu_instr_set[elem].cmd < spu_instr_set[elem - 1].cmd)
            return ERROR;
    }

    return SUCCESS;
}