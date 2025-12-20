#include "translator.hpp"

static int condition_count = 0;
static int if_count = 0;
static int while_count = 0;

FILE *ReportFile = NULL;

static const char* ntElemToStr(const void* elem_ptr)
{
    const ntElem_t *elem = (const ntElem_t*)elem_ptr;
    if (IS_BAD_PTR(elem)) return NULL;
    
    return elem->name;
}


backErr_t transCtor(trans_t *trans, const char *src)
{
    ON_DEBUG( if (IS_BAD_PTR(trans) || IS_BAD_PTR(src)) { return BACK_ERROR; })

    FILE *SourceFile = fopen(src, "r");
    if (IS_BAD_PTR(SourceFile))
    { 
        printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, src); 
        return BACK_ERROR; 
    }

    char *buffer = DataReader(SourceFile);
    trans->node = NodeReader(&buffer, NULL);
    ON_DEBUG( if (IS_BAD_PTR(trans->node)) return BACK_ERROR;)
    
    trans->name_tables = (stk_t<ht_t<ntElem_t*>*>*)calloc(1, sizeof(stk_t<ht_t<ntElem_t*>*>));
    if (IS_BAD_PTR(trans->name_tables)) { fclose(SourceFile); return BACK_ERROR; }
    
    STACK_CTOR(trans->name_tables, MIN_STK_LEN);
    
    ht_t<ntElem_t*>* global_table = (ht_t<ntElem_t*>*)calloc(1, sizeof(ht_t<ntElem_t*>));
    if (IS_BAD_PTR(global_table))
    {
        STACK_DTOR(trans->name_tables);
        free(trans->name_tables);
        fclose(SourceFile);
        return BACK_ERROR;
    }
    
    HT_CTOR(global_table);
    
    StackPush(trans->name_tables, global_table);
    trans->cur_name_table = 0;
    
    trans->global_offset = GLOBAL_OFFSET;
    
    fclose(SourceFile);
    return BACK_SUCCESS;
}

backErr_t transDtor(trans_t *trans)
{
    ON_DEBUG( if (IS_BAD_PTR(trans)) { return BACK_ERROR; })

    if (!IS_BAD_PTR(ReportFile)) { fclose(ReportFile); ReportFile = NULL; }

    if (!IS_BAD_PTR(trans->name_tables))
    {
        for (int i = 0; i < trans->name_tables->size; ++i)
        {
            if (trans->name_tables->data[i])
            {
                ht_t<ntElem_t*>* table = trans->name_tables->data[i];
                
                for (int j = 0; j < HT_SIZE; ++j)
                {
                    if (table->table[j].is_used && !IS_BAD_PTR(table->table[j].stk))
                    {
                        for (ssize_t k = 0; k < table->table[j].stk->size; ++k)
                        {
                            ntElem_t* elem = table->table[j].stk->data[k];
                            if (!IS_BAD_PTR(elem))
                            {
                                free((void*)elem->name);
                                free(elem);
                            }
                        }
                    }
                }
                
                HT_DTOR(table);                
                free(table);
            }
        }
        
        STACK_DTOR(trans->name_tables);
        free(trans->name_tables);
        trans->name_tables = NULL;
    }
    
    trans->global_offset = 0;
    trans->cur_name_table = 0;
    
    return BACK_SUCCESS;
}


static backErr_t enterScope(trans_t *trans)
{
    if (IS_BAD_PTR(trans)) { return BACK_ERROR; }
    
    ht_t<ntElem_t*>* new_table = (ht_t<ntElem_t*>*)calloc(1, sizeof(ht_t<ntElem_t*>));
    if (IS_BAD_PTR(new_table)) { return BACK_ERROR; }
    
    HT_CTOR(new_table);
    
    StackPush(trans->name_tables, new_table);
    trans->cur_name_table = trans->name_tables->size - 1;
    
    return BACK_SUCCESS;
}


