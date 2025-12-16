#include "spu.h"

// #define DEBUG
#include "DEBUG.h"


spuErr_t ReadArg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            if (IS_BAD_PTR(val))
                return BAD_SPU_PTR;
    )

    spuErr_t arg_is_mem_verd = SpuArgIsMem(spu, val);
    if (arg_is_mem_verd != ERROR)
        return arg_is_mem_verd;

    spuErr_t arg_is_const_num_verd = SpuArgIsConstNum(spu, val);
    if (arg_is_const_num_verd != ERROR)
        return arg_is_const_num_verd;

    spuErr_t arg_is_reg_verd = SpuArgIsReg(spu, val);
    if (arg_is_reg_verd != ERROR)
        return arg_is_reg_verd;

    return ARG_NEX;
}


spuErr_t SpuArgIsConstNum(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return ERROR;
            if (IS_BAD_PTR(val))
                return ERROR;
    )

    if (CURRENT_BIT(spu, OP_NUM))
    {
        arg_t temp = 0;
        memcpy(&temp, &(spu->code[spu->pc + 1]), sizeof(arg_t));
        
        *val = temp;
        return SUCCESS;
    }

    return ERROR;
}


spuErr_t SpuArgIsReg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return ERROR;
            if (IS_BAD_PTR(val))
                return ERROR;
    )

    if (CURRENT_BIT(spu, OP_REG))
    {        
        byte_t reg_num = spu->code[spu->pc + 1];
        if (reg_num < 0 || NUM_REG <= reg_num)
            return ARG_NEX;
        
        *val = reg_num;
        return SUCCESS;
    }

    return ERROR;
}


spuErr_t SpuArgIsMem(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return ERROR;
            if (IS_BAD_PTR(val))
                return ERROR;
    )

    if (CURRENT_BIT(spu, OP_MEM))
    {
        spuErr_t arg_is_const_num_verd = SpuArgIsConstNum(spu, val);
        if (arg_is_const_num_verd != ERROR)
            return arg_is_const_num_verd;

        spuErr_t arg_is_reg_verd = SpuArgIsReg(spu, val);
        if (arg_is_reg_verd != ERROR)
            return arg_is_reg_verd;

        return MEM_NEX;
    }

    return ERROR;
}