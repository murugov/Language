#ifndef ASM_GEN_HPP
#define ASM_GEN_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "IsBadPtr.hpp"
#include "TXTreader.hpp"
#include "GetHash.hpp"
#include "colors.hpp"

#define PATH_TO_SRC_KEY_FILE  "GENERATOR/src/keywords.txt"
#define PATH_TO_WOLF_OP_H     "GENERATOR/reports/HashOp.hpp"
#define PATH_TO_OP_INSTR_SET  "GENERATOR/reports/OpInstrSet.cpp"
#define PATH_TO_KEYWORD_SET   "GENERATOR/reports/KeywordSet.cpp"


typedef unsigned char count_t;
typedef char          *cmd_t;
typedef unsigned char num_t;

enum GenErr_t
{
    SUCCESS = 0,
    ERROR   = 1
};

struct asm_instr_t
{
    count_t count;
    hash_t  hash;
    cmd_t   cmd;
};

struct spu_instr_t
{
    count_t count;
    num_t num;
    cmd_t cmd;
};

GenErr_t GenCmdEnum(FILE *CmdEnumsFile, char **arr_ptr, size_t count_line);
GenErr_t GenAsmInstrSet(FILE *AsmInstrSetFile, char **arr_ptr, size_t count_line);
GenErr_t GenSpuInstrSet(FILE *SpuInstrSetFile, char **arr_ptr, size_t count_line);
hash_t HashCmd(const char *cmd);
int CmpByHash(const void *a, const void *b);
int CmpByNum(const void *a, const void *b);

#endif