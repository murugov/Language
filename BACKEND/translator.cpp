#include "translator.hpp"

static int condition_count = 0;
static int if_count = 0;
static int while_count = 0;
static int func_count = 0;

#define CMP_REG "r0"
#define ADR_REG "r1"


#define GLOBAL_RAM_OFFSET 0
#define LOCAL_RAM_OFFSET 512
#define FRAME_SIZE 64

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
    if (IS_BAD_PTR(SourceFile)) { printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, src); return BACK_ERROR; }

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
                HT_DTOR(trans->name_tables->data[i]);
                free(trans->name_tables->data[i]);
            }
        }
        
        STACK_DTOR(trans->name_tables);
        free(trans->name_tables);
        trans->name_tables = NULL;
    }

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
    
    ht_t<ntElem_t*>* cur_table = trans->name_tables->data[trans->cur_name_table];
    if (!IS_BAD_PTR(cur_table)) { HT_DTOR(cur_table); free(cur_table); }
    
    ht_t<ntElem_t*> *tmp = {};
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
        
        ntElem_t *search_elem = {};

        search_elem->name            = var_name;
        search_elem->type            = ARG_VAR;
        search_elem->offset          = 0;
        search_elem->size_of_stk_frm = 0;

        ntElem_t* found = htFind(table, &search_elem, ntElemToStr);
        
        if (!IS_BAD_PTR(found)) { return found; }
    }
    
    return NULL;
}


static ntElem_t* addVar(trans_t *trans, const char* var_name, int type, int offset)
{
    if (IS_BAD_PTR(trans) || IS_BAD_PTR(var_name) || IS_BAD_PTR(trans->name_tables) || trans->name_tables->size == 0) { return NULL; }
    
    ht_t<ntElem_t*>* cur_table = trans->name_tables->data[trans->cur_name_table];
    if (IS_BAD_PTR(cur_table)) { return NULL; }
    
    ntElem_t *search_elem = {};

    search_elem->name            = var_name;
    search_elem->type            = ARG_VAR;
    search_elem->offset          = 0;
    search_elem->size_of_stk_frm = 0;

    if (!IS_BAD_PTR(htFind(cur_table, &search_elem, ntElemToStr))) { return NULL; }
    
    ntElem_t* new_var = (ntElem_t*)calloc(1, sizeof(ntElem_t));
    if (IS_BAD_PTR(new_var)) { return NULL; }
    
    new_var->name = strdup(var_name);
    new_var->type = (type_t)type;
    new_var->offset = offset;
    new_var->size_of_stk_frm = 1;               // or zero
    
    if (htInsert(cur_table, &new_var, ntElemToStr) != HT_SUCCESS)
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
    
    fprintf(ReportFile, "\n; ============== FUNCTIONS ==============\n");
    if (CompileOnlyFunc(trans) != BACK_SUCCESS)
    { 
        fclose(ReportFile);
        ReportFile = NULL;
        return BACK_SUCCESS; 
    }
    

    fprintf(ReportFile, "\n\n; ============== MAIN ==============\n");
    fprintf(ReportFile, "main:\n");
    
    if (CompileNotFunc (trans) != BACK_SUCCESS)
    { 
        fclose(ReportFile);
        ReportFile = NULL;
        return BACK_ERROR; 
    }
    
    fprintf(ReportFile, "\n; ============== END ==============\n");
    fprintf(ReportFile, "\nhlt\n");
    
    fclose(ReportFile);
    ReportFile = NULL;
    
    return BACK_SUCCESS;
}


backErr_t CompileOnlyFunc(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) return BACK_SUCCESS;
    
    if (TR_NODE_TYPE == ARG_FUNC) { return TranslateFunc(trans); }
    
    if (TR_NODE_TYPE == ARG_LINK)
    {        
        node_t *saved_node = TR_NODE;
        if (!IS_BAD_PTR(TR_NODE_L))
        {
            TR_NODE = TR_NODE_L;
            backErr_t compile_error = CompileOnlyFunc(trans);
            if (compile_error != BACK_SUCCESS) { return compile_error; }
        }
        
        if (!IS_BAD_PTR(TR_NODE_R))
        {
            TR_NODE = TR_NODE_R;
            backErr_t compile_error = CompileOnlyFunc(trans);
            if (compile_error != BACK_SUCCESS) { return compile_error; }
        }

        TR_NODE = saved_node;
        
        return BACK_SUCCESS;
    }
    
    return BACK_SUCCESS;
}


