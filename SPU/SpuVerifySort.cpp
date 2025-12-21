#include "spu.hpp"
#include "SpuInstrSet.cpp"


spuErr_t VerifySpuInstrSetSort()
{
    for (size_t elem = 1; elem < LEN_INSTR_SET; ++elem)
    {
        if (spu_instr_set[elem].cmd < spu_instr_set[elem - 1].cmd)
            return SPU_ERROR;
    }

    return SPU_SUCCESS;
}