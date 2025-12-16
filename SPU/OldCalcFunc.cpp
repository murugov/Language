#include "spu.h"
#include "colors.h"

#define CURRENT_BIT(spu, op) (spu->code[spu->pc] & op) == op

// #define DEBUG
#include "DEBUG.h"

spuErr_t ReadMemArg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;

            if (IS_BAD_PTR(val))
                return BAD_SPU_PTR;
            )
    
    if (CURRENT_BIT(spu, OP_NUM))
    {
        union {
            arg_t signed_val;
            unsigned char bytes[sizeof(arg_t)];
        } converter;
        
        for (size_t byte = 0; byte < sizeof(arg_t); ++byte)
        {
            (spu->pc)++;
            converter.bytes[byte] = (spu->code)[spu->pc];
        }

        if (converter.signed_val < 0)
            return MEM_NEX;

        *val = converter.signed_val;
        return SUCCESS;
    }

    else if (CURRENT_BIT(spu, OP_REG))
    {
        (spu->pc)++;
        
        unsigned char reg_num = spu->code[spu->pc];
        
        if (reg_num >= NUM_REG)
            return ARG_NEX;
        
        if (spu->regs[reg_num] < 0)
            return MEM_NEX;
            
        *val = spu->regs[reg_num];
        return SUCCESS;
    }

    return MEM_NEX;
}


spuErr_t ReadArg(spu_t *spu, arg_t *val)
{
    ON_DEBUG(
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;

            if (IS_BAD_PTR(val))
                return BAD_SPU_PTR;
            )

    if (CURRENT_BIT(spu, OP_MEM))
    {
        if(ReadMemArg(spu, val))
            return MEM_NEX;
        
        *val = spu->ram[*val];
        return SUCCESS;
    }

    else if (CURRENT_BIT(spu, OP_NUM))
    { 
        union {
            arg_t signed_val;
            unsigned char bytes[sizeof(arg_t)];
        } converter;
        
        for (size_t byte = 0; byte < sizeof(arg_t); ++byte)
        {
            (spu->pc)++;
            converter.bytes[byte] = (spu->code)[spu->pc];
        }
        
        *val = converter.signed_val;
        return SUCCESS;
    }
    
    else if (CURRENT_BIT(spu, OP_REG))
    {
        (spu->pc)++;
        
        unsigned char reg_num = spu->code[spu->pc];
        
        if (reg_num >= NUM_REG)
            return ARG_NEX;
        
        *val = spu->regs[reg_num];
        return SUCCESS;
    }
    
    return ARG_NEX;
}

// {
// int cmd = code[pc++];

// int arg = 0;
// if (cmd & 0x20) { arg +=.      *(int*) code;  code += sizeof (int);  }
// if (cmd & 0x40) { arg += regs [*(char*)code]; code += sizeof (char); }
// if (cmd & 0x80) { arg  = ram  [arg]; }

// return struct_array [cmd & 0x1f] . func (spu, arg);
// }

#define JWC(spu, condition) \
    do { \
        ON_DEBUG( \
                if (IS_BAD_PTR(spu)) \
                    return BAD_SPU_PTR; \
                )\
        arg_t jump_addr = 0; \
        arg_t val_1 = 0, val_2 = 0; \
        \
        spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); \
        if(read_arg_verd != SUCCESS) \
            return read_arg_verd; \
        \
        if (jump_addr < 0) \
            return ARG_NEX; \
        \
        if (StackPop(&spu->stk, &val_1)) \
            return ERROR_STK; \
        if (StackPop(&spu->stk, &val_2)) \
            return ERROR_STK; \
        \
        if (condition) \
            spu->pc = (size_t)jump_addr - 1; \
    } while(0)


#define POP_value(spu, val_1, val_2, count) \
    do { \
        ON_DEBUG( \
                if (IS_BAD_PTR(spu)) \
                    return BAD_SPU_PTR; \
                )\
        if(StackPop(&spu->stk, &val_1)) \
            return ERROR_STK; \
        \
        if (count == 2) \
        { \
        if(StackPop(&spu->stk, &val_2)) \
            return ERROR_STK; \
        } \
        \
    } while(0)


