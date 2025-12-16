#include "spu.h"
#include "colors.h"


void spuErrPrint(spuErr_t verd)
{
    switch (verd)
    {
        case BAD_INPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, BYTE_CODE);
            break;

        case BAD_OUTPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, LOG_FILE);
            break;

        case BAD_SPU_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to spu!" ANSI_COLOR_RESET);
            break;

        case BAD_SPU_CODE_PTR:
            perror(ANSI_COLOR_RED "Error creating code!" ANSI_COLOR_RESET);
            break;

        case BAD_SPU_REGS_PTR:
            perror(ANSI_COLOR_RED "Error creating regs!" ANSI_COLOR_RESET);
            break;

        case BAD_SPU_RAM_PTR:
            perror(ANSI_COLOR_RED "Error creating RAM!" ANSI_COLOR_RESET);
            break;

        case BAD_ARR_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to arr_ptr!\n" ANSI_COLOR_RESET);
            break;

        case BAD_ARR_CMD_PC_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to arr_ptr_cmd_pc!\n" ANSI_COLOR_RESET);
            break;

        case SPU_DTOR_FAIL:
            perror(ANSI_COLOR_RED "Destruction error!" ANSI_COLOR_RESET);
            break;

        case WRONG_FILE_SIZE:
            perror(ANSI_COLOR_RED "Error getting file size!\n" ANSI_COLOR_RESET);
            break;
            
        case WRONG_STK:
            perror(ANSI_COLOR_RED "Error occurred while creating the stack\n" ANSI_COLOR_RESET);
            break;

        case WRONG_SIGN:
            printf(ANSI_COLOR_RED "Signature of the %s does not match!\n" ANSI_COLOR_RESET, BYTE_CODE);
            break;

        case WRONG_VERS:
            printf(ANSI_COLOR_RED "Version of the %s does not match!\n" ANSI_COLOR_RESET, BYTE_CODE);
            break;

        case UNKNOWN_CMD:
            printf(ANSI_COLOR_RED "Unknown command in %s!\n" ANSI_COLOR_RESET, BYTE_CODE); // return line
            break;

        case DIV_BY_ZERO:
            perror(ANSI_COLOR_RED "Division by zero!\n" ANSI_COLOR_RESET);
            break;

        case SQRT_NEG:
            perror(ANSI_COLOR_RED "Negative number under the root!\n" ANSI_COLOR_RESET);
            break;
            
        case ARG_NEX:
            perror(ANSI_COLOR_RED "Non-existent argument!\n" ANSI_COLOR_RESET);
            break; 

        case MEM_NEX:
            perror(ANSI_COLOR_RED "Non-existent memory argument!\n" ANSI_COLOR_RESET);
            break; 
            
        case ERROR_STK:
            perror(ANSI_COLOR_RED "Spu stack error!\n" ANSI_COLOR_RESET);
            break;

        case STOP_BY_HLT:
            printf(ANSI_COLOR_GREEN "Stop by HLT: SUCCESS\n" ANSI_COLOR_RESET);
            break;

        case SUCCESS:
            printf(ANSI_COLOR_GREEN "SUCSESS\n" ANSI_COLOR_RESET);
            break;

        case ERROR:
            break;

        default:
            break;
    }
}