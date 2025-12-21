#include "lexer.hpp" 
#include "KeywordSet.cpp"

#define IS_END(lexer) (lexer->cur_line > lexer->line_count - 1)

static token_t *ReadNum(lexer_t *lexer);
static token_t *ReadName(lexer_t *lexer);


char **DataReader(FILE *SourceFile, char *buffer, int *count_line)
{
    ON_DEBUG( if (IS_BAD_PTR(SourceFile)) { LOG(ERROR, "BAD_SRC_FILE_PTR"); return NULL; } )

    size_t len_buffer = 0;
    char **lines = TXTreader(SourceFile, buffer, &len_buffer, count_line, NULL);

    return lines;
}


frontErr_t LexerInit(lexer_t* lexer, char** lines, int line_count, const char* file_name)
{
    stk_t<token_t*>* tokens = (stk_t<token_t*>*)calloc(1, sizeof(stk_t<token_t*>));
    if (IS_BAD_PTR(tokens)) return FRONT_ERROR;

    STACK_CTOR(tokens, MIN_STK_LEN);

    lexer->tokens         = tokens;
    lexer->cur_token      = 0; 
    lexer->peeked_token   = NULL;
    lexer->lines          = lines;
    lexer->line_count     = line_count;
    lexer->cur_line       = 0;
    lexer->cur_col        = 1;
    lexer->cur_pos        = lines[0]; 
    lexer->file_name      = file_name;

    return FRONT_SUCCESS;
}


frontErr_t LexerCtor(lexer_t *lexer, char **lines, int line_count, const char *file_name)
{
    ON_DEBUG( if (IS_BAD_PTR(lexer) || IS_BAD_PTR(lines) || IS_BAD_PTR(file_name)) return FRONT_ERROR; )

    if (LexerInit(lexer, lines, line_count, file_name) != FRONT_SUCCESS) { free(lexer); return FRONT_ERROR; }

    PeekToken(lexer);
    while (!IS_BAD_PTR(lexer->peeked_token) && (lexer->peeked_token->type != ARG_OP || lexer->peeked_token->hash != HASH_EOF))
    {
        // printf("cur_type: [%d];   cur_pos: [%c];    cur_hash: [%zu];    cur_line = [%d];   cur_col = [%d];\n", lexer->peeked_token->type, *lexer->peeked_token->start, lexer->peeked_token->hash, lexer->cur_line, lexer->cur_col);
        if (AdvanceToken(lexer) == FRONT_ERROR) return FRONT_ERROR;
        
        PeekToken(lexer);
    }
    if (StackPush(lexer->tokens, lexer->peeked_token) != STK_SUCCESS) return FRONT_ERROR;
    
    lexer->cur_token = 0;

    ON_DEBUG( LOG(INFO, "Lexer is created"); )
    return FRONT_SUCCESS;
}


frontErr_t LexerDtor(lexer_t* lexer)
{
    ON_DEBUG(if (IS_BAD_PTR(lexer)) return FRONT_ERROR;)
    
    if (IS_BAD_PTR(lexer->tokens))
    {
        ON_DEBUG( LOG(ERROR, "Bad pointer of tokens"); )
    }
    else
    {
        for (ssize_t i = 1; i < lexer->tokens->size - 1; ++i)
        {
            if (lexer->tokens->data[i])
                free(lexer->tokens->data[i]);
        }
        
        STACK_DTOR(lexer->tokens);
        free(lexer->tokens);
        lexer->tokens = NULL;
    }
    
    lexer->cur_token    = 0;
    lexer->peeked_token = NULL;

    FreeLines(lexer->lines, lexer->line_count);
    lexer->lines        = NULL;
    lexer->line_count   = 0;

    lexer->cur_line     = 0;
    lexer->cur_col      = 0;
    lexer->cur_pos      = NULL;
    lexer->file_name    = NULL;
    
    ON_DEBUG( LOG(INFO, "Lexer is destroed"); )
    return FRONT_SUCCESS;
}


token_t *PeekToken(lexer_t* lexer)                      // maybe void
{
    if (IS_BAD_PTR(lexer->peeked_token))
    {
        const char* saved_pos = lexer->cur_pos;
        lexer->peeked_token = NextToken(lexer);
        lexer->cur_pos  = saved_pos;
    }

    return lexer->peeked_token;
}


frontErr_t AdvanceToken(lexer_t* lexer)
{
    ON_DEBUG( if (IS_BAD_PTR(lexer)) { LOG(ERROR, "Token didn't advance"); return FRONT_ERROR; } )
    
    token_t *next_token = NextToken(lexer);
    if (StackPush(lexer->tokens, next_token) != STK_SUCCESS) return FRONT_ERROR;
    
    lexer->cur_pos = next_token->start + next_token->length;
    lexer->cur_col += next_token->length;
    
    lexer->peeked_token = NULL;
    lexer->cur_token++;
    return FRONT_SUCCESS;
}


