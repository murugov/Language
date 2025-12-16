#include "hash.h"

#ifndef HASH_FUNC_HPP
#define HASH_FUNC_HPP

template <typename T>
hash_t HashFunc(stk_t<T> *stk)
{
    hash_t new_hash = 0;

    if (IS_BAD_PTR(stk) || IS_BAD_PTR(stk->data))
        return new_hash;

    for (size_t i = 0; i < (size_t)stk->capacity; ++i)
        new_hash = (new_hash << 5) - new_hash + (hash_t)(stk->data[i]);
    
    return new_hash;
}

template <typename stk_elem_t>
StackErr_t HashAdd(stk_t<stk_elem_t> *stk)
{
    if(HashArrRealloc())
        return STK_ERROR;

    hash_arr[hash_arr_size] = HashFunc(stk);
    stk->hash_index = hash_arr_size;
    hash_arr_size++;

    return STK_SUCCESS;
}

#endif