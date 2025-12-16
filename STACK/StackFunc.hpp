#include "stack.hpp"

#ifndef STACK_FUNC_HPP
#define STACK_FUNC_HPP


template <typename stackElem_T>
StackErr_t StackInit(stk_t<stackElem_T> *stk, const char *name, const char *file, const char *func, int line)
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )
    
    (*stk).id.name = name;
    (*stk).id.file = file;
    (*stk).id.func = func;
    (*stk).id.line = line;

    ON_DEBUG( LOG(DEBUG, "Created id for stack \"%s\"", stk->id.name); )
    return STK_SUCCESS;
}


template <typename stackElem_T>
StackErr_t StackCtor(stk_t<stackElem_T> *stk, ssize_t capacity)
{   
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )

    if (capacity < 0)
    { 
        ON_DEBUG( stk->error = STK_WRONG_CAPACITY; LOG(ERROR, "STK_WRONG_CAPACITY") ); 
        return STK_ERROR; 
    }
    
    stk->canary_1 = STK_CANARY_1;
    stk->size = 0;
    stk->capacity = capacity;

    stk->data = (stackElem_T*)calloc((size_t)stk->capacity, sizeof(stackElem_T));
    if (IS_BAD_PTR(stk->data))
    { 
        ON_DEBUG( stk->error |= STK_BAD_DATA_PTR; LOG(ERROR, "STK_BAD_DATA_PTR"); ) 
        return STK_ERROR; 
    }

    for (ssize_t i = 0; i < stk->capacity; ++i) { stk->data[i] = STK_POISON; }

    ON_DEBUG(
        stk->hash = StkHashFunc(stk);
        if(stk->hash == 0) { 
            stk->error |= STK_WRONG_HASH; 
            LOG(ERROR, "STK_WRONG_HASH"); 
            return STK_ERROR; 
        }
        LOG(INFO, "Hash updated");
    )

    stk->canary_2 = STK_CANARY_2;

    ON_DEBUG( LOG(INFO, "Stack \"%s\" is created", stk->id.name); )
    return STK_SUCCESS;
}


template <typename stackElem_T>
StackErr_t StackDtor(stk_t<stackElem_T> *stk)
{
    if (stk == NULL) { return STK_SUCCESS; }

    ON_DEBUG( 
        if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; }
        if (IS_BAD_PTR(stk->data)) { LOG(ERROR, "STK_BAD_DATA_PTR"); return STK_ERROR; }
    )

    free(stk->data);
    stk->data = NULL;
    stk->size = -1;
    stk->capacity = -1;
    
    ON_DEBUG(
        stk->error = STK_DESTROYED;
        stk->hash = 0;
        stk->canary_1 = 0;
        stk->canary_2 = 0;
    )
    
    ON_DEBUG( LOG(INFO, "Stack \"%s\" is destroyed", stk->id.name); )
    
    stk->id.name = NULL;
    stk->id.file = NULL;
    stk->id.func = NULL;
    stk->id.line = 0;

    return STK_SUCCESS;
}


template <typename stackElem_T>
StackErr_t StackPush(stk_t<stackElem_T> *stk, const stackElem_T value)
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )
    ON_ERR_FIND( if (ERR_DETECT(stk, STK_PUSH)) { return STK_ERROR; } )

    if (stk->size >= stk->capacity)
    {
        if (StackRealloc(stk) == STK_ERROR)
        {
            ON_DEBUG(
                stk->error |= STK_WRONG_REALLOC;
                LOG(ERROR, "STK_WRONG_REALLOC");
            )
            return STK_ERROR; 
        }
    }

    stk->data[stk->size] = value;
    stk->size++;

    ON_DEBUG( 
        char str_value[32] = {0};
        snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(value), value);
        LOG(DEBUG, "PUSH %s in \"%s\"", str_value, stk->id.name);
    )

    ON_DEBUG(
        stk->hash = StkHashFunc(stk);
        if(stk->hash == 0)
        { 
            stk->error |= STK_WRONG_HASH; 
            LOG(ERROR, "STK_WRONG_HASH"); 
            return STK_ERROR; 
        }
        LOG(INFO, "Hash is updated");
    )

    return STK_SUCCESS;
}

