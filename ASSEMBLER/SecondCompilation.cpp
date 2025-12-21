#include "assembler.hpp"
#include "AsmInstrSet.cpp"


amErr_t SecondCompilation(byte_t *code, char **arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc)
{
    ON_DEBUG(
            if (IS_BAD_PTR(code))
                return ASM_ERROR;
            if (IS_BAD_PTR(arr_cmd))
                return ASM_ERROR;
            if (IS_BAD_PTR(arr_labels))
                return ASM_ERROR;
            if (IS_BAD_PTR(count_labels))
                return ASM_ERROR;
            if (IS_BAD_PTR(pc))
                return ASM_ERROR;
    )

    (*pc) = 0;
    asm_context write_params = {code, NULL, *pc, asm_instr_set[0].cmd, arr_labels, *count_labels};

    for (size_t line = 0; line < count_cmd; ++line)
    {
        if (strchr(arr_cmd[line], ':') != NULL)
            continue;

        hash_t hash_func = HashCmd(arr_cmd[line]);

        ssize_t index = 0;
        amErr_t search_verd = HashSearch(hash_func, &index);

        if(search_verd != ASM_SUCCESS)
            return search_verd;

        write_params.ptr = arr_cmd[line];
        write_params.cmd = asm_instr_set[index].cmd;
        amErr_t write_verd = (asm_instr_set[index].func)(&write_params);

        if (write_verd != ASM_SUCCESS)
            return write_verd;
    }

    code = write_params.code;
    *pc = write_params.pc;
    arr_labels = write_params.arr_labels;
    *count_labels = write_params.count_labels;

    return ASM_SUCCESS;
}


amErr_t HashSearch(hash_t hash_func, ssize_t *index)
{   
    for (ssize_t i = 0; i < LEN_INSTR_SET; ++i)
    {
        if (asm_instr_set[i].hash == hash_func)
        {
            *index = i;
            return ASM_SUCCESS;
        }
    }
    
    return ASM_UNKNOWN_CMD;
}


// amErr_t HashSearch(hash_t hash_func, ssize_t *index)
// {
//     ON_DEBUG(
//             if (IS_BAD_PTR(index))
//                 return ASM_ERROR;
//             )

//     WrapCmd search_key = {0};
//     search_key.hash = hash_func;

//     printf("search_ptr: [%p]\n", search_key);
//     printf("asm_instr_set: [%p]\n", asm_instr_set);
//     printf("LEN_INSTR_SET: [%d]\n", LEN_INSTR_SET);
//     printf("sizeof(WrapCmd): [%zu]\n", sizeof(WrapCmd));
//     printf("CmpForBinSearch: [%p]\n", CmpForBinSearch);


//     WrapCmd *found = (WrapCmd*)bsearch(&search_key, asm_instr_set, LEN_INSTR_SET, sizeof(WrapCmd), CmpForBinSearch);

//     printf("hash_func: [%zu]\n", hash_func);
//     printf("found: [%p]\n", found);
//     if (IS_BAD_PTR(found)) return ASM_UNKNOWN_CMD;
    
//     *index = found - asm_instr_set;
//     return ASM_SUCCESS;
// }


// int CmpForBinSearch(const void *a, const void *b)
// {
//     const hash_t *hash_a = (const hash_t*)a;
//     const WrapCmd *cmd_b = (const WrapCmd*)b;
    
//     if (*hash_a > cmd_b->hash)
//         return 1;
//     if (*hash_a < cmd_b->hash)
//         return -1;
//     return 0;
// }