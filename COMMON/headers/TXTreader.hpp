#ifndef TXT_READER_HPP
#define TXT_READER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "IsBadPtr.hpp"
#include "SizeFile.hpp"
#include "LineCounter.hpp"


char** TXTreader(FILE *SourceFile, char* buffer, size_t *len_buffer, int *count_line, int (*convert)(int));
void FreeLines(char **lines, int count_lines);

#endif