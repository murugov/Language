#include "./parser.hpp"


frontErr_t parserCtor(parser_t *parser, const char *src)
{
    ON_DEBUG( if (IS_BAD_PTR(parser)) { return FRONT_ERROR; } )

    FILE *SourceFile = fopen(src, "r");
    if (IS_BAD_PTR(SourceFile)) { printf(ANSI_COLOR_RED "Error: Cannot open file %s\n" ANSI_COLOR_RESET, src); return FRONT_ERROR; }
    
    char *buffer = NULL;
    int count_lines = 0;
    char **lines = DataReader(SourceFile, buffer, &count_lines);
    fclose(SourceFile);
    if (IS_BAD_PTR(lines))
    {
        printf("Error: Failed to read file\n");
        return FRONT_ERROR;
    }
    
    lexer_t *lexer = (lexer_t*)calloc(1, sizeof(lexer_t));
    LexerCtor(lexer, lines, count_lines, src);
    if (IS_BAD_PTR(lexer))
    {
        printf("Error: Failed to initialize lexer\n");
        FreeLines(lines, count_lines);
        return FRONT_ERROR;
    }

    parser->lexer = lexer;

    stk_t<ht_t<ntElem_t*>*>* name_tables = (stk_t<ht_t<ntElem_t*>*>*)calloc(1, sizeof(stk_t<ht_t<ntElem_t*>*>));
    if (IS_BAD_PTR(name_tables))
    { 
        LexerDtor(lexer);
        FreeLines(lines, count_lines);
        return FRONT_ERROR; 
    }

    STACK_CTOR(name_tables, MIN_STK_LEN);
    
    ht_t<ntElem_t*>* global_table = (ht_t<ntElem_t*>*)calloc(1, sizeof(ht_t<ntElem_t*>));
    if (!IS_BAD_PTR(global_table))
    {
        HT_CTOR(global_table);
        StackPush(name_tables, global_table);
        CUR_NAME_TABLE_POS++;
    }

    parser->name_tables    = name_tables;
    parser->cur_name_table = 0; 

    return FRONT_SUCCESS;
}


frontErr_t parserDtor(parser_t *parser)
{
    ON_DEBUG( if (IS_BAD_PTR(parser)) { return FRONT_ERROR; } )

    if (!IS_BAD_PTR(parser->lexer))
    { 
        LexerDtor(parser->lexer);
        parser->lexer = NULL;
    }

    if (!IS_BAD_PTR(parser->name_tables))
    {
        for (int i = 0; i < parser->name_tables->size; ++i)
        {
            if (parser->name_tables->data[i])
            {
                HT_DTOR(parser->name_tables->data[i]);
            }
        }
        
        STACK_DTOR(parser->name_tables);
        free(parser->name_tables);
        parser->name_tables = NULL;
    }

    return FRONT_SUCCESS;
}


int MatchToken(parser_t* parser, hash_t hash)
{
    if (CheckToken(parser, hash))
    { 
        CUR_POS++; 
        return 1; 
    }
    return 0;
}


int CheckToken(parser_t* parser, hash_t hash)
{
    if (parser->lexer->cur_token >= parser->lexer->tokens->size) { return 0; }
    
    return (CUR_HASH == hash);
}


token_t* ConsumeToken(parser_t* parser, hash_t hash, const char* error_msg)
{
    if (CheckToken(parser, hash))
    {
        token_t* token = CUR_TOKEN;
        CUR_POS++;
        return token;
    }

    PrintError(parser, CUR_TOKEN, error_msg);
    return NULL;
}


void PrintError(parser_t* parser, token_t* token, const char* message)
{
    ON_DEBUG( if (IS_BAD_PTR(parser) || IS_BAD_PTR(message)) return; )
    
    if (!IS_BAD_PTR(token))
    {
        printf(ANSI_COLOR_RED "Error at line %d, column %d: %s\n" ANSI_COLOR_RESET, token->line, token->col, message);
        printf(ANSI_COLOR_RED "Token: type=%d, hash=%zu\n" ANSI_COLOR_RESET, token->type, token->hash);
    }
    else 
    {
        printf(ANSI_COLOR_RED "Error: %s\n" ANSI_COLOR_RESET, message);
    }
}


