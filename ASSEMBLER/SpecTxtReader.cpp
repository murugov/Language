#include "compile.h"
#include "SizeFile.h"
#include "LineCounter.h"


line_with_num_t* SpecTxtReader(FILE *SourceFile, char* buffer, size_t *count_line)
{
    if (IS_BAD_PTR(SourceFile))
        return NULL;

    ssize_t file_size = SizeFile(SourceFile);
    if (file_size < 0)
        return NULL;

    buffer = (char*)calloc((size_t)file_size + 1, sizeof(char));
    if (IS_BAD_PTR(buffer))
        return NULL;

    size_t capacity = fread(buffer, sizeof(char), (size_t)file_size, SourceFile);
    buffer[capacity] = '\0';

    for (size_t num_elem = 0; num_elem < capacity; ++num_elem)
        buffer[num_elem] = (char)toupper(buffer[num_elem]);

    *count_line = LineCounter(buffer);
    if (*count_line == 0)
        return NULL;

    line_with_num_t *arr_ptr = (line_with_num_t*)calloc(*count_line + 1, sizeof(line_with_num_t));
    if (IS_BAD_PTR(arr_ptr))
        return NULL;
    
    size_t cmd_number = 0;
    size_t line_number = 0;
    char* line_start = buffer;
    char* next_n = buffer;
    
    while ((next_n = strchr(line_start, '\n')) != NULL)
    {
        line_number++;
        int has_content = 0;

        for (char* ptr = line_start; ptr < next_n; ++ptr)
        {
            if (!isspace((byte_t)*ptr))
            {
                has_content = 1;
                arr_ptr[cmd_number].ptr = ptr;
                arr_ptr[cmd_number].line = line_number;
                break;
            }
        }
        
        if (has_content)
            cmd_number++;
        
        line_start = next_n + 1;

        *next_n = '\0';
    }
    
    if (*line_start != '\0')
    {
        for (char* ptr = line_start; *ptr != '\0'; ++ptr)
        {
            if (!isspace((byte_t)*ptr)) 
            {
                arr_ptr[cmd_number].line = line_number + 1;
                arr_ptr[cmd_number++].ptr = ptr;
                break;
            }
        }
    }

    return arr_ptr;
}