backErr_t CompileNotFunc(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_SUCCESS; }

    if (TR_NODE_TYPE == ARG_FUNC) { return BACK_SUCCESS; }
    
    if (TR_NODE_TYPE == ARG_LINK)
    {        
        node_t *saved_node = TR_NODE;
        if (!IS_BAD_PTR(TR_NODE_L))
        {
            TR_NODE = TR_NODE_L;
            backErr_t compile_error = CompileNotFunc(trans);
            if (compile_error != BACK_SUCCESS) { return compile_error; }
        }
        
        if (!IS_BAD_PTR(TR_NODE_R))
        {
            TR_NODE = TR_NODE_R;
            backErr_t compile_error = CompileNotFunc(trans);
            if (compile_error != BACK_SUCCESS) { return compile_error; }
        }

        TR_NODE = saved_node;
        
        return BACK_SUCCESS;
    }
    
    return TranslatePrimary(trans);
}


backErr_t TranslatePrimary(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) {return BACK_SUCCESS;}
    
    node_t *saved_node = TR_NODE;
    switch (TR_NODE_TYPE)
    {
        case ARG_LINK:
        {
            TR_NODE = TR_NODE_L;
            TranslatePrimary(trans);
            TR_NODE = saved_node;
            TR_NODE = TR_NODE_R;
            TranslatePrimary(trans);
            TR_NODE = saved_node;
            return BACK_SUCCESS;
        }

        case ARG_NUM:
            fprintf (ReportFile, "push %d\n", TR_NODE_ITEM.num);
            break;

        case ARG_OP:
            return TranslateOp(trans);
            break;
            
        case ARG_VAR:
            return TranslateVar(trans);
            break;
            
        case ARG_FUNC:
            return TranslateFuncCall(trans);
            break;
            
        default:
            return BACK_ERROR;
    }

    return BACK_SUCCESS;
}