static backErr_t exitScope(trans_t *trans)
{
    if (IS_BAD_PTR(trans) || IS_BAD_PTR(trans->name_tables) || trans->name_tables->size <= 1) { return BACK_ERROR; }
    
    if (!IS_BAD_PTR(CUR_NAME_TABLE)) 
    { 
        for (int i = 0; i < HT_SIZE; ++i)
        {
            if (CUR_NAME_TABLE->table[i].is_used && !IS_BAD_PTR(CUR_NAME_TABLE->table[i].stk))
            {
                for (ssize_t j = 0; j < CUR_NAME_TABLE->table[i].stk->size; ++j)
                {
                    ntElem_t* elem = CUR_NAME_TABLE->table[i].stk->data[j];
                    if (!IS_BAD_PTR(elem))
                    {
                        free((void*)elem->name);
                        free(elem);
                    }
                }
            }
        }
        
        HT_DTOR(CUR_NAME_TABLE); 
        free(CUR_NAME_TABLE); 
    }
    
    ht_t<ntElem_t*> *tmp = NULL;
    StackPop(trans->name_tables, &tmp);
    trans->cur_name_table = trans->name_tables->size - 1;
    
    return BACK_SUCCESS;
}


static ntElem_t* findVar(trans_t *trans, const char* var_name)
{
    if (IS_BAD_PTR(trans) || IS_BAD_PTR(var_name) || IS_BAD_PTR(trans->name_tables)) { return NULL; }
    
    for (int i = trans->cur_name_table; i >= 0; --i)
    {
        ht_t<ntElem_t*>* table = trans->name_tables->data[i];
        if (IS_BAD_PTR(table)) continue;
        
        ntElem_t search_elem = {var_name, ARG_VAR, 0, 0};
        ntElem_t *search_elem_ptr = &search_elem;
        ntElem_t* found = htFind(table, &search_elem_ptr, ntElemToStr);
        
        if (!IS_BAD_PTR(found)) { return found; }
    }
    
    return NULL;
}


static ntElem_t* addVar(trans_t *trans, const char* var_name, int type, int offset)
{
    if (IS_BAD_PTR(trans) || IS_BAD_PTR(var_name) || IS_BAD_PTR(trans->name_tables) || trans->name_tables->size == 0) { return NULL; }
    
    if (IS_BAD_PTR(CUR_NAME_TABLE)) { return NULL; }
    
    ntElem_t search_elem = {var_name, (type_t)type, 0, 0};
    ntElem_t *search_elem_ptr = &search_elem;
    if (!IS_BAD_PTR(htFind(CUR_NAME_TABLE, &search_elem_ptr, ntElemToStr))) { return NULL; }
    
    ntElem_t* new_var = (ntElem_t*)calloc(1, sizeof(ntElem_t));
    if (IS_BAD_PTR(new_var)) { return NULL; }
    
    new_var->name = strdup(var_name);
    new_var->type = (type_t)type;
    new_var->offset = offset;
    new_var->size_of_stk_frm = 1;
    
    if (htInsert(CUR_NAME_TABLE, &new_var, ntElemToStr) != HT_SUCCESS)
    {
        free((void*)new_var->name);
        free(new_var);
        return NULL;
    }
    
    return new_var;
}


backErr_t TranslateTree(trans_t *trans, const char* report)
{
    ON_DEBUG( if (IS_BAD_PTR(trans) || IS_BAD_PTR(report)) { return BACK_ERROR; })

    ReportFile = fopen(report, "w");
    if (IS_BAD_PTR(ReportFile))
    { 
        printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, report); 
        return BACK_ERROR; 
    }

    fprintf(ReportFile, "jmp main\n\n");

    if (CompileOnlyFunc(trans) != BACK_SUCCESS)
    { 
        fclose(ReportFile);
        return BACK_ERROR; 
    }
    
    fprintf(ReportFile, "\nmain:\n");
    
    // Пролог main
    fprintf(ReportFile, "push bp\n");
    fprintf(ReportFile, "pop [0] ; save old bp\n");
    fprintf(ReportFile, "push sp\n");
    fprintf(ReportFile, "pop bp ; set new bp for main\n");
    
    if (CompileNotFunc(trans) != BACK_SUCCESS)
    { 
        fclose(ReportFile);
        return BACK_ERROR; 
    }
    
    // Эпилог main
    fprintf(ReportFile, "\n; Epilogue of main\n");
    fprintf(ReportFile, "push [0]\n");
    fprintf(ReportFile, "pop bp ; restore old bp\n");
    fprintf(ReportFile, "ret\n");
        
    fprintf(ReportFile, "\nhlt\n");
    
    fclose(ReportFile);
    ReportFile = NULL;
    
    return BACK_SUCCESS;
}


