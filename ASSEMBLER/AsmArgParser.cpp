#include "assembler.hpp"


amErr_t ArgIsConstNum(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_ERROR;
    )

    arg_t arg_num = 0;

    int exist_op_mem = sscanf(write_params->ptr, "%*s [%d]", &arg_num) == 1;
    if (sscanf(write_params->ptr, "%*s %d ", &arg_num) == 1 || exist_op_mem)
    {
        write_params->code[write_params->pc] = (write_params->cmd) | OP_NUM;   
        if (exist_op_mem)
            write_params->code[write_params->pc] = (write_params->code[write_params->pc]) | OP_MEM; 

        memcpy(&(write_params->code[write_params->pc + 1]), &arg_num, sizeof(arg_t));
        (write_params->pc) += 1 + sizeof(arg_t);

        return ASM_SUCCESS;
    }

    return ASM_ERROR;
}


amErr_t ArgIsReg(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_ERROR;
    )

    arg_t arg_reg = 0;

    int exist_op_mem = sscanf(write_params->ptr, "%*s [R%d]", &arg_reg) == 1;
    if (sscanf(write_params->ptr, "%*s R%d ", &arg_reg) == 1 || exist_op_mem)
    {
        if (arg_reg < 0 || arg_reg >= NUM_REG)
            return ASM_REG_NEX;
        
        write_params->code[write_params->pc] = (write_params->cmd) | OP_REG;
        if (exist_op_mem)
            write_params->code[write_params->pc] = (write_params->code[write_params->pc]) | OP_MEM;

        (write_params->pc)++;

        write_params->code[write_params->pc] = (unsigned char)arg_reg;
        (write_params->pc)++;

        return ASM_SUCCESS;
    }

    return ASM_ERROR;
}


amErr_t ArgIsMem(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_ERROR;
    )

    char label[MAX_LEN_LABEL] = "\0";

    if (sscanf(write_params->ptr, "%*s [%s] ", label) == 1)
    {
        amErr_t arg_is_const_num_verd = ArgIsConstNum(write_params);
        if (arg_is_const_num_verd != ASM_ERROR)
            return arg_is_const_num_verd;

        amErr_t arg_is_reg_verd = ArgIsReg(write_params);
        if (arg_is_reg_verd != ASM_ERROR)
            return arg_is_reg_verd;

        return ASM_MEM_NEX;
    }

    return ASM_ERROR;
}


amErr_t ArgIsLabel(asm_context *write_params)
{
    ON_DEBUG(
            if (IS_BAD_PTR(write_params))
                return ASM_ERROR;
    )

    arg_t arg_num = 0;
    char label[MAX_LEN_LABEL] = "\0";

    if (sscanf(write_params->ptr, "%*s %s ", label) == 1 && (CMD_JMP <= (write_params->cmd) && (write_params->cmd) <= CMD_CALL))
    {
        hash_t hash_label = HashCmd(label) * 31 + ':';       // very bad, may be define

        ssize_t index = 0;
        amErr_t search_verd = LabelSearch(hash_label, write_params->arr_labels, write_params->count_labels, &index);

        if(search_verd != ASM_SUCCESS)
            return search_verd;

        write_params->code[write_params->pc] = (write_params->cmd) | OP_NUM;
        arg_num = (arg_t)(*write_params).arr_labels[index].pc;
        memcpy(&(write_params->code[write_params->pc + 1]), &arg_num, sizeof(arg_t));

        (write_params->pc) += 1 + sizeof(arg_t);
        return ASM_SUCCESS;
    }

    return ASM_ERROR;
}


amErr_t LabelSearch(hash_t hash_label, label_t *arr_labels, size_t count_labels, ssize_t *index)
{
    ON_DEBUG(
            if (IS_BAD_PTR(index))
                return ASM_ERROR;
    )

    hash_t *arr_hash = (hash_t*)calloc(count_labels, sizeof(hash_t));
    for (size_t i = 0; i < count_labels; ++i)
    {
        if (arr_labels[i].hash == hash_label)
        {
            *index = (ssize_t)i;
            free(arr_hash);
            return ASM_SUCCESS;
        }

    }

    free(arr_hash);
    return ASM_UNKNOWN_LABEL;
}