#include "hash.h"

hash_t *hash_arr = {};

StackErr_t HashArrCtor()
{
    if (hash_arr_cap == 0)
    {    
        hash_arr = (hash_t*)calloc(32, sizeof(hash_t));

        if (IS_BAD_PTR(hash_arr))
            return STK_ERROR;

        hash_arr_cap = 32;
        hash_arr_size = 0;

    }
    
    return STK_SUCCESS;
}

StackErr_t HashArrRealloc()
{
    if (hash_arr_size + 1 >= hash_arr_cap)
    {
        hash_arr_cap = hash_arr_cap << 1;
        hash_t *new_arr = (hash_t*)realloc(hash_arr, hash_arr_cap * sizeof(hash_t));

        if(IS_BAD_PTR(new_arr))
            return STK_ERROR;

        for (size_t i = hash_arr_cap / 2; i < hash_arr_cap; ++i)
            new_arr[i] = 0;

        hash_arr = new_arr;
    }

    return STK_SUCCESS;
}

StackErr_t HashArrDtor()
{
    if(IS_BAD_PTR(hash_arr))
            return STK_ERROR;

    free(hash_arr);

    return STK_SUCCESS;
}