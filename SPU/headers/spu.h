#ifndef SPU_H
#define SPU_H

#include <math.h>
#include "stack.h"

#define BYTE_CODE "./CompileFiles/bytecode.asm"
#define LOG_FILE "./ReportFiles/LogFile.log"

#define SIGNATURE "AM"
#define VERSION   5

#include "CmdCodesEnums.h"

enum spuErr_t
{
    SUCCESS             = 0x00,
    ERROR               = 0x01,
    BAD_INPUT_FILE_PTR  = 0x02,
    BAD_OUTPUT_FILE_PTR = 0x03,
    BAD_SPU_PTR         = 0x04,
    BAD_SPU_CODE_PTR    = 0x05,
    BAD_SPU_REGS_PTR    = 0x06,
    BAD_SPU_RAM_PTR     = 0x07,
    BAD_ARR_PTR         = 0x08,
    BAD_ARR_CMD_PC_PTR  = 0x09,
    SPU_DTOR_FAIL       = 0x0A,
    WRONG_FILE_SIZE     = 0x0B,
    WRONG_STK           = 0x0C,
    WRONG_SIGN          = 0x0D,
    WRONG_VERS          = 0x0E,
    UNKNOWN_CMD         = 0x0F,
    DIV_BY_ZERO         = 0x10,
    SQRT_NEG            = 0x11,
    ARG_NEX             = 0x12,
    MEM_NEX             = 0x13,
    STOP_BY_HLT         = 0x14,
    ERROR_STK           = 0x15
};


#define MASK_CMD 0x1F

enum operands_t
{
    OP_NUM = 0x20,
    OP_REG = 0x40,
    OP_MEM = 0x80
};

typedef int           arg_t;
typedef size_t        hash_t;
typedef unsigned char byte_t;

#define START_STK_CAP 16
#define NUM_REG       16
#define NUM_RAM       128

struct spu_t
{
    byte_t               *code;
    size_t               pc;
    stk_t<arg_t>         stk;
    arg_t                *regs;
    stk_t<unsigned long> stk_ret;
    arg_t                *ram;
};


spuErr_t FUNC_CMD_HLT(spu_t *spu);
spuErr_t FUNC_CMD_PUSH(spu_t *spu);
spuErr_t FUNC_CMD_POP(spu_t *spu);
spuErr_t FUNC_CMD_ADD(spu_t *spu);
spuErr_t FUNC_CMD_SUB(spu_t *spu);
spuErr_t FUNC_CMD_MUL(spu_t *spu);
spuErr_t FUNC_CMD_DIV(spu_t *spu);
spuErr_t FUNC_CMD_SQRT(spu_t *spu);
spuErr_t FUNC_CMD_IN(spu_t *spu);
spuErr_t FUNC_CMD_OUT(spu_t *spu);
spuErr_t FUNC_CMD_JMP(spu_t *spu);
spuErr_t FUNC_CMD_JE(spu_t *spu);
spuErr_t FUNC_CMD_JNE(spu_t *spu);
spuErr_t FUNC_CMD_JA(spu_t *spu);
spuErr_t FUNC_CMD_JAE(spu_t *spu);
spuErr_t FUNC_CMD_JB(spu_t *spu);
spuErr_t FUNC_CMD_JBE(spu_t *spu);
spuErr_t FUNC_CMD_CALL(spu_t *spu);
spuErr_t FUNC_CMD_RET(spu_t *spu);
typedef spuErr_t (*func_t)(spu_t *spu);

spuErr_t ReadArg(spu_t *spu, arg_t *val);
spuErr_t SpuArgIsConstNum(spu_t *spu, arg_t *val);
spuErr_t SpuArgIsReg(spu_t *spu, arg_t *val);
spuErr_t SpuArgIsMem(spu_t *spu, arg_t *val);


struct WrapCmd
{
    func_t   func;
    CmdCodes cmd; 
};

spuErr_t VerifySpuInstrSetSort();
spuErr_t SignVersVerify(byte_t* code);
spuErr_t spuCtor(spu_t *spu, FILE *stream);
spuErr_t spuExecutor(spu_t *spu);
spuErr_t CmdSearch(byte_t cmd, ssize_t *index);
spuErr_t spuDtor(spu_t *spu);
void spuErrPrint(spuErr_t verd);

#define CURRENT_BIT(spu, op) (spu->code[spu->pc] & op) == op

#define STACK_PUSH_(stk, elem) \
    do { \
        if (StackPush(stk, elem)) \
            return ERROR_STK;\
    } while(0)

#define STACK_POP_(stk, elem) \
    do { \
        if (StackPop(stk, elem)) \
            return ERROR_STK;\
    } while(0)

#include "BinSearch.hpp"

#endif