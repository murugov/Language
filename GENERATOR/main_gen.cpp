#include "generator.hpp"

// есть проблема со scanf для ';', '[', ']' 


hash_t bad_hash[] = {  
                        GetHash("("),      GetHash(")"),
                        GetHash("["),      GetHash("]"),
                        GetHash("{"),      GetHash("}"),
                        GetHash(","),      GetHash(";"),
                        GetHash(":"),      GetHash("="),
                        GetHash("=="),     GetHash("!="),
                        GetHash("<"),      GetHash(">"),
                        GetHash("<="),     GetHash(">="),
                        GetHash("def"),    GetHash("init"),
                        GetHash("if"),     GetHash("else"),
                        GetHash("while"),  GetHash("for"),
                        GetHash("return"), GetHash("EOF"),
                        GetHash("UNDEF")
                    };

static const size_t NUM_BAD_SYMS = sizeof(bad_hash) / sizeof(bad_hash[0]);


int main()
{
    FILE *SourceKeyFile  = fopen(PATH_TO_SRC_KEY_FILE, "r");
    if (IS_BAD_PTR(SourceKeyFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_SRC_KEY_FILE); return EXIT_FAILURE; }

    FILE *HashOpFile     = fopen(PATH_TO_HASH_OP_FILE, "w");
    if (IS_BAD_PTR(HashOpFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_HASH_OP_FILE); return EXIT_FAILURE; }
    
    FILE *OpInstrSetFile = fopen(PATH_TO_OP_INSTR_SET, "w");
    if (IS_BAD_PTR(OpInstrSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_OP_INSTR_SET); return EXIT_FAILURE; }

    FILE *KeywordSetFile = fopen(PATH_TO_KEYWORD_SET, "w");
    if (IS_BAD_PTR(KeywordSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_KEYWORD_SET); return EXIT_FAILURE; }

    char* buffer = NULL;
    size_t len_buffer = 0;
    int count_line = 0;
    char **arr_ptr = TXTreader(SourceKeyFile, buffer, &len_buffer, &count_line, NULL);
    if (IS_BAD_PTR(arr_ptr)) printf(ANSI_COLOR_RED "Error reading %s!\n" ANSI_COLOR_RESET, PATH_TO_SRC_KEY_FILE);

    RemoveComments(arr_ptr, &count_line);

    if (GenHashOp(HashOpFile, arr_ptr, count_line))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_HASH_OP_FILE);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);

    if (GenOpInstrSet(OpInstrSetFile, arr_ptr, count_line))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_OP_INSTR_SET);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);

    if (GenKeywordSet(KeywordSetFile, arr_ptr, count_line))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_KEYWORD_SET);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);

    FreeLines(arr_ptr, count_line);
    fclose(SourceKeyFile);
    fclose(HashOpFile);
    fclose(OpInstrSetFile);
    fclose(KeywordSetFile);



    FILE *SourceSpuFile = fopen(PATH_TO_SRC_SPU_FILE, "r");
    if (IS_BAD_PTR(OpInstrSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_SRC_SPU_FILE); return EXIT_FAILURE; }

    FILE *CmdEnumsFile = fopen(PATH_TO_CMD_CODES_FILE, "w");
    if (IS_BAD_PTR(OpInstrSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_CMD_CODES_FILE); return EXIT_FAILURE; }

    FILE *AsmInstrSetFile = fopen(PATH_TO_ASM_INSTR_SET, "w");
    if (IS_BAD_PTR(OpInstrSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_ASM_INSTR_SET); return EXIT_FAILURE; }

    FILE *SpuInstrSetFile = fopen(PATH_TO_SPU_INSTR_SET, "w");
    if (IS_BAD_PTR(OpInstrSetFile)) { printf(ANSI_COLOR_RED "Bad pointer %s!\n" ANSI_COLOR_RESET, PATH_TO_SPU_INSTR_SET); return EXIT_FAILURE; }

    char* buffer_2 = NULL;
    size_t len_buffer_2 = 0;
    int count_line_2 = 0;
    char **arr_ptr_2 = TXTreader(SourceSpuFile, buffer_2, &len_buffer_2, &count_line_2, NULL);
    if (IS_BAD_PTR(arr_ptr_2)) printf(ANSI_COLOR_RED "Error reading %s!\n" ANSI_COLOR_RESET, PATH_TO_SRC_SPU_FILE);

    RemoveComments(arr_ptr_2, &count_line_2);

    if (GenCmdEnum(CmdEnumsFile, arr_ptr_2, (size_t)count_line_2))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_CMD_CODES_FILE);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);
    if (GenAsmInstrSet(AsmInstrSetFile, arr_ptr_2, (size_t)count_line_2))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_ASM_INSTR_SET);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);
    if (GenSpuInstrSet(SpuInstrSetFile, arr_ptr_2, (size_t)count_line_2))
        printf(ANSI_COLOR_RED "Error creating %s!\n" ANSI_COLOR_RESET, PATH_TO_SPU_INSTR_SET);
    else
        printf(ANSI_COLOR_GREEN "SUCCESS\n" ANSI_COLOR_RESET);


    FreeLines(arr_ptr_2, count_line_2);
    fclose(SourceSpuFile);
    fclose(AsmInstrSetFile);
    fclose(SpuInstrSetFile);

    return 0;
}