spuErr_t FUNC_CMD_HLT(spu_t * /*spu*/)
{
    return STOP_BY_HLT;
}

spuErr_t FUNC_CMD_PUSH(spu_t *spu)
{
    arg_t val_1 = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &val_1);
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;
    
    if(StackPush(&spu->stk, val_1))
        return ERROR_STK;
    
    return SUCCESS;
}

spuErr_t FUNC_CMD_POP(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    if(StackPop(&spu->stk, &val_1))
        return ERROR_STK;
    
    if (CURRENT_BIT(spu, OP_MEM))
    {
        if (ReadMemArg(spu, &val_2))
            return MEM_NEX;

        spu->ram[val_2] = val_1;
    }

    else
    {        
        (spu->pc)++;    
    
        if (spu->code[spu->pc] >= NUM_REG)
            return ARG_NEX;
    
        spu->regs[(size_t)spu->code[spu->pc]] = val_1;
    }
    
    return SUCCESS;
}

spuErr_t FUNC_CMD_ADD(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_value(spu, val_1, val_2, 2);

    if(StackPush(&spu->stk, val_2 + val_1))
        return ERROR_STK;

    return SUCCESS;
}

spuErr_t FUNC_CMD_SUB(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_value(spu, val_1, val_2, 2);
    
    if(StackPush(&spu->stk, val_2 - val_1))
        return ERROR_STK;

    return SUCCESS;
}

spuErr_t FUNC_CMD_MUL(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_value(spu, val_1, val_2, 2);
    
    if(StackPush(&spu->stk, val_2 * val_1))
        return ERROR_STK;

    return SUCCESS;
}

spuErr_t FUNC_CMD_DIV(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_value(spu, val_1, val_2, 2);

    if (val_1 != 0)
    {
        if(StackPush(&spu->stk, val_2 / val_1))
            return ERROR_STK;

        return SUCCESS;
    }

    return DIV_BY_ZERO;
}

spuErr_t FUNC_CMD_SQRT(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_value(spu, val_1, val_2, 1);
    
    if (val_1 >= 0)
    {
        if(StackPush(&spu->stk, (arg_t)sqrt(val_1)))
            return ERROR_STK;
    
        return SUCCESS;
    }

    return SQRT_NEG;
}

spuErr_t FUNC_CMD_IN(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t val_1 = 0;

    printf("Enter number:\n");
    scanf(" %d", &val_1);

    if(StackPush(&spu->stk, val_1))
        return ERROR_STK;

    return SUCCESS;
}

spuErr_t FUNC_CMD_OUT(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t val_1 = 0;

    if(StackPop(&spu->stk, &val_1))
        return ERROR_STK;

    printf(ANSI_COLOR_GREEN "OUT: %d\n" ANSI_COLOR_RESET, val_1);

    return SUCCESS;
}

spuErr_t FUNC_CMD_JMP(spu_t *spu) // я поменял ReadArg
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t jump_addr = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); 
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;

    if (jump_addr < 0)
        return ARG_NEX;

    spu->pc = (size_t)jump_addr - 1;

    return SUCCESS;
}

spuErr_t FUNC_CMD_JE(spu_t *spu)
{
    JWC(spu, val_2 == val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JNE(spu_t *spu)
{
    JWC(spu, val_2 != val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JA(spu_t *spu)
{
    JWC(spu, val_2 < val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JAE(spu_t *spu)
{
    JWC(spu, val_2 <= val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JB(spu_t *spu)
{
    JWC(spu, val_2 > val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JBE(spu_t *spu)
{
    JWC(spu, val_2 >= val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_CALL(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t jump_addr = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); 
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;
    
    if(StackPush(&spu->stk_ret, spu->pc + 1))
        return ERROR_STK;

    if (jump_addr < 0)
        return ARG_NEX;
    
    spu->pc = (size_t)jump_addr - 1;

    return SUCCESS;
}

spuErr_t FUNC_CMD_RET(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    size_t jump_addr = 0;

    if(StackPop(&spu->stk_ret, &jump_addr))
        return ERROR_STK;

    if (jump_addr < 0)
        return ARG_NEX;

    spu->pc = jump_addr - 1;

    return SUCCESS;
}