backErr_t CompileOnlyFunc(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) return BACK_SUCCESS;
    
    if (TR_NODE_TYPE == ARG_OP && TR_NODE_HASH == HASH_DEF)
    {
        return TranslateFuncDef(trans);
    }
    
    if (TR_NODE_TYPE == ARG_LINK)
    {        
        node_t *saved_node = TR_NODE;
        backErr_t compile_verd = BACK_SUCCESS;
        
        if (!IS_BAD_PTR(TR_NODE_L))
        {
            TR_NODE = TR_NODE_L;
            compile_verd = CompileOnlyFunc(trans);
            if (compile_verd != BACK_SUCCESS) 
            { 
                TR_NODE = saved_node;
                return compile_verd; 
            }
        }
        
        if (!IS_BAD_PTR(TR_NODE_R))
        {
            TR_NODE = TR_NODE_R;
            compile_verd = CompileOnlyFunc(trans);
            if (compile_verd != BACK_SUCCESS) 
            { 
                TR_NODE = saved_node;
                return compile_verd; 
            }
        }

        TR_NODE = saved_node;
        return BACK_SUCCESS;
    }
    
    return BACK_SUCCESS;
}


backErr_t CompileNotFunc(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_SUCCESS; }
    
    if ((TR_NODE_TYPE == ARG_OP && TR_NODE_HASH == HASH_DEF) || TR_NODE_TYPE == ARG_FUNC) { return BACK_SUCCESS; }
        
    if (TR_NODE_TYPE == ARG_LINK)
    {        
        node_t *saved_node = TR_NODE;
        backErr_t compile_verd = BACK_SUCCESS;
        
        if (!IS_BAD_PTR(TR_NODE_L))
        {
            TR_NODE = TR_NODE_L;
            compile_verd = CompileNotFunc(trans);
            if (compile_verd != BACK_SUCCESS) 
            { 
                TR_NODE = saved_node;
                return compile_verd; 
            }
        }
        
        if (!IS_BAD_PTR(TR_NODE_R))
        {
            TR_NODE = TR_NODE_R;
            compile_verd = CompileNotFunc(trans);
            if (compile_verd != BACK_SUCCESS) 
            { 
                TR_NODE = saved_node;
                return compile_verd; 
            }
        }

        return compile_verd;
    }
    
    return TranslatePrimary(trans);
}


backErr_t TranslatePrimary(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) {return BACK_SUCCESS;}
    
    if (TR_NODE_TYPE == ARG_OP && TR_NODE_HASH == HASH_DEF) 
    { 
        return BACK_SUCCESS; 
    }
    
    node_t *saved_node = TR_NODE;
    backErr_t verd = BACK_SUCCESS;
    switch (TR_NODE_TYPE)
    {
        case ARG_LINK:
        {
            TR_NODE = TR_NODE_L;
            verd = TranslatePrimary(trans);
            TR_NODE = saved_node;
            if (verd != BACK_SUCCESS) { return verd; }
            
            TR_NODE = TR_NODE_R;
            verd = TranslatePrimary(trans);
            TR_NODE = saved_node;
            if (verd != BACK_SUCCESS) { return verd; }

            return BACK_SUCCESS;
        }

        case ARG_NUM:
            fprintf (ReportFile, "push %d\n", TR_NODE_NUM);
            return BACK_SUCCESS;

        case ARG_OP:
            return TranslateOp(trans);
            
        case ARG_VAR:
            return TranslateVar(trans);
            
        case ARG_FUNC:
            return TranslateFuncCall(trans);
            
        default:
            break;
    }

    return BACK_ERROR;
}