GenErr_t GenHashOp(FILE *HashOpFile, char **arr_ptr, int count_line)
{
    if (IS_BAD_PTR(HashOpFile) || IS_BAD_PTR(arr_ptr) || count_line < 0) return GEN_ERROR;
        
    fprintf(HashOpFile, "// Automatically generated by Andrey Murugov's code-generator. Do not edit!!!\n\n");
    
    qsort(bad_hash, NUM_BAD_SYMS, sizeof(hash_t), CmpByHash);

    funcInfo *func_infos = (funcInfo*)calloc((size_t)count_line, sizeof(funcInfo));
    if (IS_BAD_PTR(func_infos)) return GEN_ERROR;

    int actual_count = 0;
    int max_name_len = 0;

    for (int line = 0; line < count_line; ++line)
    {
        if (*arr_ptr[line] == '\0') continue;
        
        int name_len = 0;
        if (sscanf(arr_ptr[line], "%15[^:]%n: \"%15[^\"]\";", func_infos[actual_count].name, &name_len, func_infos[actual_count].op) == 2)
        {
            if (strlen(func_infos[actual_count].op) == 1)
                func_infos[actual_count].hash = (hash_t)*func_infos[actual_count].op;
            else
                func_infos[actual_count].hash = GetHash(func_infos[actual_count].op);

            if (name_len > max_name_len)
                max_name_len = name_len;
            actual_count++;
        }
        else
        {
            printf(ANSI_COLOR_RED "Error parsing line %d: '%s'\n" ANSI_COLOR_RESET, line, arr_ptr[line]);
            free(func_infos);
            return GEN_ERROR;
        }
    }

    fprintf(HashOpFile, "#ifndef HASH_OP_HPP\n"
                        "#define HASH_OP_HPP\n\n\n");

    fprintf(HashOpFile, "enum HashOp\n");
    fprintf(HashOpFile, "{\n");

    for (int i = 0; i < actual_count - 1; ++i)
        fprintf(HashOpFile, "\tHASH_%-*s = 0x%lX,\n", max_name_len, func_infos[i].name, func_infos[i].hash);
    
    if (actual_count > 0)
        fprintf(HashOpFile, "\tHASH_%-*s = 0x%lX\n", max_name_len, func_infos[actual_count - 1].name, func_infos[actual_count - 1].hash);
    

    fprintf(HashOpFile, "};\n\n");

    fprintf(HashOpFile, "struct calc_context\n");
    fprintf(HashOpFile, "{\n");
    fprintf(HashOpFile, "    double left_val;\n");
    fprintf(HashOpFile, "    double right_val;\n");
    fprintf(HashOpFile, "};\n\n");
    
    fprintf(HashOpFile, "struct diff_context\n");
    fprintf(HashOpFile, "{\n");
    fprintf(HashOpFile, "    node_t *node_left;\n");
    fprintf(HashOpFile, "    node_t *node_right;\n");
    fprintf(HashOpFile, "    hash_t hash_indep_var;\n");
    fprintf(HashOpFile, "};\n\n\n");
    
    for (int i = 0; i < actual_count; ++i)
    {
        hash_t target_hash = GetHash(func_infos[i].op);
        if (!bsearch(&target_hash, bad_hash, NUM_BAD_SYMS, sizeof(hash_t), CmpByHash))
        {
            fprintf(HashOpFile, "double calc%s(calc_context *calc_params);\n", 
                    func_infos[i].name);
            fprintf(HashOpFile, "node_t *diff%s(diff_context *diff_params);\n\n", 
                    func_infos[i].name);
        }
    }

    fprintf(HashOpFile, "typedef double (*calc_t)(calc_context *calc_params);\n");
    fprintf(HashOpFile, "typedef node_t *(*diff_t)(diff_context *diff_params);\n\n\n");
    
    fprintf(HashOpFile, "#endif\n");

    free(func_infos);
    return GEN_SUCCESS;
}


