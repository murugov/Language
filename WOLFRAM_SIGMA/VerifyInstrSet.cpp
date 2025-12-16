#include "wolfram.hpp"
#include "OpInstrSet.cpp"


WolfErr_t VerifyOpInstrSetSort()
{
    for (size_t elem = 1; elem < LEN_INSTR_SET; ++elem)
    {
        if (op_instr_set[elem].hash < op_instr_set[elem - 1].hash)
            return WOLF_ERROR;
    }

    return WOLF_SUCCESS;
}