node_t* ParseAST(parser_t* parser)
{
    ON_DEBUG( if (IS_BAD_PTR(parser)) { return NULL; } )

    node_t* program    = NULL;
    node_t* stmt       = NULL;
    node_t* last_stmt  = NULL;
    
    while (!CheckToken(parser, HASH_EOF))
    {
        if (CUR_TYPE == ARG_OP && CUR_HASH == HASH_DEF)
        {
            stmt = ParseFunc(parser);
        }
        else
        {
            stmt = ParseStatement(parser);
        }

        if (IS_BAD_PTR(stmt)) 
        { 
            FreeNodes(program);
            return NULL; 
        }

        if (IS_BAD_PTR(last_stmt)) 
        { 
            program = stmt; 
        }
        else 
        { 
            last_stmt->right = stmt; 
        }

        last_stmt = stmt;
    }

    set_parents(program, NULL);
    return program;
}


node_t* ParseFunc(parser_t* parser)
{
    MatchToken(parser, HASH_DEF);
    
    if (CUR_TYPE != ARG_FUNC)
    {
        PrintError(parser, CUR_TOKEN, "Expected function name after 'def'");
        return NULL;
    }
    
    ht_t<ntElem_t*>* name_table = (ht_t<ntElem_t*>*)calloc(1, sizeof(ht_t<ntElem_t*>));
    if (IS_BAD_PTR(name_table)) { return NULL; }
    HT_CTOR(name_table);
    StackPush(parser->name_tables, name_table);
    CUR_NAME_TABLE_POS++;

    node_t* func_node = FUNC_(CUR_START, CUR_LEN);
    if (IS_BAD_PTR(func_node)) { CUR_NAME_TABLE_POS--; return NULL; }
    
    MatchToken(parser, CUR_HASH);
    
    node_t* params = ParseParams(parser);
    
    node_t* body = ParseBlock(parser);
    if (IS_BAD_PTR(body)) 
    { 
        CUR_NAME_TABLE_POS--;
        if (params) { FreeNodes(params); } 
        return NULL; 
    }

    node_t* def_node = OP_(HASH_DEF);
    if (IS_BAD_PTR(def_node))
    {
        CUR_NAME_TABLE_POS--;
        FreeNodes(func_node);
        FreeNodes(params);
        FreeNodes(body);
        return NULL;
    }

     node_t* link_node_1 = LINK_('@');
    if (IS_BAD_PTR(link_node_1))
    { 
        CUR_NAME_TABLE_POS--;
        FreeNodes(func_node);
        FreeNodes(params);
        FreeNodes(body);
        FreeNodes(def_node);
        return NULL;
    }

    node_t* link_node_2 = LINK_('@');
    if (IS_BAD_PTR(link_node_2))
    { 
        CUR_NAME_TABLE_POS--;
        FreeNodes(func_node);
        FreeNodes(params);
        FreeNodes(body);
        FreeNodes(def_node);
        FreeNodes(link_node_1);
        return NULL;
    }

    link_node_1->left  = def_node;
    def_node->left     = func_node;
    def_node->right    = link_node_2;
    link_node_2->left  = params;
    link_node_2->right = body;

    if (!IS_BAD_PTR(CUR_NAME_TABLE)) 
    { 
        ntElem_t *func_elem = (ntElem_t*)calloc(1, sizeof(ntElem_t));
        if (IS_BAD_PTR(func_elem))
        {
            CUR_NAME_TABLE_POS--;
            FreeNodes(func_node);
            FreeNodes(params);
            FreeNodes(body);
            FreeNodes(link_node_1);
            FreeNodes(link_node_2);
            FreeNodes(def_node);
            return NULL; 
        }
        
        func_elem->name            = strndup(CUR_START, (size_t)CUR_LEN);
        func_elem->type            = ARG_FUNC;
        func_elem->offset          = 0;
        func_elem->size_of_stk_frm = CUR_NAME_TABLE_SIZE;

        htInsert(CUR_NAME_TABLE, &func_elem, TakeStrFromNT); 
    }
    
    return link_node_1;
}