template <typename stackElem_T>
StackErr_t StackPop(stk_t<stackElem_T> *stk, stackElem_T *last_value)
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )
    ON_ERR_FIND( if (ERR_DETECT(stk, STK_POP)) { return STK_ERROR; } )

    if (stk->size == 0)
    {
        ON_DEBUG(
            stk->error |= STK_ACCESS_EMPTY_DATA;
            LOG(ERROR, "STK_ACCESS_EMPTY_DATA");
        )
        return STK_ERROR;
    }

    if (stk->size - 1 < stk->capacity / 2 && stk->capacity / 2 > MIN_STK_LEN)
    {
        if (StackRevRealloc(stk) == STK_ERROR)
        {
            ON_DEBUG(
                stk->error |= STK_WRONG_REV_REALLOC;
                LOG(ERROR, "STK_WRONG_REV_REALLOC");
            )
            return STK_ERROR; 
        }
    }

    stk->size--;
    *last_value = stk->data[stk->size];
    stk->data[stk->size] = STK_POISON;

    ON_DEBUG(
        stk->hash = StkHashFunc(stk);
        if(stk->hash == 0)
        { 
            stk->error |= STK_WRONG_HASH; 
            LOG(ERROR, "STK_WRONG_HASH"); 
            return STK_ERROR; 
        }
        LOG(INFO, "Hash is updated");
    )

    ON_DEBUG(
        char str_value[32] = {0};
        snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(*last_value), *last_value);
        LOG(DEBUG, "POP %s from \"%s\"", str_value, stk->id.name);
    )

    return STK_SUCCESS;
}

template <typename stackElem_T>
StackErr_t StackRealloc(stk_t<stackElem_T> *stk)
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )
    ON_ERR_FIND( if (ERR_DETECT(stk, STK_REALLOC)) { LOG(ERROR, "STK_WRONG_REALLOC"); { return STK_ERROR; } } )

    ssize_t new_capacity = stk->capacity << 1;
    if (new_capacity <= stk->capacity)
    {
        ON_DEBUG( stk->error |= STK_WRONG_REALLOC; LOG(ERROR, "STK_WRONG_REALLOC"); )
        return STK_ERROR;
    }
    
    stackElem_T *new_data = (stackElem_T*)realloc(stk->data, (size_t)new_capacity * sizeof(stackElem_T));
    if(IS_BAD_PTR(new_data))
    { 
        ON_DEBUG( stk->error |= STK_WRONG_REALLOC; LOG(ERROR, "STK_WRONG_REALLOC"); ) 
        return STK_ERROR; 
    }

    for (ssize_t i = stk->capacity; i < new_capacity; ++i) { new_data[i] = STK_POISON; }
    
    stk->capacity = new_capacity;
    stk->data     = new_data;

    ON_DEBUG(
        LOG(INFO, "Stack \"%s\" is reallocated", stk->id.name);
        stk->hash = StkHashFunc(stk);
        if(stk->hash == 0)
        { 
            stk->error |= STK_WRONG_HASH; 
            LOG(ERROR, "STK_WRONG_HASH"); 
            return STK_ERROR; 
        }
        LOG(INFO, "Hash is updated");
    )

    return STK_SUCCESS;
}

template <typename stackElem_T>
StackErr_t StackRevRealloc(stk_t<stackElem_T> *stk)
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )
    ON_ERR_FIND( if (ERR_DETECT(stk, STK_REV_REALLOC)) { LOG(ERROR, "STK_WRONG_REV_REALLOC"); return STK_ERROR; } )

    ssize_t new_capacity = stk->capacity >> 1;
    if (new_capacity < MIN_STK_LEN) { new_capacity = MIN_STK_LEN; }
    
    if (new_capacity < stk->size) { ON_DEBUG( LOG(WARN, "Cannot shrink below current size"); ) return STK_SUCCESS; }
    
    stackElem_T *new_data = (stackElem_T*)realloc(stk->data, (size_t)new_capacity * sizeof(stackElem_T));
    if(IS_BAD_PTR(new_data))
    { 
        stk->error |= STK_WRONG_REV_REALLOC; 
        LOG(ERROR, "STK_WRONG_REV_REALLOC"); 
        return STK_ERROR; 
    }

    stk->capacity = new_capacity;
    stk->data     = new_data;

    ON_DEBUG(
        LOG(INFO, "Stack \"%s\" is reverse reallocated", stk->id.name);
        stk->hash = StkHashFunc(stk);
        if(stk->hash == 0)
        { 
            stk->error |= STK_WRONG_HASH; 
            LOG(ERROR, "STK_WRONG_HASH"); 
            return STK_ERROR; 
        }
        LOG(INFO, "Hash is updated");
    )

    return STK_SUCCESS;
}

template <typename stackElem_T>
stk_hash_t StkHashFunc(stk_t<stackElem_T> *stk)
{
    ON_DEBUG( if (IS_BAD_PTR(stk) || IS_BAD_PTR(stk->data)) { return 0; } )
    
    stk_hash_t new_hash = 0;

    for (ssize_t i = 0; i < stk->size; ++i)
    {
        new_hash = (new_hash << 5) - new_hash + (stk_hash_t)(stk->data[i]);
    }

    new_hash = (new_hash << 5) - new_hash + (stk_hash_t)(stk->size);
    new_hash = (new_hash << 5) - new_hash + (stk_hash_t)(stk->capacity);

    return new_hash;
}

#endif