GenErr_t GenOpInstrSet(FILE *OpInstrSetFile, char **arr_ptr, int count_line)
{
    if (IS_BAD_PTR(OpInstrSetFile) || IS_BAD_PTR(arr_ptr) || count_line < 0) return GEN_ERROR;
        
    fprintf(OpInstrSetFile, "// Automatically generated by Andrey Murugov's code-generator. Do not edit!!!\n\n");
    
    qsort(bad_hash, NUM_BAD_SYMS, sizeof(hash_t), CmpByHash);

    fprintf(OpInstrSetFile, "const op_t op_instr_set[] =\n");
    fprintf(OpInstrSetFile, "{\n");

    static const hash_t HASH_LOG   = GetHash("LOG");
    static const hash_t HASH_PLUS  = GetHash("+");
    static const hash_t HASH_MINUS = GetHash("-");
    static const hash_t HASH_MUL   = GetHash("*");
    static const hash_t HASH_DIV   = GetHash("/");
    static const hash_t HASH_POW   = GetHash("^");
    
    op_instr_t *func_infos = (op_instr_t*)calloc((size_t)count_line, sizeof(op_instr_t));
    if (IS_BAD_PTR(func_infos)) return GEN_ERROR;

    int actual_count = 0;
    size_t max_name_len = 0;
    size_t max_op_len = 0;
    int max_num_width = 0;

    for (int line = 0; line < count_line; ++line)
    {
        if (*arr_ptr[line] == '\0') 
            continue;
        
        if (sscanf(arr_ptr[line], "%15[^:]: \"%15[^\"]\";", 
                   func_infos[actual_count].name, 
                   func_infos[actual_count].op) == 2)
        {
            hash_t target_hash = GetHash(func_infos[actual_count].op);
            if (bsearch(&target_hash, bad_hash, NUM_BAD_SYMS, sizeof(hash_t), CmpByHash)) continue;
            
            size_t name_len = strlen(func_infos[actual_count].name);
            size_t op_len   = strlen(func_infos[actual_count].op);
            
            if (name_len > max_name_len) max_name_len = name_len;
            if (op_len > max_op_len) max_op_len = op_len;
            
            func_infos[actual_count].hash = target_hash;
            
            hash_t op_hash = func_infos[actual_count].hash;
            hash_t name_hash = GetHash(func_infos[actual_count].name);
            
            if (name_hash == HASH_LOG || op_hash == HASH_PLUS || 
                op_hash == HASH_MINUS || op_hash == HASH_MUL || 
                op_hash == HASH_DIV || op_hash == HASH_POW)
            {
                func_infos[actual_count].num_args = 2;
            }
            else
            {
                func_infos[actual_count].num_args = 1;
            }
            
            if (func_infos[actual_count].num_args > max_num_width)
                max_num_width = func_infos[actual_count].num_args;
            
            actual_count++;
        }
    }
    
    qsort(func_infos, (size_t)actual_count, sizeof(op_instr_t), CmpOpInstrSetByHash);

    for (int i = 0; i < actual_count - 1; ++i)
    {
        fprintf(OpInstrSetFile, 
                "\t{HASH_%-*s, \"%s\",%*d, calc%s, diff%s},\n",
                (int)max_name_len, func_infos[i].name, 
                func_infos[i].op,
                max_num_width + 1, 
                func_infos[i].num_args,
                func_infos[i].name, 
                func_infos[i].name);
    }
    
    if (actual_count > 0)
    {
        fprintf(OpInstrSetFile,
                "\t{HASH_%-*s, \"%s\",%*d, calc%s, diff%s}\n",
                (int)max_name_len, func_infos[actual_count - 1].name,
                func_infos[actual_count - 1].op,
                max_num_width + 1,
                func_infos[actual_count - 1].num_args,
                func_infos[actual_count - 1].name,
                func_infos[actual_count - 1].name);
    }

    fprintf(OpInstrSetFile, "};\n\n");
    
    fprintf(OpInstrSetFile, "#define LEN_INSTR_SET sizeof(op_instr_set) / sizeof(*op_instr_set)\n");

    free(func_infos);
    return GEN_SUCCESS;
}


