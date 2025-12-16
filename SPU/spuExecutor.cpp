#include "spu.h"
#include "colors.h"

#include "SpuInstrSet.cpp"

// #define DEBUG
#include "DEBUG.h"

spuErr_t spuExecutor(spu_t *spu)
{
    size_t count_cmd = spu->pc;
    spu->pc = 3;

    for (;spu->pc < count_cmd; ++(spu->pc))
    {
        byte_t cmd = (spu->code[spu->pc]) & MASK_CMD;

        ssize_t index = 0;
        spuErr_t search_verd = CmdSearch(cmd, &index);

        if(search_verd != SUCCESS)
            return UNKNOWN_CMD;

        spuErr_t calc_verd = (spu_instr_set[index].func)(spu);
        if (calc_verd != SUCCESS)
            return calc_verd;
    }

    return SUCCESS;
}


spuErr_t CmdSearch(byte_t cmd, ssize_t *index)
{
    ON_DEBUG(
            if (IS_BAD_PTR(index))
                return ERROR;
    )
    byte_t arr_cmd[LEN_INSTR_SET] ={0};
    for (size_t i = 0; i < LEN_INSTR_SET; ++i)
        arr_cmd[i] = spu_instr_set[i].cmd;

    *index = BinSearch(arr_cmd, LEN_INSTR_SET, cmd);
    if (*index == -1)
        return UNKNOWN_CMD;
    
    return SUCCESS;
}