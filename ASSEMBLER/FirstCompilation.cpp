#include "assembler.hpp"


byte_t hash_table[HASH_TABEL_SIZE] = {0};

AsmErr_t FirstCompilation(char **arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc)
{
    ON_DEBUG(
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
    arg_t arg = 0;
    char label[MAX_LEN_CMD + MAX_LEN_LABEL] = "\0";

    for (size_t line = 0; line < count_cmd; ++line)
    {
        if (strchr(arr_cmd[line], ':') != NULL)
        {
            if (strlen(arr_cmd[line]) >= MAX_LEN_CMD + MAX_LEN_LABEL)
                return ASM_LINE_SIZE_EXCEED;

            AsmErr_t add_label_verd = AddLabel(arr_cmd[line], arr_labels, count_labels);
            if (add_label_verd != ASM_SUCCESS)
                return add_label_verd;

            arr_labels[*count_labels].name = arr_cmd[line];
            arr_labels[*count_labels].hash = HashCmd(arr_cmd[line]);
            arr_labels[*count_labels].pc = (*pc) + 3;
            (*count_labels)++;
        }
        else
        {
            if (sscanf(arr_cmd[line], " %*s [R%d] ", &arg) == 1)
                (*pc)++;
            else if (sscanf(arr_cmd[line], " %*s [%d] ", &arg) == 1)
                (*pc) += sizeof(arg_t);
            else if (sscanf(arr_cmd[line], " %*s R%d ", &arg) == 1)
                (*pc)++;
            else if (sscanf(arr_cmd[line], " %*s %d ", &arg) == 1)
                (*pc) += sizeof(arg_t);
            else if (sscanf(arr_cmd[line], " %*s %s ", label) == 1)
                (*pc) += sizeof(arg_t);
            
            (*pc)++;
        }

    }

    qsort(arr_labels, *count_labels, sizeof(label_t), CmpByHash);

    return ASM_SUCCESS;
}


AsmErr_t AddLabel(char *label, label_t *arr_labels, size_t *count_labels)
{
    ON_DEBUG(
            if (IS_BAD_PTR(label))
                return ASM_ERROR;
            if (IS_BAD_PTR(arr_labels))
                return ASM_ERROR;
            if (IS_BAD_PTR(count_labels))
                return ASM_ERROR;
    )

    hash_t orig_hash = HashCmd(label);
    hash_t index = orig_hash % HASH_TABEL_SIZE;
    
    if (hash_table[index] == 0)
        hash_table[index] = 1;
    else
    {
        for (size_t i = 0; i < *count_labels; ++i)
        {
            if (arr_labels[i].hash == orig_hash)
                return ASM_RE_LABEL;
        }
    }

    return ASM_SUCCESS;
}


int CmpByHash(const void *a, const void *b)
{
    const label_t *arr_labels_a = (const label_t*)a;
    const label_t *arr_labels_b = (const label_t*)b;

    if (arr_labels_a->hash > arr_labels_b->hash)
        return 1;
    if (arr_labels_a->hash == arr_labels_b->hash)
        return 0;
    return -1;
}