backErr_t TranslateOp(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

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
        case HASH_WHILE:
            return TranslateWhile(trans);
        case HASH_RETURN:
            return TranslateReturn(trans);
            
        case HASH_SEMICOLON:
        {
            if (!IS_BAD_PTR(TR_NODE_L))
            { 
                TR_NODE = TR_NODE_L;
                TranslatePrimary(trans); 
            }
            TR_NODE = saved_node;
            if (!IS_BAD_PTR(TR_NODE_R))
            { 
                TR_NODE = TR_NODE_R;
                TranslatePrimary(trans); 
            }
            TR_NODE = saved_node;
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
    TranslatePrimary(trans);
    TR_NODE = saved_node;
    TR_NODE = TR_NODE_R;
    TranslatePrimary(trans);
    TR_NODE = saved_node;

    switch (TR_NODE_HASH)
    {
        case HASH_ADD: fprintf(ReportFile, "add\n");
            break;
        
        case HASH_SUB: fprintf(ReportFile, "sub\n");
            break;

        case HASH_MUL: fprintf(ReportFile, "mul\n");
            break;

        case HASH_DIV: fprintf(ReportFile, "div\n");
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
    TranslatePrimary(trans);
    TR_NODE = saved_node;
    TR_NODE = TR_NODE_R;
    TranslatePrimary(trans);
    TR_NODE = saved_node;

    fprintf(ReportFile, "push 0\n");
    fprintf(ReportFile, "pop %s\n", CMP_REG);

    switch (TR_NODE_HASH)
    {
        case HASH_EQEQ:
            fprintf (ReportFile, "je");
            break;
        case HASH_NE:
            fprintf (ReportFile, "jne");
            break;
        case HASH_GT:
            fprintf (ReportFile, "jb");
            break;
        case HASH_LT:
            fprintf (ReportFile, "ja");
            break;
        case HASH_GE:
            fprintf (ReportFile, "jbe");
            break;
        case HASH_LE:
            fprintf (ReportFile, "jae");
            break;    

        default:
            return BACK_ERROR;
    }

    fprintf(ReportFile,"false%d:\n", condition_count);

    fprintf(ReportFile, "push 1\n");
    fprintf(ReportFile, "pop %s\n", CMP_REG);

    fprintf(ReportFile, "false%d:\n", condition_count);

    fprintf(ReportFile, "push %s\n", CMP_REG);

    return BACK_SUCCESS;
}


backErr_t TranslateIf(trans_t * trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    if_count++;

    if (enterScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    if (TR_NODE_TYPE == ARG_LINK && !IS_BAD_PTR(TR_NODE_L))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_L;
        TranslateCondition(trans);
        TR_NODE = saved_node;
    }

    fprintf (ReportFile, "push 0\n");
    fprintf (ReportFile, "je endif%d\n", if_count);

    if (TR_NODE_TYPE == ARG_LINK && !IS_BAD_PTR(TR_NODE_R))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_R;
        TranslatePrimary(trans);
        TR_NODE = saved_node;
    }

    fprintf (ReportFile, "endif%d:\n", if_count);

    if (exitScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    return BACK_SUCCESS;
}


backErr_t TranslateWhile (trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    while_count++;

    if (enterScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    fprintf(ReportFile, "while_start%d:\n", while_count);
    
    if (TR_NODE_TYPE == ARG_LINK && !IS_BAD_PTR(TR_NODE_L))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_L;
        TranslateCondition(trans);
        TR_NODE = saved_node;
    }

    fprintf (ReportFile, "push 0\n");
    fprintf (ReportFile, "je endwhile%d\n", while_count);

    if (TR_NODE_TYPE == ARG_LINK && !IS_BAD_PTR(TR_NODE_R))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_R;
        TranslatePrimary(trans);
        TR_NODE = saved_node;
    }
    
    fprintf(ReportFile, "jmp while_start%d\n", while_count);

    fprintf (ReportFile, "endwhile%d:\n", while_count);

    if (exitScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }

    return BACK_SUCCESS;
}


backErr_t TranslateReturn(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE)) { return BACK_ERROR; }

    if (TR_NODE_TYPE == ARG_LINK && !IS_BAD_PTR(TR_NODE_L))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_L;
        TranslatePrimary(trans);
        TR_NODE = saved_node;
    }

    fprintf (ReportFile, "ret\n");

    return BACK_SUCCESS;
}


backErr_t TranslateVarInit(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_OP) { return BACK_ERROR; }
    if (IS_BAD_PTR(TR_NODE_L) || TR_NODE_L->type != ARG_VAR) { return BACK_ERROR; }
    
    const char* var_name = TR_NODE_L->item.var;
    
    int offset = 0;
    ht_t<ntElem_t*>* cur_table = trans->name_tables->data[trans->cur_name_table];
    if (!IS_BAD_PTR(cur_table))
    {
        for (int i = 0; i < HT_SIZE; ++i)
        {
            if (cur_table->table[i].is_used && !IS_BAD_PTR(cur_table->table[i].stk))
            {
                offset += cur_table->table[i].stk->size;
            }
        }
    }
    
    ntElem_t* var_elem = addVar(trans, var_name, ARG_VAR, offset);
    if (IS_BAD_PTR(var_elem)) { return BACK_ERROR; }

    fprintf (ReportFile, "\n; Initialization of %s at offset %d\n", var_name, offset);
    
    node_t *saved_node = TR_NODE;
    TR_NODE = TR_NODE_R;
    TranslatePrimary(trans);
    TR_NODE = saved_node;
    
    fprintf (ReportFile, "pop [%d]\n", offset);

    return BACK_SUCCESS;
}