backErr_t TranslateOp(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    if (TR_NODE_HASH == HASH_DEF)
    { 
        return BACK_SUCCESS;
    }

    node_t *saved_node = TR_NODE;
    
    switch (TR_NODE_HASH)
    {
        case HASH_ADD:
        case HASH_SUB:
        case HASH_MUL:
        case HASH_DIV:
            return TranslateCalc(trans);
    
        case HASH_INIT:
            return TranslateVarInit(trans);

        case HASH_DEF:
            return TranslateFuncDef(trans);

        case HASH_EQ:
            return TranslateAssignment(trans);

        case HASH_GT:
        case HASH_LT:
        case HASH_GE:
        case HASH_LE:
        case HASH_EQEQ:
        case HASH_NE:
            return TranslateCondition(trans);

        case HASH_IF: 
            return TranslateIf(trans);
        case HASH_ELSE:
            return BACK_SUCCESS;

        case HASH_WHILE:
            return TranslateWhile(trans);
        case HASH_RETURN:
            return TranslateReturn(trans);
            
        case HASH_SEMICOLON:
        {
            backErr_t verd = BACK_SUCCESS;

            if (!IS_BAD_PTR(TR_NODE_L))
            { 
                TR_NODE = TR_NODE_L;
                verd = TranslatePrimary(trans); 
                TR_NODE = saved_node;
                if (verd != BACK_SUCCESS) return verd;
            }
            
            if (!IS_BAD_PTR(TR_NODE_R))
            { 
                TR_NODE = TR_NODE_R;
                verd = TranslatePrimary(trans); 
                TR_NODE = saved_node;
                if (verd != BACK_SUCCESS) return verd;
            }
            return BACK_SUCCESS;
        }

        default:
            break;
    }

    return BACK_ERROR;
}


backErr_t TranslateCalc(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_SUCCESS; }

    node_t *saved_node = TR_NODE;

    TR_NODE = TR_NODE_L;
    backErr_t res = TranslatePrimary(trans);
    if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
    TR_NODE = saved_node;
    
    TR_NODE = TR_NODE_R;
    res = TranslatePrimary(trans);
    if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
    TR_NODE = saved_node;

    switch (TR_NODE_HASH)
    {
        case HASH_ADD: 
            fprintf(ReportFile, "add\n");
            break;
        
        case HASH_SUB: 
            fprintf(ReportFile, "sub\n");
            break;

        case HASH_MUL: 
            fprintf(ReportFile, "mul\n");
            break;

        case HASH_DIV: 
            fprintf(ReportFile, "div\n");
            break;

        default:
            break;
    }

    return BACK_SUCCESS;
}


backErr_t TranslateCondition(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    condition_count++;

    node_t *saved_node = TR_NODE;

    TR_NODE = TR_NODE_L;
    backErr_t res = TranslatePrimary(trans);
    if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
    TR_NODE = saved_node;
    
    TR_NODE = TR_NODE_R;
    res = TranslatePrimary(trans);
    if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
    TR_NODE = saved_node;

    switch (TR_NODE_HASH)
    {
        case HASH_EQEQ: 
            fprintf(ReportFile, "je ");
            break;
        case HASH_NE:   
            fprintf(ReportFile, "jne ");
            break;
        case HASH_GT:
            fprintf(ReportFile, "jb ");
            break;
        case HASH_LT:
            fprintf(ReportFile, "ja ");
            break;
        case HASH_GE:
            fprintf(ReportFile, "jbe ");
            break;
        case HASH_LE:
            fprintf(ReportFile, "jae ");
            break;    
        default:
            return BACK_ERROR;
    }

    fprintf(ReportFile, "true%d\n", condition_count);
    
    fprintf(ReportFile, "push 0\n");
    fprintf(ReportFile, "jmp endcond%d\n", condition_count);
    
    fprintf(ReportFile, "true%d:\n", condition_count);
    fprintf(ReportFile, "push 1\n");
    
    fprintf(ReportFile, "endcond%d:\n", condition_count);

    return BACK_SUCCESS;
}


