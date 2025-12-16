#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "tree.h"
#include "readtree.h"
#include "config.h"
#include "token.h"



node_t* ReadTree (data_lexer* data_lex)
{
    struct ast_data data = {};
    AstDataInit (&data, data_lex);

    node_t* root = GetProgram (&data);

    AstDataDeleter (&data);

    return root;
}


LangErr_t AstDataInit (ast_data* data, data_lexer* data_lex)
{
    data->tokens = data_lex->tokens;
    data->vars  = (char**) calloc ( MAX_VARS, sizeof (char*));
    if (data->vars == NULL)
    {
        D_PRINT ("data->vars == NULL\n");
        return LN_ERR;
    }

    data->funcs = (func*) calloc ( MAX_FUNCS, sizeof (func));
    if (data->funcs == NULL)
    {
        D_PRINT ("data->funcs == NULL\n");
        return LN_ERR;
    }

    data->pos = 0;
    return LN_OK;
}


LangErr_t AstDataDeleter (ast_data* data)
{
    free(data->vars);
    free(data->funcs);

    data->pos = 0;
    data->vars_pos = 0;
    data->func_pos = 0;

    return LN_OK;
}


node_t* GetProgram(ast_data* data)
{
    node_t* root =  NULL;
    node_t* node =  NULL;
    node_t* old  =  NULL;

    while (CUR_TOKEN->type != END_TOKEN)
    {

        if (CUR_TOKEN->type == OP &&
            CUR_TOKEN->val.op == FUNC)
        {
            node = GetFunc (data);
        }
        
        else
        {
            node = GetStatement (data);
        }

        if (old) old->right = node;
        else root = node;

        if (!node) return NULL;

        old = node;
    }
    
    return root;
}


node_t* GetFunc (ast_data* data)
{
    node_t* func_op = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN->type != FUNCTION);

    node_t* name = CUR_TOKEN;
    data->funcs[data->func_pos].name = CUR_TOKEN->val.var;

    data->pos++;

    SYNTAX_ERROR (CUR_TOKEN->type != OP &&
                  CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* params = GetParams (data);
    data->func_pos++;
    
    node_t* body   = GetBlock (data);

    func_op->left = name;
    name->left = body;
    name->right = params;
    
    return func_op;
}

node_t* GetReturn(ast_data* data)
{
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != RETURN );
    node_t* return_op = CUR_TOKEN;
    data->pos++;
    
    node_t* expr = GetExpresion(data);
    
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    node_t* semic = CUR_TOKEN;
    data->pos++;
    
    return_op->left = expr;
    semic->left = return_op;

    return semic;
}

node_t* GetParams(ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }

    int var_idx = 0;

    SYNTAX_ERROR(CUR_TOKEN->type != VAR);
    
    node_t* root = CUR_TOKEN;
    data->pos++;

    data->funcs[data->func_pos].vars_loc[var_idx] = CUR_TOKEN->val.var;
    
    while (1)
    {
        if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
        {
            data->pos++;
            break;
        }
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != COMMA);
        
        node_t* comma = CUR_TOKEN;
        data->pos++;
        
        SYNTAX_ERROR(CUR_TOKEN->type != VAR);
        
        node_t* next_param = CUR_TOKEN;
        data->funcs[data->func_pos].vars_loc[var_idx] = CUR_TOKEN->val.var;
        data->pos++;
        
        comma->left = root;
        comma->right = next_param;
        
        root = comma;

        var_idx++;

        if (var_idx >= MAX_FUNC_PARAMS)
        {
            SYNTAX_ERROR(var_idx >= MAX_FUNC_PARAMS);
            printf("max number of vars given to func riched\n");
        }

    }
    
    return root;
}


node_t* GetStatement (ast_data* data)
{
    node_t* root = NULL;

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == IF)
    {
        root = GetIf (data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == WHILE)
    {
        root = GetWhile (data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == RETURN)
    {
        root = GetReturn (data);
    }

    else if (CUR_TOKEN->type == FUNCTION)
    {
        root = GetCall (data);
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
        node_t* semic = CUR_TOKEN;

        semic->left = root;
        root = semic;

        data->pos++;
    }
    
    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == INIT)
    {
        root = GetVarInit (data);
    }

    else if (CUR_TOKEN->type == VAR)
    {
        root = GetVarAct (data);
    }

    else
    {
        root = GetExpresion (data);
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
        node_t* semic = CUR_TOKEN;

        semic->left = root;
        root = semic;

        data->pos++;
    }
    
    return root;  

}

