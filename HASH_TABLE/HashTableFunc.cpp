#include "hash_table.hpp"


htErr_t htInit(ht_t *hash_table, const char *name, const char *file, const char *func, int line)
{
    ON_DEBUG( if (IS_BAD_PTR(hash_table)) { LOG(ERROR, "BAD_HT_PTR"); return HT_ERROR; } )
    
    hash_table->id.name = name;
    hash_table->id.file = file;
    hash_table->id.func = func;
    hash_table->id.line = line;

    ON_DEBUG( LOG(DEBUG, "Created id for hash-table \"%s\"", hash_table->id.name); )
    return HT_SUCCESS;
}


htErr_t htCtor(ht_t *hash_table)
{
    ON_DEBUG( if (IS_BAD_PTR(hash_table)) { LOG(ERROR, "BAD_HT_PTR"); return HT_ERROR; } )
    
    hash_table->table = (table_t*)calloc(HT_SIZE, sizeof(table_t));
    if (IS_BAD_PTR(hash_table->table)) { return HT_ERROR; }
    
    for (int i = 0; i < HT_SIZE; ++i)
    {
        hash_table->table[i].is_used = 0;
        hash_table->table[i].stk = NULL;
    }
    
    ON_DEBUG( LOG(INFO, "Hash-table \"%s\" created", hash_table->id.name); )
    return HT_SUCCESS;
}


htErr_t htDtor(ht_t *hash_table)
{
    ON_DEBUG( if (IS_BAD_PTR(hash_table)) { LOG(ERROR, "BAD_HT_PTR"); return HT_ERROR; } )

    for (int i = 0; i < HT_SIZE; ++i)
    {
        if (hash_table->table[i].stk != NULL)
        {
            STACK_DTOR(hash_table->table[i].stk);
            free(hash_table->table[i].stk);
            hash_table->table[i].stk = NULL;
        }
        hash_table->table[i].is_used = 0;
    }
    free(hash_table->table);

    ON_DEBUG( LOG(INFO, "Hash-table \"%s\" destroed", hash_table->id.name); )
    
    hash_table->id.name = NULL;
    hash_table->id.file = NULL;
    hash_table->id.func = NULL;
    hash_table->id.line = 0;

    hash_table->table = NULL;
    free(hash_table);

    return HT_SUCCESS;
}


const char *htFind(ht_t *hash_table, const char *target)
{
    ON_DEBUG( if(IS_BAD_PTR(hash_table) || IS_BAD_PTR(target)) { return NULL; } )

    hash_t hash_target = GetHash(target) & (HT_SIZE - 1);

    if (hash_table->table[hash_target].is_used == 0) { return NULL; }
    
    stk_t<const char*> *stk = hash_table->table[hash_target].stk;

    for (ssize_t i = 0; i < stk->size; ++i)
    {
        const char* current_elem = stk->data[i];
        if (strcmp(target, current_elem) == 0)
        {
            return stk->data[i];
        }
    }

    return NULL;
}


htErr_t htInsert(ht_t *hash_table, const char *item)
{
    ON_DEBUG( if(IS_BAD_PTR(hash_table) || IS_BAD_PTR(item)) { return HT_ERROR; } )

    hash_t hash_item = GetHash(item) & (HT_SIZE - 1);

    if (hash_table->table[hash_item].is_used == 0)
    {
        hash_table->table[hash_item].is_used = 1;
        stk_t<const char*> *stk = (stk_t<const char*>*)calloc(1, sizeof(stk_t<const char*>));
        if (IS_BAD_PTR(stk))
        {
            for (int i = 0; i < HT_SIZE; ++i)
            {
                STACK_DTOR(hash_table->table[i].stk);
                if (hash_table->table[i].stk != NULL) { free(hash_table->table[i].stk); }
            }
            free(hash_table->table);
            return HT_ERROR;
        }
    
        STACK_CTOR(stk, INIT_NUM_COL);
        ON_DEBUG(
            if (stk->error != STK_NO_ERRORS)
            {
                LOG(ERROR, "Stack construction failed");
                free(stk);
                return HT_ERROR;
            }
        )
        hash_table->table[hash_item].stk = stk;
    }

    StackPush(hash_table->table[hash_item].stk, item);

    return HT_SUCCESS;
}


htErr_t htRemove(ht_t *hash_table, const char *item)
{
    ON_DEBUG( if(IS_BAD_PTR(hash_table) || IS_BAD_PTR(item)) { return HT_ERROR; } )

    hash_t hash_item = GetHash(item) & (HT_SIZE - 1);

    if (hash_table->table[hash_item].is_used == 0) { return HT_ERROR; }

    stk_t<const char*> stk_ret = {};
    STACK_CTOR(&stk_ret, INIT_NUM_COL);
    if (IS_BAD_PTR(&stk_ret)) { return HT_ERROR; }

    bool found = false;
    const char *current_elem = NULL;
    
    while (hash_table->table[hash_item].stk->size > 0)
    {
        StackPop(hash_table->table[hash_item].stk, &current_elem);
        
        if (strcmp(item, current_elem) == 0)
        {
            found = true;
            break;
        }
        else
        {
            StackPush(&stk_ret, current_elem);
        }
    }

    while (stk_ret.size > 0)
    {
        StackPop(&stk_ret, &current_elem);     
        StackPush(hash_table->table[hash_item].stk, current_elem);
    }

    STACK_DTOR(&stk_ret);

    if (hash_table->table[hash_item].stk->size == 0)
    {
        hash_table->table[hash_item].is_used = 0;
        STACK_DTOR(hash_table->table[hash_item].stk);
        hash_table->table[hash_item].stk = NULL;
    }

    return found ? HT_SUCCESS : HT_ERROR;
}