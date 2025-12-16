#include "compile.h"
#include "colors.h"

#include "AsmInstrSet.cpp"

// #define DEBUG
#include "DEBUG.h"


byte_t hash_table[HASH_TABEL_SIZE] = {0};

AsmErr_t Compilation(byte_t *code, line_with_num_t *arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc)
{
    ON_DEBUG(
            if (IS_BAD_PTR(code))
                return ERROR;
            if (IS_BAD_PTR(arr_cmd))
                return ERROR;
            if (IS_BAD_PTR(arr_labels))
                return ERROR;
            if (IS_BAD_PTR(count_labels))
                return ERROR;
            if (IS_BAD_PTR(pc))
                return ERROR;
    )

    (*pc) = 0;
    bool is_first_pass = !(bool)(*count_labels);
    asm_context write_params = {code, NULL, MASK_EMP, *pc, asm_instr_set[0].cmd, arr_labels, *count_labels, is_first_pass};

    for (size_t line = 0; line < count_cmd; ++line)
    {
        if (strchr(arr_cmd[line].ptr, ':') != NULL)
        {
            if (is_first_pass)
            {
                printf(ANSI_COLOR_GREEN "pc_label = [%zu]\n" ANSI_COLOR_RESET, write_params.pc);
                if (strlen(arr_cmd[line].ptr) >= MAX_LEN_CMD + MAX_LEN_LABEL)
                    return LINE_SIZE_EXCEED;
    
                AsmErr_t add_label_verd = AddLabel(arr_cmd, line, arr_labels, &(write_params.count_labels), write_params.pc);
                if (add_label_verd != SUCCESS)
                    return add_label_verd;
            }
        }
        else
        {
            printf(ANSI_COLOR_YELLOW "pc_cmd = [%zu]\n" ANSI_COLOR_RESET, write_params.pc);

            hash_t hash_func = HashCmd(arr_cmd[line].ptr);

            ssize_t index = 0;
            AsmErr_t search_verd = HashSearch(hash_func, &index);
            if(search_verd != SUCCESS)
                return search_verd;
            
            write_params.ptr = arr_cmd[line].ptr;
            write_params.cmd = asm_instr_set[index].cmd;
            write_params.mask = asm_instr_set[index].mask;
            
            AsmErr_t write_verd = (asm_instr_set[index].func)(&write_params);
            if (write_verd != SUCCESS)
                return write_verd;
        }

    }

    qsort(arr_labels, *count_labels, sizeof(label_t), LabelCmpByHash);

    code = write_params.code;
    *pc = write_params.pc;
    arr_labels = write_params.arr_labels;
    *count_labels = write_params.count_labels;

    return SUCCESS;
}


AsmErr_t AddLabel(line_with_num_t *arr_cmd, size_t line, label_t *arr_labels, size_t *count_labels, size_t pc)
{
    ON_DEBUG(
            if (IS_BAD_PTR(label))
                return ERROR;
            if (IS_BAD_PTR(arr_labels))
                return ERROR;
            if (IS_BAD_PTR(count_labels))
                return ERROR;
    )

    hash_t orig_hash = HashCmd(arr_cmd[line].ptr);
    hash_t index = orig_hash % HASH_TABEL_SIZE;
    
    if (hash_table[index] == 0)
        hash_table[index] = 1;
    else
    {
        for (size_t i = 0; i < *count_labels; ++i)
        {
            if (arr_labels[i].hash == orig_hash)
            {
                if (strcmp(arr_labels[i].name, arr_cmd[line].ptr))
                    return RE_LABEL;

                break;
            }
        }
    }

    arr_labels[*count_labels].name = arr_cmd[line].ptr;
    arr_labels[*count_labels].hash = orig_hash;
    arr_labels[*count_labels].pc = pc + 3;
    (*count_labels)++;

    return SUCCESS;
}


AsmErr_t HashSearch(hash_t hash_func, ssize_t *index)
{
    ON_DEBUG( if (IS_BAD_PTR(index)) return ERROR; )

    hash_t arr_hash[LEN_INSTR_SET] = {0};
    for (size_t i = 0; i < LEN_INSTR_SET; ++i)
        arr_hash[i] = asm_instr_set[i].hash;

    *index = BinSearch(arr_hash, LEN_INSTR_SET, hash_func);
    if (*index == -1)
        return UNKNOWN_CMD;
    
    return SUCCESS;
}

// AsmErr_t HashSearch(hash_t hash_cmd, size_t *index)
// {
//     ON_DEBUG( if (IS_BAD_PTR(index)) return ERROR; )
//     printf("hash_cmd = %zu\n", hash_cmd);
//     wrap_t *found = (wrap_t*)bsearch(&hash_cmd, asm_instr_set, LEN_INSTR_SET, sizeof(asm_instr_set[0]), CmpForBinSearch);
//     printf("found = %p\n", found);
//     if (found == NULL)
//         return UNKNOWN_CMD;
    
//     *index = (size_t)(found - asm_instr_set);
//     return SUCCESS;
// }


// int CmpForBinSearch(const void *a, const void *b)
// {
//     const hash_t *hash_a = (const hash_t*)a;
//     const wrap_t *wrap_b = (const wrap_t*)b;
//     if (*hash_a - wrap_b->hash > 0)
//         return 1;
//     if (*hash_a - wrap_b->hash < 0)
//         return -1;
//     return 0;
// }


int LabelCmpByHash(const void *a, const void *b)
{
    const label_t *arr_labels_a = (const label_t*)a;
    const label_t *arr_labels_b = (const label_t*)b;

    if (arr_labels_a->hash - arr_labels_b->hash > 0)
        return 1;
    if (arr_labels_a->hash - arr_labels_b->hash < 0)
        return -1;
    return 0;
}