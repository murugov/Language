#include "spu.h"
#include "colors.h"
#include "SizeFile.h"


spuErr_t spuCtor(spu_t *spu, FILE *stream)
{
    if (IS_BAD_PTR(stream))
        return BAD_INPUT_FILE_PTR;

    ssize_t file_size = SizeFile(stream);
    if (file_size < 0)
        return WRONG_FILE_SIZE;

    spu->code = (byte_t*)calloc((size_t)file_size + 1, sizeof(byte_t));
    if (IS_BAD_PTR(spu->code))
        return BAD_SPU_CODE_PTR;

    fread(spu->code, sizeof(byte_t), (size_t)file_size, stream);

    spuErr_t file_ext_verd = SignVersVerify(spu->code);
    if (file_ext_verd)
        return file_ext_verd;

    spu->pc = (size_t)file_size;

    STACK_INIT(&spu->stk, START_STK_CAP);

    if (spu->stk.error != SUCCESS)
        return WRONG_STK;

    spu->regs = (arg_t*)calloc(NUM_REG, sizeof(arg_t));
    if (IS_BAD_PTR(spu->regs))
        return BAD_SPU_REGS_PTR;

    STACK_INIT(&spu->stk_ret, START_STK_CAP);

    spu->ram = (arg_t*)calloc(NUM_RAM, sizeof(arg_t));
    if (IS_BAD_PTR(spu->ram))
        return BAD_SPU_RAM_PTR;

    return SUCCESS;
}


spuErr_t SignVersVerify(byte_t* code)
{
    const char* signature = SIGNATURE;
    byte_t version = VERSION;

    if (signature[0] != code[0] || signature[1] != code[1])
        return WRONG_SIGN;
    if (version != code[2])
        return WRONG_VERS;

    return SUCCESS;
}


spuErr_t spuDtor(spu_t *spu)
{
    free(spu->code);
    StackDtor(&spu->stk);
    free(spu->regs);
    StackDtor(&spu->stk_ret);
    free(spu->ram);
    HashArrDtor();

    return SUCCESS;
}