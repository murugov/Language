#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "IsBadPtr.hpp"
#include "logger.hpp"
#include "colors.hpp"

#define SIGNATURE  {'A', 'M'}
#define VERSION    5

#define NUM_REG         16
#define MAX_LEN_CMD     16
#define MAX_LEN_LABEL   16
#define HASH_TABEL_SIZE 2048

typedef int           arg_t;
typedef               size_t hash_t;
typedef unsigned char byte_t;

struct sign_t
{
    const char signature[2];
    byte_t     vers;
};

#include "CmdCodesEnum.hpp"

enum amErr_t
{
    ASM_SUCCESS              = 0x00,
    ASM_ERROR                = 0x01,
    ASM_BAD_INPUT_FILE_PTR   = 0x02,
    ASM_BAD_OUTPUT_FILE_PTR  = 0x03,
    ASM_BAD_BUFFER_PTR       = 0x04,
    ASM_BAD_ARR_CMD_PTR      = 0x05,
    ASM_BAD_CODE_PTR         = 0x06,
    ASM_BAD_ARR_LABELS_PTR   = 0x07,
    ASM_WRONG_FILE_SIZE      = 0x08,
    ASM_CTOR_FAIL            = 0x09,
    ASM_CMD_NUM_FAIL         = 0x0A,
    ASM_CMD_WITH_ARG_FAIL    = 0x0B,
    ASM_CMD_WITHOUT_ARG_FAIL = 0x0C,
    ASM_LINE_SIZE_EXCEED     = 0x0D,
    ASM_REG_NEX              = 0x0E,
    ASM_MEM_NEX              = 0x0F,
    ASM_UNKNOWN_CMD          = 0x10,
    ASM_UNKNOWN_LABEL        = 0x11,
    ASM_RE_LABEL             = 0x12
};

enum operands_t
{
    OP_NUM = 0x20,
    OP_REG = 0x40,
    OP_MEM = 0x80
};

struct label_t
{
    const char* name;
    hash_t      hash;
    size_t      pc;
};


struct asm_context
{
    byte_t   *code;
    char     *ptr;
    size_t   pc;
    CmdCodes cmd;
    label_t  *arr_labels;
    size_t   count_labels;
};

amErr_t CmdWithArg(asm_context *write_params);
amErr_t CmdWithoutArg(asm_context *write_params);
typedef amErr_t (*func_t)(asm_context *write_params);

amErr_t ArgIsConstNum(asm_context *write_params);
amErr_t ArgIsReg(asm_context *write_params);
amErr_t ArgIsMem(asm_context *write_params);
amErr_t ArgIsLabel(asm_context *write_params);
amErr_t LabelSearch(hash_t hash_label, label_t *arr_labels, size_t count_labels, ssize_t *index);


struct WrapCmd
{
    func_t   func;
    hash_t   hash;
    CmdCodes cmd; 
};


amErr_t VerifyAsmInstrSetSort();
hash_t HashCmd(const char *buffer);
char** ArrPtrCtor(FILE *SourceFile, char* buffer, int *count_line);
void RemoveComments(char** arr_cmd, int *count_line);
amErr_t CodeCtor(FILE *ByteCode, char **arr_ptr, int count_cmd);

amErr_t FirstCompilation(char **arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc);
amErr_t AddLabel(char *label, label_t *arr_labels, size_t *count_labels);
int CmpByHash(const void *a, const void *b);

amErr_t SecondCompilation(byte_t *code, char **arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc);
amErr_t HashSearch(hash_t hash_func, ssize_t *index);
int CmpForBinSearch(const void *a, const void *b);

void AsmErrPrint(char *SourceFile, char *ByteCode, amErr_t verd);
void AsmDtor(char *buffer, char **arr_ptr);

#define IS_BAD_PTR(ptr) IsBadPtr((void*)ptr)


#endif