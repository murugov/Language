#include "spu.hpp"


spuErr_t ReadArg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return SPU_BAD_SPU_PTR;
            if (IS_BAD_PTR(val))
                return SPU_BAD_SPU_PTR;
    )

    spuErr_t arg_is_mem_verd = SpuArgIsMem(spu, val);
    if (arg_is_mem_verd != SPU_ERROR)
        return arg_is_mem_verd;

    spuErr_t arg_is_const_num_verd = SpuArgIsConstNum(spu, val);
    if (arg_is_const_num_verd != SPU_ERROR)
        return arg_is_const_num_verd;

    spuErr_t arg_is_reg_verd = SpuArgIsReg(spu, val);
    if (arg_is_reg_verd != SPU_ERROR)
        return arg_is_reg_verd;

    return SPU_ARG_NEX;
}


spuErr_t SpuArgIsConstNum(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return SPU_ERROR;
            if (IS_BAD_PTR(val))
                return SPU_ERROR;
    )

    if (CURRENT_BIT(spu, OP_NUM))
    {
        arg_t temp = 0;
        memcpy(&temp, &(spu->code[spu->pc + 1]), sizeof(arg_t));
        
        *val = temp;
        return SPU_SUCCESS;
    }

    return SPU_ERROR;
}


spuErr_t SpuArgIsReg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return SPU_ERROR;
            if (IS_BAD_PTR(val))
                return SPU_ERROR;
    )

    if (CURRENT_BIT(spu, OP_REG))
    {        
        byte_t reg_num = spu->code[spu->pc + 1];
        if (reg_num < 0 || NUM_REG <= reg_num)
            return SPU_ARG_NEX;
        
        *val = reg_num;
        return SPU_SUCCESS;
    }

    return SPU_ERROR;
}


spuErr_t SpuArgIsMem(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return SPU_ERROR;
            if (IS_BAD_PTR(val))
                return SPU_ERROR;
    )

    if (CURRENT_BIT(spu, OP_MEM))
    {
        spuErr_t arg_is_const_num_verd = SpuArgIsConstNum(spu, val);
        if (arg_is_const_num_verd != SPU_ERROR)
            return arg_is_const_num_verd;

        spuErr_t arg_is_reg_verd = SpuArgIsReg(spu, val);
        if (arg_is_reg_verd != SPU_ERROR)
            return arg_is_reg_verd;

        return SPU_MEM_NEX;
    }

    return SPU_ERROR;
}