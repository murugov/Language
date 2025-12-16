#include "compile.h"
#include "colors.h"


void AsmErrPrint(char *SourceFile, char *ByteCode, AsmErr_t verd)
{
    switch (verd)
    {
        case BAD_INPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, SourceFile);
            break;

        case BAD_OUTPUT_FILE_PTR:
            printf(ANSI_COLOR_RED "Bad pointer to %s!" ANSI_COLOR_RESET, ByteCode);
            break;

        case BAD_BUFFER_PTR:
            perror(ANSI_COLOR_RED "Bad buffer pointer!\n" ANSI_COLOR_RESET);
            break;

        case BAD_ARR_CMD_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array arr_cmd!\n" ANSI_COLOR_RESET);
            break;

        case BAD_CODE_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array code!\n" ANSI_COLOR_RESET);
            break;

        case BAD_ARR_LABELS_PTR:
            perror(ANSI_COLOR_RED "Bad pointer to array arr_labels!\n" ANSI_COLOR_RESET);
            break;

        case WRONG_FILE_SIZE:
            perror(ANSI_COLOR_RED "Error getting file size!\n" ANSI_COLOR_RESET);
            break;

        case CTOR_FAIL:
            perror(ANSI_COLOR_RED "Error creating array arr_cmd!\n" ANSI_COLOR_RESET);
            break;

        case CMD_NUM_FAIL:
            perror(ANSI_COLOR_RED "Incorrect number of commands!\n" ANSI_COLOR_RESET);
            break;

        case CMD_WITH_ARG_FAIL:
            perror(ANSI_COLOR_RED "Error filling array code in function CmdWithArg!\n" ANSI_COLOR_RESET);
            break;

        case CMD_WITHOUT_ARG_FAIL:
            perror(ANSI_COLOR_RED "Error filling array code in function CmdWithoutArg!\n" ANSI_COLOR_RESET);
            break;

        case LINE_SIZE_EXCEED:
            printf(ANSI_COLOR_RED "Line size exceeded in %s!\n" ANSI_COLOR_RESET, SourceFile);
            break;

        case REG_NEX:
            perror(ANSI_COLOR_RED "Non-existent register!\n" ANSI_COLOR_RESET);
            break; 

        case MEM_NEX:
            perror(ANSI_COLOR_RED "Non-existent memory block!\n" ANSI_COLOR_RESET);
            break; 

        case ARG_NEX:
            perror(ANSI_COLOR_RED "Non-existent argument!\n" ANSI_COLOR_RESET);
            break; 

        case UNKNOWN_CMD:
            printf(ANSI_COLOR_RED "Unknown command in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
            break;

        case UNKNOWN_LABEL:
            printf(ANSI_COLOR_RED "Unknown label in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
            break;

        case RE_LABEL:
            printf(ANSI_COLOR_RED "Repeated label in %s!\n" ANSI_COLOR_RESET, SourceFile); // return line with error
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