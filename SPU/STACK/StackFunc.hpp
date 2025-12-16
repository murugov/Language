#include "stack.h"
#include "hash.h"

#ifndef STACK_FUNC_HPP
#define STACK_FUNC_HPP

//template <typename stackElem_T>
template <typename T>
StackErr_t StackInit(stk_t<T> *stk, const char *name, const char *file, const char *func, size_t line)
{
    if (IS_BAD_PTR(stk))
        return STK_ERROR;
    
    (*stk).id.name = name;
    (*stk).id.file = file;
    (*stk).id.func = func;
    (*stk).id.line = line;

    return STK_SUCCESS;
}


template <typename T>
StackErr_t StackCtor(stk_t<T> *stk, cap_t capacity)
{   
    stk->size = 2;
    stk->capacity = capacity + 2;

    if (ERR_DETECT(stk, STK_CTOR))
        return STK_ERROR;
    
    stk->data = (T*)calloc((size_t)capacity + 2, sizeof(T));

    if (ERR_DETECT(stk, STK_CTOR))
        return STK_ERROR;

    stk->data[0] = CANARY_3;
    stk->data[stk->capacity - 1] = CANARY_4;

    if(HashArrCtor())
    {
        stk->error = BAD_HASH_ARR_PTR;
        StackDump(stk, __FILE__, __func__, __LINE__);
        
        return STK_ERROR;
    }

    if (HashAdd(stk))
    {
        stk->error = WRONG_HASH;
        StackDump(stk, __FILE__, __func__, __LINE__);
        
        return STK_ERROR;
    }

    return STK_SUCCESS;
}


template <typename T>
StackErr_t StackPush(stk_t<T> *stk, const T value)
{
    if (ERR_DETECT(stk, STK_PUSH))
        return STK_ERROR;

    if (StackRealloc(stk))
        return STK_ERROR;

    if (ERR_DETECT(stk, STK_PUSH))
        return STK_ERROR;

    stk->data[stk->size - 1] = value;
    stk->size++;

    hash_arr[stk->hash_index] = HashFunc(stk);

    return STK_SUCCESS;
}


template <typename T>
StackErr_t StackPop(stk_t<T> *stk, T *last_value)
{
    stk->size--;

    if (ERR_DETECT(stk, STK_POP))
        return STK_ERROR;
    
    *last_value = stk->data[stk->size - 1];
    stk->data[stk->size - 1] = '\0';

    hash_arr[stk->hash_index] = HashFunc(stk);

    return STK_SUCCESS;
}


template <typename T>
StackErr_t StackRealloc(stk_t<T> *stk)
{
    if (ERR_DETECT(stk, STK_CTOR))
        return STK_ERROR;

    if (stk->size + 1 >= stk->capacity)
    {
        stk->capacity = stk->capacity << 1;
        T *new_data = (T*)realloc(stk->data, (size_t)stk->capacity * sizeof(T));

        if(IS_BAD_PTR(new_data))
        {
            stk->error = WRONG_REALLOC;
            StackDump(stk, __FILE__, __func__, __LINE__);

            return STK_ERROR;
        }

        for (size_t i = (size_t)(stk->capacity / 2) - 1; i < (size_t)stk->capacity - 1; ++i)
            new_data[i] = '\0';
        
        new_data[(size_t)stk->capacity - 1] = CANARY_4;

        stk->data = new_data;

        hash_arr[stk->hash_index] = HashFunc(stk);
    }

    return STK_SUCCESS;
}

template <typename T>
StackErr_t StackDtor(stk_t<T> *stk)
{
    if (ERR_DETECT(stk, STK_DTOR))
        return STK_ERROR;

    free(stk->data);

    return STK_SUCCESS;
}

#endif