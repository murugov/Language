#include "compile.h"
#include "colors.h"

// #define DEBUG
#include "DEBUG.h"


AsmErr_t CodeWriter(FILE *ByteCode, line_with_num_t *arr_cmd, size_t count_cmd)
{    
    if (IS_BAD_PTR(ByteCode))
        return BAD_OUTPUT_FILE_PTR;
        
    ON_DEBUG( if (IS_BAD_PTR(arr_cmd)) return BAD_ARR_CMD_PTR; )
        
    byte_t *code = (byte_t*)calloc(count_cmd * (1 + sizeof(arg_t)) + 1, sizeof(byte_t));
    if (IS_BAD_PTR(code))
        return BAD_CODE_PTR;

    size_t count_labels = 0;
    label_t *arr_labels = (label_t*)calloc(count_cmd, sizeof(label_t));
    if (IS_BAD_PTR(arr_labels))
        return BAD_ARR_LABELS_PTR;

    size_t pc = 0;
    
    AsmErr_t first_pass = Compilation(code, arr_cmd, count_cmd, arr_labels, &count_labels, &pc);
    if (first_pass != SUCCESS)
        return first_pass;

    if (count_labels > 0)
    {
        AsmErr_t second_pass = Compilation(code, arr_cmd, count_cmd, arr_labels, &count_labels, &pc);
        if (second_pass != SUCCESS)
            return second_pass;
    }

    sign_t signature = {SIGNATURE, VERSION};

    fwrite(&signature, sizeof(sign_t), 1, ByteCode); 
    fwrite(code, sizeof(byte_t), pc, ByteCode);

    free(code);
    free(arr_labels);
        
    return SUCCESS;
}