backErr_t TranslateIf(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    static int if_count = 0;
    if_count++;

    if (enterScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    node_t *saved_node = TR_NODE;

    if (!IS_BAD_PTR(TR_NODE_L))
    {
        TR_NODE = TR_NODE_L;
        backErr_t res = TranslateCondition(trans);
        if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
        TR_NODE = saved_node;
    }

    int has_else = 0;
    if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_OP && TR_NODE_R->item.op == HASH_ELSE)
    {
        has_else = 1;
    }

    if (has_else)
    {
        fprintf(ReportFile, "push 0\n");
        fprintf(ReportFile, "je else%d\n", if_count);

        if (!IS_BAD_PTR(TR_NODE_R->left))
        {
            TR_NODE = TR_NODE_R->left;
            backErr_t res = TranslatePrimary(trans);
            if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
            TR_NODE = saved_node;
        }

        fprintf(ReportFile, "jmp endif%d\n", if_count);
        fprintf(ReportFile, "else%d:\n", if_count);

        if (!IS_BAD_PTR(TR_NODE_R->right))
        {
            TR_NODE = TR_NODE_R->right;
            backErr_t res = TranslatePrimary(trans);
            if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
            TR_NODE = saved_node;
        }
    }
    else
    {
        fprintf(ReportFile, "push 0\n");
        fprintf(ReportFile, "je endif%d\n", if_count);

        if (!IS_BAD_PTR(TR_NODE_R))
        {
            TR_NODE = TR_NODE_R;
            backErr_t res = TranslatePrimary(trans);
            if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
            TR_NODE = saved_node;
        }
    }

    fprintf(ReportFile, "endif%d:\n", if_count);

    if (exitScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    TR_NODE = saved_node;
    return BACK_SUCCESS;
}


backErr_t TranslateWhile(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    while_count++;

    if (enterScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    node_t *saved_node = TR_NODE;

    fprintf(ReportFile, "startwhile%d:\n", while_count);
    
    if (TR_NODE_TYPE == ARG_OP && !IS_BAD_PTR(TR_NODE_L))
    {
        TR_NODE = TR_NODE_L;
        backErr_t res = TranslateCondition(trans);
        if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
        TR_NODE = saved_node;
    }

    fprintf(ReportFile, "push 0\n");
    fprintf(ReportFile, "je endwhile%d\n", while_count);

    if (TR_NODE_TYPE == ARG_OP && !IS_BAD_PTR(TR_NODE_R))
    {
        TR_NODE = TR_NODE_R;
        backErr_t res = TranslatePrimary(trans);
        if (res != BACK_SUCCESS) { TR_NODE = saved_node; return res; }
        TR_NODE = saved_node;
    }
    
    fprintf(ReportFile, "jmp startwhile%d\n", while_count);

    fprintf(ReportFile, "endwhile%d:\n", while_count);

    if (exitScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    return BACK_SUCCESS;
}


backErr_t TranslateReturn(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    if (TR_NODE_TYPE == ARG_OP && !IS_BAD_PTR(TR_NODE_L))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_L;
        backErr_t verd = TranslatePrimary(trans);
        TR_NODE = saved_node;
        if (verd != BACK_SUCCESS) return verd;
    }

    // Эпилог функции перед возвратом
    fprintf(ReportFile, "; Function epilogue\n");
    fprintf(ReportFile, "push [bp]\n");
    fprintf(ReportFile, "pop bp ; restore old bp\n");
    fprintf(ReportFile, "ret\n");

    return BACK_SUCCESS;
}

backErr_t TranslateFuncDef(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_OP || TR_NODE_HASH != HASH_DEF) { 
        return BACK_ERROR; 
    }
    
    node_t *saved_node = TR_NODE;

    if (!IS_BAD_PTR(TR_NODE_L) && TR_NODE_L->type == ARG_FUNC)
    {
        if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK)
        {
            const char* func_name = TR_NODE_L->item.func;
            fprintf(ReportFile, "%s:\n", func_name);
            
            // Пролог функции
            fprintf(ReportFile, "push bp\n");
            fprintf(ReportFile, "pop [bp] ; save old bp at [bp]\n");
            fprintf(ReportFile, "push sp\n");
            fprintf(ReportFile, "pop bp ; set new bp\n");
            
            if (enterScope(trans) != BACK_SUCCESS)
            { 
                TR_NODE = saved_node;
                return BACK_ERROR; 
            }
                        
            // Обработка параметров (в левой части LINK)
            node_t* param_list = TR_NODE_R->left;
            int param_offset = 2;  // Параметры начинаются с bp+2 (после сохраненного bp и адреса возврата)
            
            // Сначала собираем имена параметров
            const char* param_names[100] = {0};
            int param_count = 0;
            node_t* current_param = param_list;
            
            while (!IS_BAD_PTR(current_param) && current_param->type == ARG_VAR)
            {
                param_names[param_count] = current_param->item.var;
                param_count++;
                current_param = current_param->left;
            }
            
            // Создаем параметры в таблице имен
            for (int i = 0; i < param_count; i++)
            {
                ntElem_t* param_elem = addVar(trans, param_names[i], ARG_VAR, param_offset + i);
                
                if (!IS_BAD_PTR(param_elem))
                {
                    fprintf(ReportFile, "; Parameter: %s at bp+%d\n", param_names[i], param_offset + i);
                }
            }
            
            // Обработка тела функции (в правой части LINK)
            node_t* func_body = TR_NODE_R->right;
            if (!IS_BAD_PTR(func_body))
            {
                node_t* saved_body_node = TR_NODE;
                
                TR_NODE = func_body;
                backErr_t result = TranslatePrimary(trans);                
                TR_NODE = saved_body_node;
                
                if (result != BACK_SUCCESS)
                {
                    exitScope(trans);
                    TR_NODE = saved_node;
                    return result;
                }
            }
            
            // Эпилог функции (будет выполнен при return)
            // Не добавляем ret здесь - он добавляется в TranslateReturn
            
            if (exitScope(trans) != BACK_SUCCESS)
            { 
                TR_NODE = saved_node;
                return BACK_ERROR; 
            }
            
            TR_NODE = saved_node;
            return BACK_SUCCESS;
        }
    }
    
    TR_NODE = saved_node;
    return BACK_SUCCESS;
}