node_t* ParseParams(parser_t* parser)
{
    if (!ConsumeToken(parser, HASH_LPAREN, "Expected '(' after function name")) { return NULL; }
    
    node_t* params = NULL;
    
    if (CheckToken(parser, HASH_RPAREN))
    {
        MatchToken(parser, HASH_RPAREN);
        return NULL;
    }
    
    do {
        if (CUR_TYPE != ARG_VAR)
        {
            PrintError(parser, CUR_TOKEN, "Expected parameter name");
            if (params) { FreeNodes(params); }
            return NULL;
        }

        node_t* param = ParseVar(parser);
        if (IS_BAD_PTR(param)) 
        { 
            if (params) FreeNodes(params);
            return NULL; 
        }

        if (params == NULL)
        {
            params = param;
        }
        else
        {
            node_t* comma_node = OP_(HASH_COMMA);
            if (IS_BAD_PTR(comma_node))
            {
                FreeNodes(params);
                return NULL;
            }
            
            param->left = params;
            params = param;
        }
        
    } while (MatchToken(parser, HASH_COMMA));
    
    if (!ConsumeToken(parser, HASH_RPAREN, "Expected ')' after parameters")) 
    { 
        FreeNodes(params); 
        return NULL; 
    }
    
    return params;
}


node_t* ParseStatement(parser_t* parser)
{
    node_t* stmt = NULL;

    if (CUR_TYPE == ARG_OP)
    {
        switch (CUR_HASH)
        {
            case HASH_IF:
                return ParseIf(parser);
            case HASH_WHILE:
                return ParseWhile(parser);
            case HASH_RETURN:
                return ParseReturn(parser);
            case HASH_INIT:
                return ParseVarInit(parser);
            default:
                break;
        }
    }

    if (CUR_TYPE == ARG_VAR)
    {
        if (CUR_POS + 1 < parser->lexer->tokens->size &&
            parser->lexer->tokens->data[CUR_POS + 1]->hash == HASH_EQ)
        {
            return ParseAssignment(parser);
        }
    }
    
    if (CUR_TYPE == ARG_FUNC)
    {
        stmt = ParseFuncCall(parser);
        if (!IS_BAD_PTR(stmt))
        {
            node_t* semic_node = OP_(HASH_SEMICOLON);
            if (IS_BAD_PTR(semic_node))
            {
                FreeNodes(stmt);
                return NULL;
            }
            
            ConsumeToken(parser, HASH_SEMICOLON, "Expected ';' after function call");
            semic_node->left = stmt;
            return semic_node;
        }
    }
    
    stmt = ParseExpression(parser);
    if (!IS_BAD_PTR(stmt))
    {
        node_t* semic_node = OP_(HASH_SEMICOLON);
        if (IS_BAD_PTR(semic_node))
        {
            FreeNodes(stmt);
            return NULL;
        }
        
        ConsumeToken(parser, HASH_SEMICOLON, "Expected ';' after expression");
        semic_node->left = stmt;
        return semic_node;
    }
    
    return NULL;
}


