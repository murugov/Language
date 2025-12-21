#include "spu.hpp"
#include "SizeFile.hpp"


spuErr_t spuCtor(spu_t *spu, FILE *stream)
{
    if (IS_BAD_PTR(stream))
        return SPU_BAD_INPUT_FILE_PTR;

    ssize_t file_size = SizeFile(stream);
    if (file_size < 0)
        return SPU_WRONG_FILE_SIZE;

    spu->code = (byte_t*)calloc((size_t)file_size + 1, sizeof(byte_t));
    if (IS_BAD_PTR(spu->code))
        return SPU_BAD_SPU_CODE_PTR;

    fread(spu->code, sizeof(byte_t), (size_t)file_size, stream);

    spuErr_t file_ext_verd = SignVersVerify(spu->code);
    if (file_ext_verd)
        return file_ext_verd;

    spu->pc = (size_t)file_size;

    STACK_CTOR(&spu->stk, MIN_STK_LEN);

    if (spu->stk.error != SPU_SUCCESS)
        return SPU_WRONG_STK;

    spu->regs = (arg_t*)calloc(NUM_REG, sizeof(arg_t));
    if (IS_BAD_PTR(spu->regs))
        return SPU_BAD_SPU_REGS_PTR;

    STACK_CTOR(&spu->stk_ret, MIN_STK_LEN);

    spu->ram = (arg_t*)calloc(NUM_RAM, sizeof(arg_t));
    if (IS_BAD_PTR(spu->ram))
        return SPU_BAD_SPU_RAM_PTR;

    return SPU_SUCCESS;
}


spuErr_t SignVersVerify(byte_t* code)
{
    const char* signature = SIGNATURE;
    byte_t version = VERSION;

    if (signature[0] != code[0] || signature[1] != code[1])
        return SPU_WRONG_SIGN;
    if (version != code[2])
        return SPU_WRONG_VERS;

    return SPU_SUCCESS;
}


spuErr_t spuDtor(spu_t *spu)
{
    free(spu->code);
    StackDtor(&spu->stk);
    free(spu->regs);
    StackDtor(&spu->stk_ret);
    free(spu->ram);

    return SPU_SUCCESS;
}