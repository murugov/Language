#include "stack.h"

#ifndef HASH_H
#define HASH_H

typedef size_t hash_t;

extern hash_t *hash_arr; // разобраться с этим
static size_t hash_arr_cap = 0;
static size_t hash_arr_size = 0;

template <typename T>
hash_t HashFunc(stk_t<T> *stk);

hash_t HashCmd(char *buffer);

StackErr_t HashArrCtor();

template <typename T>
StackErr_t HashAdd(stk_t<T> *stk);

StackErr_t HashArrRealloc();

StackErr_t HashArrDtor();

#include "../HashFunc.hpp"

#endif