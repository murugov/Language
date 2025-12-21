#include "assembler.hpp"
#include "TXTreader.hpp"


char** ArrPtrCtor(FILE *SourceFile, char* buffer, int *count_line)
{
    size_t len_buffer = 0;
    char **arr_ptr = TXTreader(SourceFile, buffer, &len_buffer, count_line, toupper);
    
    ON_DEBUG(
            if (IS_BAD_PTR(arr_ptr))
                return NULL;
            )

    RemoveComments(arr_ptr, count_line);

    return arr_ptr;
}


void RemoveComments(char** arr_cmd, int *count_line)
{
    for (int i = 0; i < *count_line; ++i)
    {
        char* colon = strchr(arr_cmd[i], ';');

        if (colon != NULL)
            *colon = '\0';
    }
}


void AsmDtor(char *buffer, char **arr_ptr)
{
    free(buffer);
    free(arr_ptr);
}