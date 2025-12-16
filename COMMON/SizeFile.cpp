#include "SizeFile.hpp"


ssize_t SizeFile(FILE* stream)
{
    struct stat file_info = {};

    if (fstat(fileno(stream), &file_info) != 0)
        return -1;

    return (ssize_t)file_info.st_size;
}