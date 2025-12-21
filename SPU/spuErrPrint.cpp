#include "spu.hpp"

#define PATH_TO_BYTECODE "src/CompileFiles/bytecode.asm"


void spuErrPrint(spuErr_t verd)
{
    switch (verd)
    {
        case SPU_BAD_INPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, PATH_TO_BYTECODE);
            break;

        case SPU_BAD_OUTPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, PATH_TO_LOGFILE);
            break;

        case SPU_BAD_SPU_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to spu!" ANSI_COLOR_RESET);
            break;

        case SPU_BAD_SPU_CODE_PTR:
            perror(ANSI_COLOR_RED "Error creating code!" ANSI_COLOR_RESET);
            break;

        case SPU_BAD_SPU_REGS_PTR:
            perror(ANSI_COLOR_RED "Error creating regs!" ANSI_COLOR_RESET);
            break;

        case SPU_BAD_SPU_RAM_PTR:
            perror(ANSI_COLOR_RED "Error creating RAM!" ANSI_COLOR_RESET);
            break;

        case SPU_BAD_ARR_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to arr_ptr!\n" ANSI_COLOR_RESET);
            break;

        case SPU_BAD_ARR_CMD_PC_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to arr_ptr_cmd_pc!\n" ANSI_COLOR_RESET);
            break;

        case SPU_SPU_DTOR_FAIL:
            perror(ANSI_COLOR_RED "Destruction error!" ANSI_COLOR_RESET);
            break;

        case SPU_WRONG_FILE_SIZE:
            perror(ANSI_COLOR_RED "Error getting file size!\n" ANSI_COLOR_RESET);
            break;
            
        case SPU_WRONG_STK:
            perror(ANSI_COLOR_RED "Error occurred while creating the stack\n" ANSI_COLOR_RESET);
            break;

        case SPU_WRONG_SIGN:
            printf(ANSI_COLOR_RED "Signature of the %s does not match!\n" ANSI_COLOR_RESET, PATH_TO_BYTECODE);
            break;

        case SPU_WRONG_VERS:
            printf(ANSI_COLOR_RED "Version of the %s does not match!\n" ANSI_COLOR_RESET, PATH_TO_BYTECODE);
            break;

        case SPU_UNKNOWN_CMD:
            printf(ANSI_COLOR_RED "Unknown command in %s!\n" ANSI_COLOR_RESET, PATH_TO_BYTECODE); // return line
            break;

        case SPU_DIV_BY_ZERO:
            perror(ANSI_COLOR_RED "Division by zero!\n" ANSI_COLOR_RESET);
            break;

        case SPU_SQRT_NEG:
            perror(ANSI_COLOR_RED "Negative number under the root!\n" ANSI_COLOR_RESET);
            break;
            
        case SPU_ARG_NEX:
            perror(ANSI_COLOR_RED "Non-existent argument!\n" ANSI_COLOR_RESET);
            break; 

        case SPU_MEM_NEX:
            perror(ANSI_COLOR_RED "Non-existent memory argument!\n" ANSI_COLOR_RESET);
            break; 
            
        case SPU_ERROR_STK:
            perror(ANSI_COLOR_RED "Spu stack error!\n" ANSI_COLOR_RESET);
            break;

        case SPU_STOP_BY_HLT:
            printf(ANSI_COLOR_GREEN "Stop by HLT: SUCCESS\n" ANSI_COLOR_RESET);
            break;

        case SPU_SUCCESS:
            printf(ANSI_COLOR_GREEN "SUCSESS\n" ANSI_COLOR_RESET);
            break;

        case SPU_ERROR:
            break;

        default:
            break;
    }
}