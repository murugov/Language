#include "spu.hpp"
#include "SpuInstrSet.cpp"


spuErr_t spuExecutor(spu_t *spu)
{
    size_t count_cmd = spu->pc;
    spu->pc = 3;

    for (;spu->pc < count_cmd; ++(spu->pc))
    {
        byte_t cmd = (spu->code[spu->pc]) & MASK_CMD;

        ssize_t index = 0;
        spuErr_t search_verd = CmdSearch(cmd, &index);

        if(search_verd != SPU_SUCCESS)
            return SPU_UNKNOWN_CMD;

        spuErr_t calc_verd = (spu_instr_set[index].func)(spu);
        if (calc_verd != SPU_SUCCESS)
            return calc_verd;
    }

    return SPU_SUCCESS;
}


spuErr_t CmdSearch(byte_t cmd, ssize_t *index)
{
    ON_DEBUG(
            if (IS_BAD_PTR(index))
                return SPU_ERROR;
    )

    for (size_t i = 0; i < LEN_INSTR_SET; ++i)
    {
        if (spu_instr_set[i].cmd == cmd)
        {
            *index = i;
            return SPU_SUCCESS;
        }
    }

    return SPU_UNKNOWN_CMD;
}