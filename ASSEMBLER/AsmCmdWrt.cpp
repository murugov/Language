#include "assembler.hpp"


AsmErr_t CmdWithArg(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_CMD_WITH_ARG_FAIL;
    )

    AsmErr_t arg_is_mem_verd = ArgIsMem(write_params);
    if (arg_is_mem_verd != ASM_ERROR)
        return arg_is_mem_verd;

    AsmErr_t arg_is_label_verd = ArgIsLabel(write_params);
    if (arg_is_label_verd != ASM_ERROR)
        return arg_is_label_verd;

    AsmErr_t arg_is_const_num_verd = ArgIsConstNum(write_params);
    if (arg_is_const_num_verd != ASM_ERROR)
        return arg_is_const_num_verd;

    AsmErr_t arg_is_reg_verd = ArgIsReg(write_params);
    if (arg_is_reg_verd != ASM_ERROR)
        return arg_is_reg_verd;

    return ASM_UNKNOWN_CMD;
}


AsmErr_t CmdWithoutArg(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_CMD_WITHOUT_ARG_FAIL;
    )

    write_params->code[write_params->pc] = write_params->cmd;
    (write_params->pc)++;

    return ASM_SUCCESS;
}