#include "compile.h"

// #define DEBUG
#include "DEBUG.h"


line_with_num_t* ArrPtrCtor(FILE *SourceFile, char* buffer, size_t *count_line)
{
    line_with_num_t *arr_ptr = SpecTxtReader(SourceFile, buffer, count_line);
    
    ON_DEBUG( if (IS_BAD_PTR(arr_ptr)) return NULL; )

    RemoveComments(arr_ptr, count_line);

    return arr_ptr;
}


void RemoveComments(line_with_num_t *arr_cmd, size_t *count_line)
{
    for (size_t i = 0; i < *count_line; ++i)
    {
        char* colon = strchr(arr_cmd[i].ptr, ';');

        if (colon != NULL)
            *colon = '\0';
    }
}


void AsmDtor(char *buffer, line_with_num_t *arr_ptr)
{
    free(buffer);
    free(arr_ptr);
}