node_t* GetIf (ast_data* data)
{
    node_t* else_node = NULL;
    node_t* else_body = NULL;

    node_t* root = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
    data->pos++;


    node_t* body = GetBlock (data);

    if (CUR_TOKEN->type == OP || CUR_TOKEN->val.op == ELSE)
    {
        else_node = CUR_TOKEN;
        data->pos++;

        else_body = GetBlock (data);
    }

    root->left = condition;

    if (else_body) 
    {
        root->right = else_node;
        else_node->left  = body;
        else_node->right = else_body;
    }
    else root->right = body;

    connection->left = root;

    return connection;
}


node_t* GetWhile (ast_data* data)
{
    node_t* root = CUR_TOKEN;
    data->pos++;

    if (CUR_TOKEN->type != CONNECTION)
    {
        D_PRINT("ERROR, connetion node missed\n");
        return NULL;
    }

    node_t* connection = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* condition = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
    data->pos++;
    
    node_t* body = GetBlock (data);
    
    root->left = condition;
    root->right = body;

    connection->left = root;

    return root;
}


node_t* GetExpresion (ast_data* data)
{
    return GetLogicOr (data);
}

node_t* GetLogicOr (ast_data* data)
{
    node_t* node = GetLogicAnd (data);
    
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == OR)
    {
        node_t* node_or = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetLogicAnd (data);
        
        node_or->left = node;
        node_or->right = right;
        node = node_or;
    }
    
    return node;
}

node_t* GetLogicAnd (ast_data* data)
{
    node_t* node = GetEquality (data);
    
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == AND)
    {
        node_t* node_and = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetEquality (data);
        
        node_and->left = node;
        node_and->right = right;
        node = node_and;
    }
    
    return node;
}


node_t* GetEquality (ast_data* data)
{
    node_t* node = GetCompare (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == EQEQ || CUR_TOKEN->val.op == NEQ))
    {
        node_t* node_eq = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetCompare (data);
        
        node_eq->left = node;
        node_eq->right = right;
        node = node_eq;
    }
    
    return node;
}

node_t* GetCompare (ast_data* data)
{
    node_t* node = GetAddSub (data);

    while (CUR_TOKEN->type == OP &&
          (CUR_TOKEN->val.op == BIGR || CUR_TOKEN->val.op == SMLR ||
           CUR_TOKEN->val.op == EBIGR || CUR_TOKEN->val.op == ESMLR))
    {
        node_t* node_cmp = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetAddSub (data);
        
        node_cmp->left = node;
        node_cmp->right = right;
        node = node_cmp;
    }
    
    return node;
}

node_t* GetAddSub (ast_data* data)
{
    node_t* node = GetMulDiv (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == ADD || CUR_TOKEN->val.op == SUB))
    {
        node_t* node_op = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetMulDiv (data);
        
        node_op->left = node;
        node_op->right = right;
        node = node_op;
    }
    
    return node;
}

node_t* GetMulDiv (ast_data* data)
{
    node_t* node = GetUnar (data);
    
    while (CUR_TOKEN->type == OP && 
          (CUR_TOKEN->val.op == MUL || CUR_TOKEN->val.op == DIV))
    {
        node_t* node_op = CUR_TOKEN;
        data->pos++;
        
        node_t* right = GetUnar (data);
        
        node_op->left = node;
        node_op->right = right;
        node = node_op;
    }
    
    return node;
}


node_t* GetUnar(ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == SUB)
    {
        node_t* oper = CUR_TOKEN;
        data->pos++;

        node_t* val = GetVal(data);
        
        oper->left = val;
        return oper;
    }
    
    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == ADD)
    {
        data->pos++;
        return GetVal(data);
    }
    
    else return GetBracket(data);
}

node_t* GetBracket (ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == L_PAR)
    {
        data->pos++;
        node = GetAddSub (data);
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP && CUR_TOKEN->val.op != R_PAR);
        data->pos++;

        return node;
    }

    else return GetVal (data);
}

