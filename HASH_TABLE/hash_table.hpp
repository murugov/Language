#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <string.h>
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

template <typename htElem_T>
struct table_t
{
    int              is_used;
    stk_t<htElem_T>* stk;
};

template <typename htElem_T>
struct ht_t
{
    ht_id              id;
    table_t<htElem_T>* table;
};


template <typename htElem_T>
htErr_t htInit(ht_t<htElem_T> *hash_table, const char *name, const char *file, const char *func, int line);

template <typename htElem_T>
htErr_t htCtor(ht_t<htElem_T> *hash_table);

template <typename htElem_T>
htErr_t htDtor(ht_t<htElem_T> *hash_table);

template <typename htElem_T>
htElem_T htFind(ht_t<htElem_T> *hash_table, htElem_T *item, const char* (*htElemToStr)(const void*));

template <typename htElem_T>
htErr_t htInsert(ht_t<htElem_T> *hash_table, htElem_T *item, const char* (*htElemToStr)(const void*));

template <typename htElem_T>
htErr_t htRemove(ht_t<htElem_T> *hash_table, htElem_T *item, const char* (*htElemToStr)(const void*));

#define HT_CTOR(hash_table) htInit(hash_table, #hash_table, __FILE__, __func__, __LINE__); htCtor(hash_table)
#define HT_DTOR(hash_table) htDtor(hash_table)

#include "HashTableFunc.hpp"

#endif