backErr_t TranslateFuncCall(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_FUNC) { return BACK_ERROR; }
    
    node_t *saved_node = TR_NODE;
    const char* func_name = TR_NODE_FUNC;
    
    // Подсчитываем количество аргументов
    int arg_count = 0;
    if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK)
    {
        node_t* arg_node = TR_NODE_R;
        
        // Подсчет аргументов
        while (!IS_BAD_PTR(arg_node) && arg_node->type == ARG_LINK)
        {
            arg_count++;
            arg_node = arg_node->left;
        }
    }
    
    // Если есть аргументы, вычисляем их
    if (arg_count > 0 && !IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK)
    {
        node_t* arg_node = TR_NODE_R;
        
        // Собираем узлы аргументов в массив
        node_t* arg_nodes[100] = {0};
        int i = 0;
        
        while (!IS_BAD_PTR(arg_node) && arg_node->type == ARG_LINK && i < arg_count)
        {
            if (!IS_BAD_PTR(arg_node->right))
            {
                arg_nodes[i] = arg_node->right;
                i++;
            }
            arg_node = arg_node->left;
        }
        
        // Вычисляем аргументы и кладем в стек (первый аргумент - последним)
        for (int j = arg_count - 1; j >= 0; j--)
        {
            if (!IS_BAD_PTR(arg_nodes[j]))
            {
                TR_NODE = arg_nodes[j];
                backErr_t res = TranslatePrimary(trans);
                if (res != BACK_SUCCESS) 
                { 
                    TR_NODE = saved_node;
                    return res; 
                }
            }
        }
    }
    
    // Сохраняем адрес возврата
    fprintf(ReportFile, "push $+2 ; save return address\n");
    
    // Вызов функции
    fprintf(ReportFile, "jmp %s\n", func_name);
    
    // Метка возврата
    fprintf(ReportFile, "; Return from %s\n", func_name);
    
    // Очищаем аргументы из стека
    if (arg_count > 0)
    {
        fprintf(ReportFile, "add sp, %d ; clean up %d arguments\n", arg_count, arg_count);
    }
    
    TR_NODE = saved_node;
    return BACK_SUCCESS;
}

backErr_t TranslateVarInit(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_OP || IS_BAD_PTR(TR_NODE_L) ||
        TR_NODE_L->type != ARG_OP) { return BACK_ERROR; }

    const char* var_name = TR_NODE_L->item.var;
    
    int offset = 0;
    if (CUR_NAME_TABLE_POS == 0)
    {
        offset = trans->global_offset;
        trans->global_offset += 1;
    }
    else
    {
        // Для локальных переменных считаем отрицательные смещения от bp
        if (!IS_BAD_PTR(CUR_NAME_TABLE))
        {
            int var_count = 0;
            for (int i = 0; i < HT_SIZE; ++i)
            {
                if (CUR_NAME_TABLE->table[i].is_used && !IS_BAD_PTR(CUR_NAME_TABLE->table[i].stk))
                {
                    var_count += CUR_NAME_TABLE->table[i].stk->size;
                }
            }
            offset = -(var_count + 1);  // -1, -2, -3, ...
        }
    }
    
    node_t *saved_node = TR_NODE;
    TR_NODE = TR_NODE_L;
    backErr_t res = TranslateAssignment(trans);
    TR_NODE = saved_node;
    
    return res;
}