GenErr_t GenKeywordSet(FILE *KeywordSetFile, char **arr_ptr, int count_line)
{
    if (IS_BAD_PTR(KeywordSetFile) || IS_BAD_PTR(arr_ptr) || count_line < 0) return GEN_ERROR;
        
    fprintf(KeywordSetFile, "// Automatically generated by Andrey Murugov's code-generator. Do not edit!!!\n\n");
    
    qsort(bad_hash, NUM_BAD_SYMS, sizeof(hash_t), CmpByHash);

    fprintf(KeywordSetFile, "const keyword_t keyword_set[] =\n");
    fprintf(KeywordSetFile, "{\n");
    
    keyword_set_t *func_infos = (keyword_set_t*)calloc((size_t)count_line, sizeof(keyword_set_t));
    if (IS_BAD_PTR(func_infos)) return GEN_ERROR;

    int actual_count    = 0;
    size_t max_name_len = 0;
    size_t max_key_len  = 0;

    for (int line = 0; line < count_line; ++line)
    {
        if (*arr_ptr[line] == '\0') 
            continue;
        
        if (sscanf(arr_ptr[line], "%15[^:]: \"%15[^\"]\";", 
                   func_infos[actual_count].name, 
                   func_infos[actual_count].key) == 2)
        {
            size_t name_len = strlen(func_infos[actual_count].name);
            size_t key_len  = strlen(func_infos[actual_count].key);
            
            if (name_len > max_name_len) max_name_len = name_len;
            if (key_len > max_key_len) max_key_len = key_len;
            
            if (key_len == 1)
                func_infos[actual_count].hash = (hash_t)(*func_infos[actual_count].key);
            else
                func_infos[actual_count].hash = GetHash(func_infos[actual_count].key);

            actual_count++;
        }
    }
    
    qsort(func_infos, (size_t)actual_count, sizeof(keyword_set_t), CmpKeywordSetByHash);

    for (int i = 0; i < actual_count - 1; ++i)
    {
        fprintf(KeywordSetFile, 
                "\t{ARG_OP, \"%s\",\t%zu ,\t0x%zX},\n",
                func_infos[i].key,
                strlen(func_infos[i].key),
                func_infos[i].hash);
    }
    
    if (actual_count > 0)
    {
        fprintf(KeywordSetFile, 
                "\t{ARG_OP, \"%s\",\t%zu ,\t0x%zX}\n",
                func_infos[actual_count - 1].key,
                strlen(func_infos[actual_count - 1].key),
                func_infos[actual_count - 1].hash);
    }

    fprintf(KeywordSetFile, "};\n\n");
    
    fprintf(KeywordSetFile, "#define LEN_KEYWORD_SET sizeof(keyword_set) / sizeof(*keyword_set)\n");

    free(func_infos);
    return GEN_SUCCESS;
}


