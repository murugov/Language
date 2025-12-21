#include "AstParser.hpp"
#include "KeywordSet.cpp"

static int CmpHashForBinSearch(const void *a, const void *b);


void AstWriter(node_t* node, FILE* stream)
{
    if (IS_BAD_PTR(node)) { fprintf (stream, " nil"); return; }

    fprintf (stream, "(");
    
    keyword_t* keyword = NULL;
    
    switch (node->type)
    {
        case ARG_LINK:
            fprintf (stream, "LINK: ");
            fprintf (stream, "\"@\" ");
            break;
        
        case ARG_NUM:
            fprintf (stream, "NUM: ");
            fprintf (stream, "\"%d\" ", node->item.num);
            break;

        case ARG_OP:
            fprintf (stream, "OP: ");
            keyword = (keyword_t*)bsearch(&(node->item.op), keyword_set, LEN_KEYWORD_SET, sizeof(keyword_t), CmpHashForBinSearch);
            fprintf (stream, "\"%s\" ", keyword->c_name);
            break;
        
        case ARG_VAR:
            fprintf (stream, "VAR: ");
            fprintf (stream, "\"%s\" ", node->item.var);
            break;

        case ARG_FUNC:
            fprintf (stream, "FUNC: ");
            fprintf (stream, "\"%s\" ", node->item.func);
            break;

        default:
            fprintf (stream, "unknown: ");
            break;
    }

    AstWriter(node->left, stream);
    AstWriter(node->right, stream);

    fprintf (stream, ")");
}


static int CmpHashForBinSearch(const void *a, const void *b)
{
    const hash_t    *hash_a    = (const hash_t*)a;
    const keyword_t *keyword_b = (const keyword_t*)b;
    
    if (*hash_a > keyword_b->hash)
        return 1;
    if (*hash_a < keyword_b->hash)
        return -1;
    return 0;
}