node_t* ParseExpression(parser_t* parser)
{
    node_t* node = ParseTerm(parser);
    if (IS_BAD_PTR(node)) { return NULL; }
    
    while (MatchToken(parser, HASH_ADD) || MatchToken(parser, HASH_SUB))
    {
        hash_t op_hash = PREV_HASH;
        
        node_t* right_node = ParseTerm(parser);
        if (IS_BAD_PTR(right_node))
        {
            FreeNodes(node);
            return NULL;
        }
        
        node_t* op_node = OP_(op_hash);
        if (IS_BAD_PTR(op_node))
        {
            FreeNodes(node);
            FreeNodes(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* ParseTerm(parser_t* parser)
{
    node_t* node = ParseFactor(parser);
    if (IS_BAD_PTR(node)) { return NULL; }
    
    while (MatchToken(parser, HASH_MUL) || MatchToken(parser, HASH_DIV))
    {
        hash_t op_hash = PREV_HASH;
        
        node_t* right_node = ParseFactor(parser);
        if (IS_BAD_PTR(right_node))
        {
            FreeNodes(node);
            return NULL;
        }
        
        node_t* op_node = OP_(op_hash);
        if (IS_BAD_PTR(op_node))
        {
            FreeNodes(node);
            FreeNodes(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* ParseFactor(parser_t* parser)
{
    node_t* node = ParsePrimary(parser);
    if (IS_BAD_PTR(node)) { return NULL; }

    while (MatchToken(parser, HASH_POW))
    {
        node_t* right_node = ParsePrimary(parser);
        if (IS_BAD_PTR(right_node))
        {
            FreeNodes(node);
            return NULL;
        }
        
        node_t* op_node = OP_(HASH_POW);
        if (IS_BAD_PTR(op_node))
        {
            FreeNodes(node);
            FreeNodes(right_node);
            return NULL;
        }
        
        op_node->left  = node;
        op_node->right = right_node;
        node           = op_node;
    }
    
    return node;
}


node_t* ParsePrimary(parser_t* parser)
{
    if (CUR_HASH == HASH_INIT) { return ParseVarInit(parser); }

    switch (CUR_TYPE)
    {
        case ARG_NUM:
            return ParseNum(parser);
        case ARG_VAR:
            return ParseVar(parser);
        case ARG_FUNC:
            return ParseFuncCall(parser);
        case ARG_LINK:
        case ARG_OP:
        default:
            break;
    }

    if (MatchToken(parser, HASH_LPAREN))
    {
        node_t* expr = ParseExpression(parser);
        if (IS_BAD_PTR(expr)) { return NULL; }
        
        if (!ConsumeToken(parser, HASH_RPAREN, "Expected ')' after expression"))
        {
            FreeNodes(expr);
            return NULL;
        }
        
        return expr;
    }
    
    PrintError(parser, CUR_TOKEN, "Expected number, variable, function, or '('");
    return NULL;
}


node_t* ParseVarInit(parser_t* parser)
{
    MatchToken(parser, HASH_INIT);
    
    if (CUR_TYPE != ARG_VAR)
    {
        PrintError(parser, CUR_TOKEN, "Expected variable name after 'init'");
        return NULL;
    }
    
    node_t* semic_node = OP_(HASH_SEMICOLON);
    if (IS_BAD_PTR(semic_node)) { return NULL; }

    node_t* assig_node = ParseAssignment(parser);
    if (IS_BAD_PTR(assig_node))
    {
        FreeNodes(semic_node);
        return NULL;
    }

    semic_node->left = assig_node;
    assig_node->item.op = HASH_INIT;
    return semic_node;
}


node_t* ParseAssignment(parser_t* parser)
{
    if (CUR_TYPE != ARG_VAR)
    {
        PrintError(parser, CUR_TOKEN, "Expected variable name for assignment");
        return NULL;
    }

    node_t* var_node = VAR_(CUR_START, CUR_LEN);
    if (IS_BAD_PTR(var_node)) { return NULL; }

    if (!IS_BAD_PTR(CUR_NAME_TABLE)) 
    { 
        ntElem_t *var_elem = (ntElem_t*)calloc(1, sizeof(ntElem_t));
        if (IS_BAD_PTR(var_elem))
        {
            FreeNodes(var_node);
            return NULL; 
        }
        
        var_elem->name            = strndup(CUR_START, (size_t)CUR_LEN);
        var_elem->type            = ARG_VAR;
        var_elem->offset          = CUR_NAME_TABLE_SIZE;
        var_elem->size_of_stk_frm = 0;

        if(PREV_HASH == HASH_INIT && !IS_BAD_PTR(htFind(CUR_NAME_TABLE, &var_elem, TakeStrFromNT)))
        {
            FreeNodes(var_node);
            free(var_elem);
            PrintError(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        else if(PREV_HASH != HASH_INIT && IS_BAD_PTR(htFind(CUR_NAME_TABLE, &var_elem, TakeStrFromNT)))
        {
            FreeNodes(var_node);
            free(var_elem);
            PrintError(parser, CUR_TOKEN, "Undeclarated variable");
            return NULL;
        }

        htInsert(CUR_NAME_TABLE, &var_elem, TakeStrFromNT); 
    }

    MatchToken(parser, CUR_HASH);
    
    if (!ConsumeToken(parser, HASH_EQ, "Expected '=' after variable name"))
    {
        FreeNodes(var_node);
        return NULL;
    }
    
    node_t* value = ParseExpression(parser);
    if (IS_BAD_PTR(value))
    {
        FreeNodes(var_node);
        return NULL;
    }
    
    if (!ConsumeToken(parser, HASH_SEMICOLON, "Expected ';' after assignment")) 
    { 
        FreeNodes(var_node);
        FreeNodes(value); 
        return NULL; 
    }
    
    node_t* semic_node = OP_(HASH_SEMICOLON);
    if (IS_BAD_PTR(semic_node))
    {
        FreeNodes(var_node);
        FreeNodes(value);
        return NULL;
    }
    
    node_t* assign_node = OP_(HASH_EQ);
    if (IS_BAD_PTR(assign_node))
    {
        FreeNodes(value);
        FreeNodes(semic_node);
        return NULL;
    }
    
    assign_node->left  = var_node;
    assign_node->right = value;

    semic_node->left = assign_node;
    
    return semic_node;
}


node_t* ParseIf(parser_t* parser)
{
    MatchToken(parser, HASH_IF);
    
    if (!ConsumeToken(parser, HASH_LPAREN, "Expected '(' after 'if'")) { return NULL; }
    
    node_t* if_node = OP_(HASH_IF);
    if (IS_BAD_PTR(if_node)) { return NULL; }

    node_t* condition = ParseCondition(parser);
    if (IS_BAD_PTR(condition))
    {
        FreeNodes(if_node);
        return NULL;
    }
    
    if (!ConsumeToken(parser, HASH_RPAREN, "Expected ')' after condition"))
    {
        FreeNodes(if_node);
        FreeNodes(condition);
        return NULL;
    }
    
    node_t* if_body = ParseBlock(parser);
    if (IS_BAD_PTR(if_body))
    {
        FreeNodes(if_node);
        FreeNodes(condition);
        return NULL;
    }

    if_node->left = condition;
    
    node_t* else_body = NULL;
    node_t* else_node = NULL;

    if (CheckToken(parser, HASH_ELSE))
    {
        MatchToken(parser, HASH_ELSE);
        
        else_body = ParseBlock(parser);
        if (IS_BAD_PTR(else_body))
        {
            FreeNodes(if_node);
            FreeNodes(condition);
            FreeNodes(if_body);
            return NULL;
        }
        
        else_node = OP_(HASH_ELSE);
        if (IS_BAD_PTR(else_node))
        {
            FreeNodes(if_node);
            FreeNodes(condition);
            FreeNodes(if_body);
            FreeNodes(else_body);
            return NULL;
        }
        
        if_node->right = else_node;
        else_node->left = if_body;
        else_node->right = else_body;
    }
    else
    {
        if_node->right = if_body;
    }

    node_t* link_node = LINK_('@');
    if (IS_BAD_PTR(link_node)) 
    {
        FreeNodes(if_node);
        FreeNodes(condition);
        FreeNodes(if_body);
        if (!IS_BAD_PTR(else_body)) { FreeNodes(else_body); }
        if (!IS_BAD_PTR(else_node)) { FreeNodes(else_node); }
        return NULL; 
    }
    
    link_node->left = if_node;

    return link_node;
}


node_t* ParseWhile(parser_t* parser)
{
    MatchToken(parser, HASH_WHILE);
    
    if (!ConsumeToken(parser, HASH_LPAREN, "Expected '(' after 'while'")) { return NULL; }

    node_t* condition = ParseCondition(parser);
    if (IS_BAD_PTR(condition)) { return NULL; }
    
    if (!ConsumeToken(parser, HASH_RPAREN, "Expected ')' after condition"))
    { 
        FreeNodes(condition);
        return NULL;
    }
    
    node_t* body = ParseBlock(parser);
    if (IS_BAD_PTR(body))
    {
        FreeNodes(condition);
        return NULL;
    }
    
    node_t* while_node = OP_(HASH_WHILE);
    if (IS_BAD_PTR(while_node))
    {
        FreeNodes(condition);
        FreeNodes(body);
        return NULL;
    }
    
    while_node->left = condition;
    while_node->right = body;

    node_t* link_node = LINK_('@');
    if (IS_BAD_PTR(link_node))
    {
        FreeNodes(body);
        FreeNodes(condition);
        FreeNodes(while_node);
        return NULL;
    }
    
    link_node->left = while_node;
    
    return link_node;
}


node_t* ParseReturn(parser_t* parser)
{
    MatchToken(parser, HASH_RETURN);
    
    node_t* value = ParseExpression(parser);
    if (IS_BAD_PTR(value)) { return NULL; }
    
    if (!ConsumeToken(parser, HASH_SEMICOLON, "Expected ';' after return")) 
    { 
        FreeNodes(value); 
        return NULL; 
    }
    
    node_t* return_node = OP_(HASH_RETURN);
    if (IS_BAD_PTR(return_node))
    {
        FreeNodes(value);
        return NULL;
    }
    
    return_node->left = value;
    
    return return_node;
}


node_t* ParseFuncCall(parser_t* parser)
{
    if (CUR_TYPE != ARG_FUNC)
    {
        PrintError(parser, CUR_TOKEN, "Expected function name");
        return NULL;
    }
    
    token_t* func_token = CUR_TOKEN;
    MatchToken(parser, func_token->hash);
    
    if (!ConsumeToken(parser, HASH_LPAREN, "Expected '(' after function name")) { return NULL; }
    
    node_t* args = NULL;
    
    if (!MatchToken(parser, HASH_RPAREN))
    {
        node_t* arg = ParseExpression(parser);
        if (IS_BAD_PTR(arg)) { return NULL; }
        
        args = arg;
        
        while (MatchToken(parser, HASH_COMMA))
        {
            node_t* next_arg = ParseExpression(parser);
            if (IS_BAD_PTR(next_arg)) 
            { 
                FreeNodes(args); 
                return NULL; 
            }
            
            node_t* comma_node = OP_(HASH_COMMA);
            if (IS_BAD_PTR(comma_node))
            {
                FreeNodes(next_arg);
                FreeNodes(args);
                return NULL;
            }
            
            comma_node->left = args;
            comma_node->right = next_arg;
            args = comma_node;
        }
        
        if (!ConsumeToken(parser, HASH_RPAREN, "Expected ')' after arguments")) 
        { 
            FreeNodes(args); 
            return NULL; 
        }
    }
    
    node_t* call_node = FUNC_(func_token->start, func_token->length);
    if (IS_BAD_PTR(call_node)) 
    { 
        FreeNodes(args); 
        return NULL; 
    }
    
    call_node->left = args;
    
    return call_node;
}


node_t* ParseCondition(parser_t* parser)
{
    node_t* left_node = ParseExpression(parser);
    if (IS_BAD_PTR(left_node)) { return NULL; }
    
    if (CheckToken(parser, HASH_LT)   ||
        CheckToken(parser, HASH_GT)   ||
        CheckToken(parser, HASH_LE)   ||
        CheckToken(parser, HASH_GE)   ||
        CheckToken(parser, HASH_EQEQ) ||
        CheckToken(parser, HASH_NE))
    {
        hash_t op_hash = CUR_HASH;
        MatchToken(parser, op_hash);
        
        node_t* right_node = ParseExpression(parser);
        if (IS_BAD_PTR(right_node))
        {
            FreeNodes(left_node);
            return NULL;
        }
        
        node_t* cmp_node = OP_(op_hash);
        if (IS_BAD_PTR(cmp_node))
        {
            FreeNodes(left_node);
            FreeNodes(right_node);
            return NULL;
        }
        
        cmp_node->left  = left_node;
        cmp_node->right = right_node;
        return cmp_node;
    }
    
    return left_node;
}


node_t* ParseBlock(parser_t* parser)
{
    if (!ConsumeToken(parser, HASH_LBRACE, "Expected '{'")) { return NULL; }
    
    node_t* first_stmt = NULL;
    node_t* last_stmt = NULL;
    
    while (!CheckToken(parser, HASH_RBRACE) && !CheckToken(parser, HASH_EOF))
    {
        node_t* stmt = ParseStatement(parser);
        
        if (IS_BAD_PTR(stmt)) 
        {
            if (first_stmt) { FreeNodes(first_stmt); }
            return NULL;
        }
        
        if (first_stmt == NULL)
        {
            first_stmt = stmt;
            last_stmt = stmt;
        }
        else
        {
            last_stmt->right = stmt;
            last_stmt = stmt;
        }
    }
    
    if (!ConsumeToken(parser, HASH_RBRACE, "Expected '}'"))
    {
        if (first_stmt) { FreeNodes(first_stmt); }
        return NULL;
    }
    
    return first_stmt;
}


node_t* ParseVar(parser_t* parser)
{
    if (CUR_TYPE != ARG_VAR)
    {
        PrintError(parser, CUR_TOKEN, "Expected variable");
        return NULL;
    }
    
    node_t* var_node = VAR_(CUR_START, CUR_LEN);

    if (!IS_BAD_PTR(CUR_NAME_TABLE)) 
    { 
        ntElem_t *var_elem = (ntElem_t*)calloc(1, sizeof(ntElem_t));
        if (IS_BAD_PTR(var_elem))
        {
            FreeNodes(var_node);
            return NULL; 
        }
        
        var_elem->name            = strndup(CUR_START, (size_t)CUR_LEN);
        var_elem->type            = ARG_VAR;
        var_elem->offset          = CUR_NAME_TABLE_SIZE;
        var_elem->size_of_stk_frm = 0;

        if(PREV_HASH == HASH_INIT && !IS_BAD_PTR(htFind(CUR_NAME_TABLE, &var_elem, TakeStrFromNT)))
        {
            FreeNodes(var_node);
            free(var_elem);
            PrintError(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        else if(PREV_HASH == HASH_COMMA && !IS_BAD_PTR(htFind(CUR_NAME_TABLE, &var_elem, TakeStrFromNT)))
        {
            FreeNodes(var_node);
            free(var_elem);
            PrintError(parser, CUR_TOKEN, "Redeclarated variable");
            return NULL;
        }
        // else if(PREV_HASH != HASH_INIT && PREV_HASH != HASH_COMMA && PREV_HASH != HASH_LPAREN && IS_BAD_PTR(htFind(CUR_NAME_TABLE, &var_elem, TakeStrFromNT)))
        // {
        //     FreeNodes(var_node);
        //     free(var_elem);
        //     PrintError(parser, CUR_TOKEN, "Undeclarated variable");
        //     return NULL;
        // }
        // // mistake in third condition
        htInsert(CUR_NAME_TABLE, &var_elem, TakeStrFromNT); 
    }
    
    MatchToken(parser, CUR_HASH);

    return var_node;
}


node_t* ParseNum(parser_t* parser)
{
    if (CUR_TYPE != ARG_NUM)
    {
        PrintError(parser, CUR_TOKEN, "Expected number");
        return NULL;
    }
    
    token_t* num_token = CUR_TOKEN;
    MatchToken(parser, CUR_HASH);
    
    int value = 0;
    sscanf(num_token->start, "%d", &value);
    
    return NUM_(value);
}


const char *TakeStrFromNT(const void *nt_elem)
{
   const ntElem_t *var = (const ntElem_t*)nt_elem;

    return var->name;
}