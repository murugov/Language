#include "compile.h"

// #define DEBUG
#include "DEBUG.h"


AsmErr_t ReadOpcode8(asm_context *write_params)
{
    ON_DEBUG( if (IS_BAD_PTR(write_params)) return CMD_WITH_ARG_FAIL; )

    write_params->code[write_params->pc] = write_params->cmd;
    (write_params->pc)++;

    if (write_params->mask == MASK_EMP)
        return SUCCESS;

    return CheckArg(write_params, 1);
}


AsmErr_t ReadOpcode16(asm_context *write_params)
{
    ON_DEBUG( if (IS_BAD_PTR(write_params)) return CMD_WITH_ARG_FAIL; )

    write_params->code[write_params->pc] = write_params->cmd | OP_TWO_BYTES;
    (write_params->pc)++;

    char arg_1[MAX_LEN_LABEL] = {0};
    char arg_2[MAX_LEN_LABEL] = {0};

    if (sscanf(write_params->ptr, " %*s %[^,], %s ", arg_1, arg_2) == 2)
    {
        write_params->ptr = arg_1;
        AsmErr_t check_arg_1_verd = CheckArg(write_params, 2);
        if (check_arg_1_verd != SUCCESS)
            return check_arg_1_verd;

        write_params->ptr = arg_2;
        AsmErr_t check_arg_2_verd = CheckArg(write_params, 1);

        return check_arg_2_verd;
    }

    return ARG_NEX;
}


AsmErr_t CheckArg(asm_context *write_params, byte_t count_args)
{
    ON_DEBUG( if (IS_BAD_PTR(write_params)) return CMD_WITH_ARG_FAIL; )

    if (MASK_CHECK(write_params->mask, MASK_LAB))
    {
        AsmErr_t arg_is_label_verd = ArgIsLabel(write_params, count_args);
        if (arg_is_label_verd != ERROR)
            return arg_is_label_verd;
    }

    if (MASK_CHECK(write_params->mask, MASK_MEM))
    {
        AsmErr_t arg_is_mem_verd = ArgIsMem(write_params, count_args);
        if (arg_is_mem_verd != ERROR)
            return arg_is_mem_verd;
    }

    if (MASK_CHECK(write_params->mask, MASK_NUM))
    {
        AsmErr_t arg_is_const_num_verd = ArgIsConstNum(write_params, count_args);
        if (arg_is_const_num_verd != ERROR)
            return arg_is_const_num_verd;
    }

    if (MASK_CHECK(write_params->mask, MASK_REG))
    {
        AsmErr_t arg_is_reg_verd = ArgIsReg(write_params, count_args);
        if (arg_is_reg_verd != ERROR)
            return arg_is_reg_verd;
    }

    return ARG_NEX;
}