#ifndef GET_HASH_HPP
#define GET_HASH_HPP

#include "CONFIG.hpp"

#include <stdio.h>
#include <ctype.h>
#include "IsBadPtr.hpp"

#define MAX_LEN_STR_FOR_HASH 8              // to avoid overflow

hash_t GetHash(const char *ptr);

#endif