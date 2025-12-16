#ifndef ASSEMBLER_HPP
#define ASSEMBLER_HPP

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "IsBadPtr.hpp"

#define SIGNATURE  {'A', 'M'}
#define VERSION    7

#define NUM_REG         16
#define MAX_LEN_CMD     8
#define MAX_LEN_LABEL   8

typedef int           arg_t;
typedef unsigned char count_t;
typedef size_t        hash_t;
typedef unsigned char byte_t;

struct sign_t
{
    const char signature[2];
    byte_t     vers;
};

#include "CmdCodesEnum.hpp"

enum AsmErr_t
{
    SUCCESS              = 0x00,
    ERROR                = 0x01,
    BAD_INPUT_FILE_PTR   = 0x02,
    BAD_OUTPUT_FILE_PTR  = 0x03,
    BAD_BUFFER_PTR       = 0x04,
    BAD_ARR_CMD_PTR      = 0x05,
    BAD_CODE_PTR         = 0x06,
    BAD_ARR_LABELS_PTR   = 0x07,
    WRONG_FILE_SIZE      = 0x08,
    CTOR_FAIL            = 0x09,
    CMD_NUM_FAIL         = 0x0A,
    CMD_WITH_ARG_FAIL    = 0x0B,
    CMD_WITHOUT_ARG_FAIL = 0x0C,
    LINE_SIZE_EXCEED     = 0x0D,
    REG_NEX              = 0x0E,
    MEM_NEX              = 0x0F,
    ARG_NEX              = 0x10,
    UNKNOWN_CMD          = 0x11,
    UNKNOWN_LABEL        = 0x12,
    RE_LABEL             = 0x13
};

enum mask_t
{
    MASK_EMP = 0x00,
    MASK_NUM = 0x01,
    MASK_REG = 0x02,
    MASK_MEM = 0x04,
    MASK_LAB = 0x08
};

enum operands_t
{
    OP_NUM       = 0x20,
    OP_REG       = 0x40,
    OP_MEM       = 0x80,
    OP_TWO_BYTES = 0x80
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
    byte_t   mask;
    size_t   pc;
    CmdCodes cmd;
    label_t  *arr_labels;
    size_t   count_labels;
    bool     is_first_pass;
};

AsmErr_t ReadOpcode16(asm_context *write_params);
AsmErr_t ReadOpcode8(asm_context *write_params);
typedef AsmErr_t (*func_t)(asm_context *write_params);

AsmErr_t CheckArg(asm_context *write_params, byte_t count_args);

AsmErr_t ArgIsConstNum(asm_context *write_params, byte_t count_args);
AsmErr_t ArgIsReg(asm_context *write_params, byte_t count_args);
AsmErr_t ArgIsMem(asm_context *write_params, byte_t count_args);
AsmErr_t ArgIsLabel(asm_context *write_params, byte_t count_args);
AsmErr_t LabelSearch(hash_t hash_label, label_t *arr_labels, size_t count_labels, ssize_t *index);


struct wrap_t
{
    func_t   func;
    byte_t   mask;
    hash_t   hash;
    CmdCodes cmd; 
};


AsmErr_t VerifyAsmInstrSetSort();
hash_t HashCmd(const char *buffer);

struct line_with_num_t
{
    char   *ptr;
    size_t line;
};

line_with_num_t* ArrPtrCtor(FILE *SourceFile, char* buffer, size_t *count_line);
void RemoveComments(line_with_num_t *arr_cmd, size_t *count_line);
line_with_num_t* SpecTxtReader(FILE *SourceFile, char* buffer, size_t *count_line);

AsmErr_t CodeWriter(FILE *ByteCode, line_with_num_t *arr_ptr, size_t count_n);

AsmErr_t Compilation(byte_t *code, line_with_num_t *arr_cmd, size_t count_cmd, label_t *arr_labels, size_t *count_labels, size_t *pc);
AsmErr_t AddLabel(line_with_num_t *arr_cmd, size_t line, label_t *arr_labels, size_t *count_labels, size_t pc);
AsmErr_t HashSearch(hash_t hash_cmd, ssize_t *index);
//int CmpForBinSearch(const void *a, const void *b);
int LabelCmpByHash(const void *a, const void *b);

void AsmErrPrint(char *SourceFile, char *ByteCode, AsmErr_t verd);
void AsmDtor(char *buffer, line_with_num_t *arr_ptr);

#define IS_BAD_PTR(ptr) IsBadPtr((void*)ptr)
#define MASK_CHECK(type_cmd, mask) (type_cmd & mask) == mask

#include "BinSearch.hpp"

#endif