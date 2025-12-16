#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include "stack.hpp"
#include "GetHash.hpp"

#define HT_SIZE      1024
#define INIT_NUM_COL 8


enum htErr_t
{
    HT_SUCCESS = 0,
    HT_ERROR   = 1
};

struct ht_id
{
    const char* name;
    const char* file;
    const char* func;
    int         line;
};

struct table_t
{
    int                 is_used;
    stk_t<const char*>* stk;
};

struct ht_t
{
    ht_id    id;
    table_t* table;
};


htErr_t htInit(ht_t *hash_table, const char *name, const char *file, const char *func, int line);
htErr_t htCtor(ht_t *hash_table);
htErr_t htDtor(ht_t *hash_table);

const char *htFind(ht_t *hash_table, const char *target);
htErr_t htInsert(ht_t *hash_table, const char *item);
htErr_t htRemove(ht_t *hash_table, const char *item);

#define HT_CTOR(hash_table) htInit(hash_table, #hash_table, __FILE__, __func__, __LINE__); htCtor(hash_table)
#define HT_DTOR(hash_table) htDtor(hash_table)


#endif