backErr_t TranslateAssignment(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_OP || TR_NODE_HASH != HASH_EQ ||
        IS_BAD_PTR(TR_NODE_L) || TR_NODE_L->type != ARG_VAR)  { return BACK_ERROR; }

    const char* var_name = TR_NODE_L->item.var;
    
    ntElem_t* var_elem = findVar(trans, var_name);
    if (IS_BAD_PTR(var_elem))
    {
        int offset = 0;
        
        if (CUR_NAME_TABLE_POS == 0)
        {
            // Глобальная переменная
            offset = trans->global_offset;
            trans->global_offset += 1;
        }
        else
        {
            // Локальная переменная - отрицательное смещение от bp
            if (!IS_BAD_PTR(CUR_NAME_TABLE))
            {
                int var_count = 0;
                for (int i = 0; i < HT_SIZE; ++i)
                {
                    if (CUR_NAME_TABLE->table[i].is_used && !IS_BAD_PTR(CUR_NAME_TABLE->table[i].stk))
                    {
                        var_count += CUR_NAME_TABLE->table[i].stk->size;
                    }
                }
                offset = -(var_count + 1);  // -1, -2, -3, ...
            }
        }
        
        var_elem = addVar(trans, var_name, ARG_VAR, offset);
        if (IS_BAD_PTR(var_elem))
        {
            printf(ANSI_COLOR_RED "Error: Cannot create variable '%s'\n" ANSI_COLOR_RESET, var_name);
            return BACK_ERROR;
        }
        
        // Инициализация переменной нулем
        if (CUR_NAME_TABLE_POS == 0)
        {
            fprintf(ReportFile, "push 0\n");
            fprintf(ReportFile, "pop [%d]\n", offset);
        }
        else
        {
            fprintf(ReportFile, "push 0\n");
            fprintf(ReportFile, "pop [bp%d]\n", offset);  // bp-1, bp-2, ...
        }
    }

    node_t *saved_node = TR_NODE;
    TR_NODE = TR_NODE_R;
    backErr_t res = TranslatePrimary(trans);
    TR_NODE = saved_node;
    if (res != BACK_SUCCESS) return res;

    // Запись в переменную
    if (CUR_NAME_TABLE_POS == 0)
    {
        fprintf(ReportFile, "pop [%d]\n", var_elem->offset);
    }
    else
    {
        fprintf(ReportFile, "pop [bp");
        if (var_elem->offset >= 0)
        {
            fprintf(ReportFile, "+%d", var_elem->offset);
        }
        else
        {
            fprintf(ReportFile, "%d", var_elem->offset);  // отрицательное число
        }
        fprintf(ReportFile, "]\n");
    }
    
    return BACK_SUCCESS;
}


backErr_t TranslateVar(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_VAR) { return BACK_ERROR; }

    const char* var_name = TR_NODE_VAR;
    
    ntElem_t* var_elem = findVar(trans, var_name);
    if (IS_BAD_PTR(var_elem))
    {
        printf(ANSI_COLOR_RED "Error: Undefined variable '%s'\n" ANSI_COLOR_RESET, var_name);
        return BACK_ERROR;
    }

    // Чтение переменной
    if (CUR_NAME_TABLE_POS == 0)
    {
        // Глобальная переменная
        fprintf(ReportFile, "push [%d]\n", var_elem->offset);
    }
    else
    {
        // Локальная переменная
        fprintf(ReportFile, "push [bp");
        if (var_elem->offset >= 0)
        {
            fprintf(ReportFile, "+%d", var_elem->offset);
        }
        else
        {
            fprintf(ReportFile, "%d", var_elem->offset);  // отрицательное число
        }
        fprintf(ReportFile, "]\n");
    }

    return BACK_SUCCESS;
}