node_t* GetVal(ast_data* data)
{
    node_t* node = NULL;

    if (CUR_TOKEN->type == NUM)
    {
        node = CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN->type == VAR)
    {   
        node = CUR_TOKEN;
        data->pos++;
    }
    
    else if (CUR_TOKEN->type == FUNCTION)
    {
        node = GetCall(data);
    }

    else if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == L_PAR)
    {
        data->pos++;
        
        node = GetExpresion(data);
        
        SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_PAR);
        data->pos++;
    }

    else if (CUR_TOKEN->type == OP && 
            (CUR_TOKEN->val.op == SUB || 
             CUR_TOKEN->val.op == ADD) )
    {
        node_t* op_node = CUR_TOKEN;
        data->pos++;
        
        node_t* operand = GetVal(data);
        
        op_node->left = operand;
        node = op_node;
    }
    
    else SYNTAX_ERROR(1);

    return node;
}


node_t* GetBlock (ast_data* data)
{
    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_BR);
    data->pos++;

    node_t* root = NULL;
    node_t* node = NULL;
    node_t* old  = NULL;

    while (CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_BR)
    {
        node = GetStatement (data);

        if (!node) return NULL;

        if (old) old->right = node;
        else root = node;

        old = node;
    }

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != R_BR);
    data->pos++;

    return root;
}


node_t* GetVarInit (ast_data* data)
{
    node_t* init = CUR_TOKEN;
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != VAR);
    node_t* var   = CUR_TOKEN;
    node_t* semic = NULL;
    
    data->vars[data->vars_pos] = var->val.var;
    data->vars_pos++;
    data->pos++;

    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == SEMIC)
    {
        semic = CUR_TOKEN;
        semic->left = var;

        return semic;
    }

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != EQ);
    node_t* equal = CUR_TOKEN;
    data->pos++;

    node_t* expr = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    semic = CUR_TOKEN;
    data->pos++;

    equal->left  = var;
    equal->right = expr;

    semic->left = equal;

    init->left = semic;

    return init;
}

node_t* GetVarAct (ast_data* data)
{
    node_t* var = CUR_TOKEN;
    int found = 0;
    for (size_t i = 0; i < data->vars_pos && !found; i++)
        if (strcmp(CUR_TOKEN->val.var, data->vars[i]) == 0) found = 1;

    if (found == 0)
        printf("ERROR, variable - '%s' doesnt exist\n", CUR_TOKEN->val.var);

    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != EQ);
    node_t* equal = CUR_TOKEN;
    data->pos++;

    node_t* val = GetExpresion (data);

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != SEMIC);
    node_t* semic = CUR_TOKEN;
    data->pos++;

    semic->left  = equal;
    equal->left  = var;
    equal->right = val;

    return semic;
}

node_t* GetArgs (ast_data* data)
{
    if (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == R_PAR) 
    {
        data->pos++;
        return NULL;
    }
    
    node_t* root = GetExpresion (data);
   
    while (CUR_TOKEN->type == OP && CUR_TOKEN->val.op == COMMA)
    {  
        node_t* comma = CUR_TOKEN;
        data->pos++;
        
        node_t* next_param = GetExpresion (data);
        
        comma->left = root;
        comma->right = next_param;
        
        root = comma;
    }
    
    SYNTAX_ERROR(CUR_TOKEN->type != OP ||CUR_TOKEN->val.op != R_PAR);
    data->pos++;
    
    return root;
}


node_t* GetCall (ast_data* data)
{
    node_t* function = CUR_TOKEN;
    int check = 1;

    SYNTAX_ERROR(CUR_TOKEN->type != FUNCTION);

    for (size_t i = 0; i < data->func_pos && check != 0; i++ )
    {
        check = strcmp(CUR_TOKEN->val.var, data->funcs[i].name);
    }

    if (check)
        printf("function %s doesnt exist\n", CUR_TOKEN->val.var);

    SYNTAX_ERROR(check);
    data->pos++;

    SYNTAX_ERROR(CUR_TOKEN->type != OP || CUR_TOKEN->val.op != L_PAR);
    data->pos++;

    node_t* args = GetArgs (data);

    function->left = args;

    return function;
}

