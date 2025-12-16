#include "spu.h"
#include "colors.h"

// #define DEBUG
#include "DEBUG.h"


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

    if (CURRENT_BIT(spu, OP_NUM))
    {
        if (CURRENT_BIT(spu, OP_MEM))
            STACK_PUSH_(&spu->stk, spu->ram[val_1]);
        else
            STACK_PUSH_(&spu->stk, val_1);

        (spu->pc) += sizeof(arg_t);
        return SUCCESS;
    }

    if (CURRENT_BIT(spu, OP_REG))
    {
        if (CURRENT_BIT(spu, OP_MEM))
            STACK_PUSH_(&spu->stk, spu->ram[spu->regs[val_1]]);
        else
            STACK_PUSH_(&spu->stk, spu->regs[val_1]);

        (spu->pc)++;
        return SUCCESS;
    }

    return ARG_NEX;
}

spuErr_t FUNC_CMD_POP(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &val_1);
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;

    STACK_POP_(&spu->stk, &val_2);

    if (CURRENT_BIT(spu, OP_NUM) && CURRENT_BIT(spu, OP_MEM))
    {
        spu->ram[(size_t)val_1] = val_2;

        (spu->pc) += sizeof(arg_t);
        return SUCCESS;
    }

    if (CURRENT_BIT(spu, OP_REG))
    {
        if (CURRENT_BIT(spu, OP_MEM))
            spu->ram[(size_t)spu->regs[val_1]] = val_2;
        else
            spu->regs[val_1] = val_2;

        (spu->pc)++;
        return SUCCESS;
    }

    return ARG_NEX;
}


#define POP_TWO_VALUES(spu, val_1, val_2, count) \
    do { \
        ON_DEBUG( if (IS_BAD_PTR(spu)) return BAD_SPU_PTR; ) \
        \
        STACK_POP_(&spu->stk, &val_1); \
        \
        if (count == 2) \
            STACK_POP_(&spu->stk, &val_2); \
        \
    } while(0)

spuErr_t FUNC_CMD_ADD(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_TWO_VALUES(spu, val_1, val_2, 2);

    STACK_PUSH_(&spu->stk, val_2 + val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_SUB(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_TWO_VALUES(spu, val_1, val_2, 2);

    STACK_PUSH_(&spu->stk, val_2 - val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_MUL(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_TWO_VALUES(spu, val_1, val_2, 2);
    
    STACK_PUSH_(&spu->stk, val_2 * val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_DIV(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_TWO_VALUES(spu, val_1, val_2, 2);

    if (val_1 != 0)
    {
        STACK_PUSH_(&spu->stk, val_2 / val_1);
        return SUCCESS;
    }

    return DIV_BY_ZERO;
}

spuErr_t FUNC_CMD_SQRT(spu_t *spu)
{
    arg_t val_1 = 0;
    arg_t val_2 = 0;

    POP_TWO_VALUES(spu, val_1, val_2, 1);
    
    if (val_1 >= 0)
    {
        STACK_PUSH_(&spu->stk, (arg_t)sqrt(val_1));
        return SUCCESS;
    }

    return SQRT_NEG;
}

#undef POP_value

spuErr_t FUNC_CMD_IN(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t val_1 = 0;

    printf("Enter number:\n");
    while (scanf("%d", &val_1) == 0)
    {
        while (getchar() != '\n');
        printf("Invalid input. Please enter a number.\n");
        printf("Enter number:\n");
    }

    STACK_PUSH_(&spu->stk, val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_OUT(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t val_1 = 0;

    STACK_POP_(&spu->stk, &val_1);

    printf(ANSI_COLOR_GREEN "OUT: %d\n" ANSI_COLOR_RESET, val_1);
    return SUCCESS;
}

spuErr_t FUNC_CMD_JMP(spu_t *spu)
{
    ON_DEBUG( 
            if (IS_BAD_PTR(spu))
                return BAD_SPU_PTR;
            )

    arg_t jump_addr = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); 
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;

    if (CURRENT_BIT(spu, OP_NUM) && 3 < jump_addr)
    {
        spu->pc = (size_t)(jump_addr - 1);
        return SUCCESS;
    }

    if (CURRENT_BIT(spu, OP_REG) && 0 <= jump_addr && jump_addr < NUM_REG)
    {
        spu->pc = (size_t)(spu->regs[jump_addr] - 1);
        return SUCCESS;
    }

    return ARG_NEX;
}


#define JWC(spu, condition) \
    do { \
        ON_DEBUG( if (IS_BAD_PTR(spu)) return BAD_SPU_PTR; ) \
        \
        arg_t jump_addr = 0; \
        arg_t val_1 = 0, val_2 = 0; \
        \
        spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); \
        if(read_arg_verd != SUCCESS) \
            return read_arg_verd; \
        \
        STACK_POP_(&spu->stk, &val_1); \
        STACK_POP_(&spu->stk, &val_2); \
        \
        if (CURRENT_BIT(spu, OP_NUM)) \
        { \
            if (jump_addr > 3) \
                (spu->pc) += sizeof(arg_t); \
            else \
                return ARG_NEX; \
        } \
        if (val_2 condition val_1) \
            (spu->pc) = (size_t)jump_addr - 1; \
    } while(0)

spuErr_t FUNC_CMD_JE(spu_t *spu)
{
    JWC(spu, == );
    return SUCCESS;
}

spuErr_t FUNC_CMD_JNE(spu_t *spu)
{
    JWC(spu, != );
    return SUCCESS;
}

spuErr_t FUNC_CMD_JA(spu_t *spu)
{
    JWC(spu, < );
    return SUCCESS;
}

spuErr_t FUNC_CMD_JAE(spu_t *spu)
{
    JWC(spu, <= );
    return SUCCESS;
}

spuErr_t FUNC_CMD_JB(spu_t *spu)
{
    JWC(spu, > );
    return SUCCESS;
}

spuErr_t FUNC_CMD_JBE(spu_t *spu)
{
    JWC(spu, >= );
    return SUCCESS;
}

#undef JWC

spuErr_t FUNC_CMD_CALL(spu_t *spu)
{
    ON_DEBUG( if (IS_BAD_PTR(spu)) return BAD_SPU_PTR; )

    arg_t jump_addr = 0;

    spuErr_t read_arg_verd = ReadArg(spu, &jump_addr); 
    if(read_arg_verd != SUCCESS)
        return read_arg_verd;

    if (CURRENT_BIT(spu, OP_NUM))
    {
        if (jump_addr > 3)
            (spu->pc) += sizeof(arg_t);
        else 
            return ARG_NEX;
    }
    
    STACK_PUSH_(&spu->stk_ret, spu->pc);

    spu->pc = (size_t)jump_addr - 1;

    return SUCCESS;
}

spuErr_t FUNC_CMD_RET(spu_t *spu)
{
    ON_DEBUG( if (IS_BAD_PTR(spu)) return BAD_SPU_PTR; )

    size_t jump_addr = 0;

    STACK_POP_(&spu->stk_ret, &jump_addr);

    if (jump_addr <= 3)
            return ARG_NEX;

    spu->pc = jump_addr;
    return SUCCESS;
}