GenErr_t GenCmdEnum(FILE *CmdEnumsFile, char **arr_ptr, size_t count_line)
{
    if (IS_BAD_PTR(CmdEnumsFile) || IS_BAD_PTR(arr_ptr)) return GEN_ERROR;
        
    fprintf(CmdEnumsFile, "// Automatically generated by Andrey Murugov's code-generator. Do not edit!!!\n\n\n");

    unsigned int max_num_cmd = 0;
    unsigned int min_num_cmd = 32;

    char *cmd = (char*)calloc(MAX_LEN_STR_FOR_HASH, sizeof(char));
    if (IS_BAD_PTR(cmd)) return GEN_ERROR;

    unsigned char num_cmd   = 0;
    unsigned char count_arg = 0;

    fprintf(CmdEnumsFile, "enum CmdCodes\n");
    fprintf(CmdEnumsFile, "{\n");
    for (size_t line = 0; line < count_line - 1; ++line)
    {
        if (sscanf(arr_ptr[line], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
        {
            fprintf(CmdEnumsFile, "\tCMD_%s%*s = 0x%02X,\n", cmd, MAX_LEN_STR_FOR_HASH - (int)strlen(cmd), "", num_cmd);
            max_num_cmd = (num_cmd > max_num_cmd) ? num_cmd: max_num_cmd;
            min_num_cmd = (num_cmd < min_num_cmd) ? num_cmd: min_num_cmd;
        }
        else
            return GEN_ERROR;
    }
    if (sscanf(arr_ptr[count_line - 1], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
    {
        fprintf(CmdEnumsFile, "\tCMD_%s%*s = 0x%02X\n", cmd, MAX_LEN_STR_FOR_HASH - (int)strlen(cmd), "", num_cmd);
        max_num_cmd = (num_cmd > max_num_cmd) ? num_cmd: max_num_cmd;
        min_num_cmd = (num_cmd < min_num_cmd) ? num_cmd: min_num_cmd;
    }
    else
    {
        return GEN_ERROR;
    }

    fprintf(CmdEnumsFile, "};\n\n");

    fprintf(CmdEnumsFile, "#define MAX_NUM_CMD %u\n", max_num_cmd);
    fprintf(CmdEnumsFile, "#define MIN_NUM_CMD %u\n", min_num_cmd);
    
    free(cmd);
    return GEN_SUCCESS;
}


GenErr_t GenAsmInstrSet(FILE *AsmInstrSetFile, char **arr_ptr, size_t count_line)
{
    if (IS_BAD_PTR(AsmInstrSetFile) || IS_BAD_PTR(arr_ptr)) return GEN_ERROR;

    asm_instr_t *arr_instr = (asm_instr_t*)calloc(count_line, sizeof(asm_instr_t));
    if (IS_BAD_PTR(arr_instr)) return GEN_ERROR;
        
    char *cmd = (char*)calloc(MAX_LEN_STR_FOR_HASH, sizeof(char));
    if (IS_BAD_PTR(cmd)) { free(arr_instr); return GEN_ERROR; }

    unsigned char num_cmd = 0;
    unsigned char count_arg = 0;

    for (size_t line = 0; line < count_line - 1; ++line)
    {
        if (sscanf(arr_ptr[line], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
        {
            arr_instr[line].count = count_arg;
            arr_instr[line].hash = GetHash(cmd);
            arr_instr[line].cmd = strdup(cmd);
        }
        else
            return GEN_ERROR;
    }
    if (sscanf(arr_ptr[count_line - 1], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
    {
        arr_instr[count_line - 1].count = count_arg;
        arr_instr[count_line - 1].hash = GetHash(cmd);
        arr_instr[count_line - 1].cmd = strdup(cmd);
    }
    else
        return GEN_ERROR;

    qsort(arr_instr, count_line, sizeof(asm_instr_t), CmpAsmInstrSetByHash);


    fprintf(AsmInstrSetFile, "// Automatically generated by Andrey Murugov's code-generator. Do not edit!!!\n\n\n");

    fprintf(AsmInstrSetFile, "const WrapCmd asm_instr_set[] =\n");
    fprintf(AsmInstrSetFile, "{\n");
    for (size_t line = 0; line < count_line - 1; ++line)
    {
        cmd = arr_instr[line].cmd;
        unsigned int len_cmd_spec = (arr_instr[line].count > 1) ? strlen("ReadOpcode16"): strlen("ReadOpcode8");
        fprintf(AsmInstrSetFile, "\t{%s,%*s %hhu,\t 0x%02X,%*s CMD_%s},\n", \
            (arr_instr[line].count > 1) ? "ReadOpcode16": "ReadOpcode8", \
            MAX_LEN_STR_FOR_HASH - len_cmd_spec, "", arr_instr[line].count, \
            (unsigned int)GetHash(cmd), MAX_LEN_STR_FOR_HASH  - (int)strlen(cmd), "", cmd);
    }
    cmd = arr_instr[count_line - 1].cmd;
    unsigned int len_cmd_spec = (arr_instr[count_line - 1].count > 1) ? strlen("ReadOpcode16"): strlen("ReadOpcode8");
    fprintf(AsmInstrSetFile, "\t{%s,%*s %hhu,\t 0x%02X,%*s CMD_%s}\n", \
        (arr_instr[count_line - 1].count > 1) ? "ReadOpcode16": "ReadOpcode8", \
        MAX_LEN_STR_FOR_HASH - len_cmd_spec, "", arr_instr[count_line - 1].count, \
        (unsigned int)GetHash(cmd), MAX_LEN_STR_FOR_HASH  - (int)strlen(cmd), "", cmd);
    fprintf(AsmInstrSetFile, "};\n\n");

    fprintf(AsmInstrSetFile, "#define LEN_INSTR_SET sizeof(asm_instr_set) / sizeof(*asm_instr_set)");

    free(cmd);
    free(arr_instr);
    return GEN_SUCCESS;
}


GenErr_t GenSpuInstrSet(FILE *SpuInstrSetFile, char **arr_ptr, size_t count_line)
{
    if (IS_BAD_PTR(SpuInstrSetFile) || IS_BAD_PTR(arr_ptr)) return GEN_ERROR;

    spu_instr_t *arr_instr = (spu_instr_t*)calloc(count_line, sizeof(spu_instr_t));
    if (IS_BAD_PTR(arr_instr)) return GEN_ERROR;
        
    char *cmd = (char*)calloc(MAX_LEN_STR_FOR_HASH, sizeof(char));
    if (IS_BAD_PTR(cmd)) return GEN_ERROR;

    unsigned char num_cmd   = 0;
    unsigned char count_arg = 0;

    for (size_t line = 0; line < count_line - 1; ++line)
    {
        if (sscanf(arr_ptr[line], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
        {
            arr_instr[line].count = count_arg;
            arr_instr[line].num = num_cmd;
            arr_instr[line].cmd = strdup(cmd);
        }
        else
            return GEN_ERROR;
    }
    if (sscanf(arr_ptr[count_line - 1], "%s %hhu %hhu", cmd, &num_cmd, &count_arg) == 3)
    {
        arr_instr[count_line - 1].count = count_arg;
        arr_instr[count_line - 1].num = num_cmd;
        arr_instr[count_line - 1].cmd = strdup(cmd);
    }
    else
        return GEN_ERROR;

    qsort(arr_instr, count_line, sizeof(spu_instr_t), CmpSpuInstrSetByNum);
        
    fprintf(SpuInstrSetFile, "// Automatically generated by Andrey Murugov's code generator. Do not edit!!!\n\n\n");

    fprintf(SpuInstrSetFile, "const WrapCmd spu_instr_set[] =\n");
    fprintf(SpuInstrSetFile, "{\n");
    for (size_t line = 0; line < count_line - 1; ++line)
    {
        cmd = arr_instr[line].cmd;
        count_arg = arr_instr[line].count;
        fprintf(SpuInstrSetFile, "\t{FUNC_CMD_%s,%*s %u,\t CMD_%s},\n", cmd, MAX_LEN_STR_FOR_HASH - (int)strlen(cmd), "", count_arg, cmd);
    }
    count_arg = arr_instr[count_line - 1].count;
    cmd = arr_instr[count_line - 1].cmd;
    fprintf(SpuInstrSetFile, "\t{FUNC_CMD_%s,%*s %u,\t CMD_%s}\n", cmd, MAX_LEN_STR_FOR_HASH - (int)strlen(cmd), "", count_arg, cmd);
    fprintf(SpuInstrSetFile, "};\n\n");

    fprintf(SpuInstrSetFile, "#define LEN_INSTR_SET sizeof(spu_instr_set) / sizeof(*spu_instr_set)");

    free(cmd);
    free(arr_instr);
    return GEN_SUCCESS;
}


int CmpOpInstrSetByHash(const void *a, const void *b)
{
    const op_instr_t *op_instr_a = (const op_instr_t*)a;
    const op_instr_t *op_instr_b = (const op_instr_t*)b;

    if (op_instr_a->hash > op_instr_b->hash)
        return 1;
    if (op_instr_a->hash == op_instr_b->hash)
        return 0;
    return -1;
}


int CmpKeywordSetByHash(const void *a, const void *b)
{
    const keyword_set_t *keyword_a = (const keyword_set_t*)a;
    const keyword_set_t *keyword_b = (const keyword_set_t*)b;

    if (keyword_a->hash > keyword_b->hash)
        return 1;
    if (keyword_a->hash == keyword_b->hash)
        return 0;
    return -1;
}


int CmpByHash(const void *a, const void *b)
{
    const hash_t *hash_a = (const hash_t*)a;
    const hash_t *hash_b = (const hash_t*)b;

    if (*hash_a > *hash_b)
        return 1;
    if (*hash_a == *hash_b)
        return 0;
    return -1;
}


int CmpAsmInstrSetByHash(const void *a, const void *b)
{
    const asm_instr_t *arr_instr_a = (const asm_instr_t*)a;
    const asm_instr_t *arr_instr_b = (const asm_instr_t*)b;

    if (arr_instr_a->hash > arr_instr_b->hash)
        return 1;
    if (arr_instr_a->hash == arr_instr_b->hash)
        return 0;
    return -1;
}


int CmpSpuInstrSetByNum(const void *a, const void *b)
{
    const spu_instr_t *arr_instr_a = (const spu_instr_t*)a;
    const spu_instr_t *arr_instr_b = (const spu_instr_t*)b;

    if (arr_instr_a->num > arr_instr_b->num)
        return 1;
    if (arr_instr_a->num == arr_instr_b->num)
        return 0;
    return -1;
}


void RemoveComments(char** arr_ptr, int *count_line)
{
    for (int i = 0; i < *count_line; ++i)
    {
        char* colon = strchr(arr_ptr[i], '#');

        if (colon != NULL)
            *colon = '\0';
    }
}