backErr_t TranslateFunc (trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_FUNC) { return BACK_ERROR; }

    if (IS_BAD_PTR(TR_NODE_L)) { return BACK_ERROR; }
    
    const char* func_name = TR_NODE_L->item.func;
    fprintf (ReportFile, "\n; Function: %s\n", func_name);
    fprintf (ReportFile, "%s:\n", func_name);
    
    if (enterScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }
    
    if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK)
    {
        node_t* param = TR_NODE_R->left;
        int param_offset = 0;
        
        while (!IS_BAD_PTR(param))
        {
            if (param->type == ARG_VAR)
            {
                ntElem_t* param_elem = addVar(trans, param->item.var, ARG_VAR, param_offset);
                if (!IS_BAD_PTR(param_elem))
                {
                    fprintf(ReportFile, "; Parameter: %s at offset %d\n", param->item.var, param_offset);
                    param_offset++;
                }
            }
            param = param->left;
        }
    }
    
    if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK && 
        !IS_BAD_PTR(TR_NODE_R->right))
    {
        node_t *saved_node = TR_NODE;
        TR_NODE = TR_NODE_R->right;
        TranslatePrimary(trans);
        TR_NODE = saved_node;
    }
    
    if (exitScope(trans) != BACK_SUCCESS) { return BACK_ERROR; }
    
    fprintf (ReportFile, "ret\n\n");
    
    return BACK_SUCCESS;
}


backErr_t TranslateVar(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_VAR) { return BACK_ERROR; }

    const char* var_name = TR_NODE_ITEM.var;
    
    ntElem_t* var_elem = findVar(trans, var_name);
    if (IS_BAD_PTR(var_elem))
    {
        fprintf(stderr, ANSI_COLOR_RED "Error: Undefined variable '%s'\n" ANSI_COLOR_RESET, var_name);
        return BACK_ERROR;
    }

    fprintf (ReportFile, "; Loading variable %s from offset %d\n", var_name, var_elem->offset);
    fprintf (ReportFile, "push [%d]\n", var_elem->offset);

    return BACK_SUCCESS;
}


backErr_t TranslateAssignment(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_OP || TR_NODE_HASH != HASH_EQ) 
    { 
        return BACK_ERROR; 
    }

    if (IS_BAD_PTR(TR_NODE_L) || TR_NODE_L->type != ARG_VAR) { return BACK_ERROR; }
    
    const char* var_name = TR_NODE_L->item.var;
    
    ntElem_t* var_elem = findVar(trans, var_name);
    if (IS_BAD_PTR(var_elem))
    {
        fprintf(stderr, ANSI_COLOR_RED "Error: Undefined variable '%s'\n" ANSI_COLOR_RESET, var_name);
        return BACK_ERROR;
    }

    node_t *saved_node = TR_NODE;
    TR_NODE = TR_NODE_R;
    TranslatePrimary(trans);
    TR_NODE = saved_node;

    fprintf (ReportFile, "; Storing to variable %s at offset %d\n", var_name, var_elem->offset);
    fprintf (ReportFile, "pop [%d]\n", var_elem->offset);
    
    return BACK_SUCCESS;
}


backErr_t TranslateFuncCall(trans_t *trans)
{
    if (IS_BAD_PTR(TR_NODE) || TR_NODE_TYPE != ARG_FUNC) { return BACK_ERROR; }

    if (IS_BAD_PTR(TR_NODE_L)) { return BACK_ERROR; }
    
    const char* func_name = TR_NODE_L->item.func;
    
    if (!IS_BAD_PTR(TR_NODE_R) && TR_NODE_R->type == ARG_LINK)
    {
        node_t* param = TR_NODE_R;
        int arg_count = 0;
        
        while (!IS_BAD_PTR(param) && param->type == ARG_LINK)
        {
            if (!IS_BAD_PTR(param->right))
            {
                node_t *saved_node = TR_NODE;
                TR_NODE = param->right;
                TranslatePrimary(trans);
                TR_NODE = saved_node;
                arg_count++;
            }
            param = param->left;
        }
        
        fprintf(ReportFile, "; Pushing %d arguments for function %s\n", arg_count, func_name);
    }
    
    fprintf (ReportFile, "call %s\n", func_name);
    
    fprintf (ReportFile, "; Cleaning up arguments\n");

    return BACK_SUCCESS;
}