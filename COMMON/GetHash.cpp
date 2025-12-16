#include "GetHash.hpp"


hash_t GetHash(const char *ptr)
{    
    if (IS_BAD_PTR(ptr)) return 0;
    
    hash_t new_hash = 0;
    int renum = 0;
    while (renum < MAX_LEN_STR_FOR_HASH && *ptr != '\0' && !isspace(*ptr))
    {
        new_hash = (new_hash << 5) - new_hash + (hash_t)(*ptr++);
        renum++;
    }

    return new_hash;
}