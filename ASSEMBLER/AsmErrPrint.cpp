#include "assembler.hpp"


void AsmErrPrint(char *SourceFile, char *ByteCode, amErr_t verd)
{
    switch (verd)
    {
        case ASM_BAD_INPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, SourceFile);
            break;

        case ASM_BAD_OUTPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, ByteCode);
            break;

        case ASM_BAD_BUFFER_PTR:
            perror(ANSI_COLOR_RED "Bad buffer pointer!\n" ANSI_COLOR_RESET);
            break;

        case ASM_BAD_ARR_CMD_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array arr_cmd!\n" ANSI_COLOR_RESET);
            break;

        case ASM_BAD_CODE_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array code!\n" ANSI_COLOR_RESET);
            break;

        case ASM_BAD_ARR_LABELS_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array arr_labels!\n" ANSI_COLOR_RESET);
            break;

        case ASM_WRONG_FILE_SIZE:
            perror(ANSI_COLOR_RED "Error getting file size!\n" ANSI_COLOR_RESET);
            break;

        case ASM_CTOR_FAIL:
            perror(ANSI_COLOR_RED "Error creating array arr_cmd!\n" ANSI_COLOR_RESET);
            break;

        case ASM_CMD_NUM_FAIL:
            perror(ANSI_COLOR_RED "Incorrect number of commands!\n" ANSI_COLOR_RESET);
            break;

        case ASM_CMD_WITH_ARG_FAIL:
            perror(ANSI_COLOR_RED "Error filling array code in function CmdWithArg!\n" ANSI_COLOR_RESET);
            break;

        case ASM_CMD_WITHOUT_ARG_FAIL:
            perror(ANSI_COLOR_RED "Error filling array code in function CmdWithoutArg!\n" ANSI_COLOR_RESET);
            break;

        case ASM_LINE_SIZE_EXCEED:
            printf(ANSI_COLOR_RED "Line size exceeded in %s!\n" ANSI_COLOR_RESET, SourceFile);
            break;

        case ASM_REG_NEX:
            perror(ANSI_COLOR_RED "Non-existent register!\n" ANSI_COLOR_RESET);
            break; 

        case ASM_MEM_NEX:
            perror(ANSI_COLOR_RED "Non-existent memory block!\n" ANSI_COLOR_RESET);
            break; 

        case ASM_UNKNOWN_CMD:
            printf(ANSI_COLOR_RED "Unknown command in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
            break;

        case ASM_UNKNOWN_LABEL:
            printf(ANSI_COLOR_RED "Unknown label in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
            break;

        case ASM_RE_LABEL:
            printf(ANSI_COLOR_RED "Repeated label in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
            break;

        case ASM_SUCCESS:
            printf(ANSI_COLOR_GREEN "SUCSESS\n" ANSI_COLOR_RESET);
            break;

        case ASM_ERROR:
            break;

        default:
            break;
    }
}