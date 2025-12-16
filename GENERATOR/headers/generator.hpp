#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "IsBadPtr.hpp"
#include "TXTreader.hpp"
#include "GetHash.hpp"
#include "colors.hpp"

#define PATH_TO_SRC_KEY_FILE   "GENERATOR/src/keywords.txt"
#define PATH_TO_HASH_OP_FILE   "GENERATOR/reports/HashOp.hpp"
#define PATH_TO_OP_INSTR_SET   "GENERATOR/reports/OpInstrSet.cpp"
#define PATH_TO_KEYWORD_SET    "GENERATOR/reports/KeywordSet.cpp"

#define PATH_TO_SRC_SPU_FILE   "GENERATOR/src/cmds.txt"
#define PATH_TO_CMD_CODES_FILE "GENERATOR/reports/CmdCodesEnum.hpp"
#define PATH_TO_ASM_INSTR_SET  "GENERATOR/reports/AsmInstrSet.cpp"
#define PATH_TO_SPU_INSTR_SET  "GENERATOR/reports/SpuInstrSet.cpp"

#define MAX_LEN_NAME_FUNC 8

typedef unsigned char count_t;
typedef char          *cmd_t;
typedef unsigned char num_t;

enum GenErr_t
{
    GEN_SUCCESS = 0,
    GEN_ERROR   = 1
};

struct funcInfo
{
    char   name[MAX_LEN_NAME_FUNC];
    char   op[MAX_LEN_NAME_FUNC];
    hash_t hash;
};

struct op_instr_t
{
    char name[32];
    char op[8];
    hash_t hash;
    int num_args; 
};

struct keyword_set_t
{
    char name[32];
    char key[8];
    hash_t hash;
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


void RemoveComments(char** arr_cmd, int *count_line);

GenErr_t GenHashOp(FILE *HashOpFile, char **arr_ptr, int count_line);
GenErr_t GenOpInstrSet(FILE *OpInstrSetFile, char **arr_ptr, int count_line);
GenErr_t GenKeywordSet(FILE *KeywordSetFile, char **arr_ptr, int count_line);

int CmpOpInstrSetByHash(const void *a, const void *b);
int CmpKeywordSetByHash(const void *a, const void *b);
int CmpByHash(const void *a, const void *b);


GenErr_t GenCmdEnum(FILE *CmdEnumsFile, char **arr_ptr, size_t count_line);
GenErr_t GenAsmInstrSet(FILE *AsmInstrSetFile, char **arr_ptr, size_t count_line);
GenErr_t GenSpuInstrSet(FILE *SpuInstrSetFile, char **arr_ptr, size_t count_line);

int CmpAsmInstrSetByHash(const void *a, const void *b);
int CmpSpuInstrSetByNum(const void *a, const void *b);


#endif