frontErr_t SkipSpaces(lexer_t* lexer)
{
    ON_DEBUG( if (IS_BAD_PTR(lexer)) return FRONT_ERROR; )

    while (!(IS_END(lexer)))
    {
        while (*lexer->cur_pos == ' ' || *lexer->cur_pos == '\t') { lexer->cur_pos++; lexer->cur_col++; }
        
        if (*lexer->cur_pos == '\0' || *lexer->cur_pos == '#')
        {
            lexer->cur_line++;
            if (lexer->cur_line < lexer->line_count)
            {
                lexer->cur_pos = lexer->lines[lexer->cur_line];
                lexer->cur_col = 1;
            } 
            else
                break;
        }
        else
            break;
    }

    return FRONT_SUCCESS;
}


token_t *NextToken(lexer_t* lexer)
{
    if (!IS_BAD_PTR(lexer->peeked_token)) { return lexer->peeked_token; }

    SkipSpaces(lexer);
    if (IS_END(lexer)) return NewToken(ARG_OP, HASH_EOF, "", 0, lexer->cur_line + 1, lexer->cur_col);
    
    const char* start = lexer->cur_pos;
    lexer->cur_col++;
    
    int num_digits = 0;
    sscanf(lexer->cur_pos, "%*g%n", &num_digits);
    if (num_digits) { return ReadNum(lexer); }

    hash_t id_hash = GetHash(lexer->cur_pos);
    keyword_t *key = (keyword_t*)bsearch(&id_hash, keyword_set, LEN_KEYWORD_SET, sizeof(keyword_t), CmpHashForBinSearch);
    if (key != NULL)
    {
        if (strncmp(lexer->cur_pos, key->name, (size_t)(key->len)) == 0)
        {
            return NewToken(ARG_OP, id_hash, start, key->len, lexer->cur_line, lexer->cur_col);
        }
    }

    id_hash = (hash_t)(*lexer->cur_pos);
    key = (keyword_t*)bsearch(&id_hash, keyword_set, LEN_KEYWORD_SET, sizeof(keyword_t), CmpHashForBinSearch);
    if (key != NULL)
    {
        if (strncmp(lexer->cur_pos, key->name, (size_t)(key->len)) == 0)
            return NewToken(ARG_OP, id_hash, start, key->len, lexer->cur_line, lexer->cur_col);
    }
    
    if (isalpha(*lexer->cur_pos)) { return ReadName(lexer); }
    
    lexer->cur_pos++;
    lexer->cur_col++;
    return NewToken(ARG_OP, HASH_UNDEF, start, 1, lexer->cur_line, lexer->cur_col);
}


static token_t *ReadNum(lexer_t *lexer)
{
    int tmp_num = 0.0;
    int num_digits = 0;
    if (sscanf(lexer->cur_pos, "%d%n", &tmp_num, &num_digits) != 1) return NULL;        //TODO: strtod

    token_t* token = NewToken(ARG_NUM, 0, lexer->cur_pos, num_digits, lexer->cur_line + 1, lexer->cur_col);
    
    lexer->cur_pos += num_digits;
    lexer->cur_col += num_digits;
    
    return token;
}


static token_t *ReadName(lexer_t *lexer)
{
    const char* start = lexer->cur_pos;
    int line = lexer->cur_line + 1;
    int col = lexer->cur_col;
    
    while (isalnum(*lexer->cur_pos) || *lexer->cur_pos == '_') { lexer->cur_pos++; lexer->cur_col++; }
    
    int length = (int)((size_t)lexer->cur_pos - (size_t)start);
    
    const char* saved_pos = lexer->cur_pos;
    int saved_col = lexer->cur_col;
    int saved_line = lexer->cur_line;
    
    token_t* next_token = PeekToken(lexer);
    
    lexer->cur_pos = saved_pos;
    lexer->cur_col = saved_col;
    lexer->cur_line = saved_line;

    char* tmp_str = strndup(start, (size_t)length);
    hash_t tmp_hash = GetHash(tmp_str);
    free(tmp_str);

    if (!IS_BAD_PTR(next_token) && next_token->type == ARG_OP && next_token->hash == HASH_LPAREN)
        return NewToken(ARG_FUNC, tmp_hash, start, length, line, col);
    else
        return NewToken(ARG_VAR, tmp_hash, start, length, line, col);
}


int CmpHashForBinSearch(const void *a, const void *b)
{
    const hash_t    *hash_a    = (const hash_t*)a;
    const keyword_t *keyword_b = (const keyword_t*)b;
    
    if (*hash_a > keyword_b->hash)
        return 1;
    if (*hash_a < keyword_b->hash)
        return -1;
    return 0;
}