#include "LineCounter.hpp"


int LineCounter(char* buffer)
{
    if (IS_BAD_PTR(buffer)) return 0;

    int count_line = 0;
    char* line_start = buffer;
    char* next_n = buffer;
    
    while ((next_n = strchr(line_start, '\n')) != NULL)
    {
        int has_content = 0;

        for (char* ptr = line_start; ptr < next_n; ++ptr)
        {
            if (!isspace((unsigned char)*ptr))
            {
                has_content = 1;
                break;
            }
        }
        
        if (has_content)
            count_line++;
        
        line_start = next_n + 1;
    }
    
    if (*line_start != '\0')
    {
        for (char* ptr = line_start; *ptr != '\0'; ++ptr)
        {
            if (!isspace((unsigned char)*ptr)) 
            {
                count_line++;
                break;
            }
        }
    }
    return count_line;
}