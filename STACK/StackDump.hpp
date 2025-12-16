#include "stack.hpp"

#ifndef STACK_DUMP_HPP
#define STACK_DUMP_HPP

template <typename stackElem_T>
StackErr_t StackDump(stk_t<stackElem_T>* ON_DEBUG(stk), const char* ON_DEBUG(file), const char* ON_DEBUG(func), int ON_DEBUG(line))
{
    ON_DEBUG( if (IS_BAD_PTR(stk)) { LOG(ERROR, "STK_BAD_STK_PTR"); return STK_ERROR; } )

    #define PRINT_ERRCODE_(code) do { if (ERR_CHECK(code) == STK_ERROR) LOG(ERROR, #code); } while(0)

    ON_DEBUG(
        // Вывод кодов ошибок
        PRINT_ERRCODE_(STK_WRONG_CANARY);
        PRINT_ERRCODE_(STK_WRONG_HASH);
        PRINT_ERRCODE_(STK_BAD_DATA_PTR);
        PRINT_ERRCODE_(STK_WRONG_SIZE);
        PRINT_ERRCODE_(STK_WRONG_CAPACITY);
        PRINT_ERRCODE_(STK_WRONG_REALLOC);
        PRINT_ERRCODE_(STK_WRONG_REV_REALLOC);
        PRINT_ERRCODE_(STK_WRONG_POISON_VAL);
        PRINT_ERRCODE_(STK_DATA_OVERFLOW);
        PRINT_ERRCODE_(STK_ACCESS_EMPTY_DATA);
        PRINT_ERRCODE_(STK_ACCESS_EMPTY_DATA);
        
        if (ERR_CHECK(STK_BAD_DATA_PTR) == STK_NO_ERRORS && stk->capacity > 0)
        {
            fprintf(LogFile, "---------------------------------------------------------------------------------------------------------------------------\n");
    
            char str_value[32] = {0};
    
            fprintf(LogFile, "DUMP stack \"%s\" [%p] from %s() at %s:%d:\n\n", 
                    stk->id.name, stk, func, file, line);
    
            fprintf(LogFile, "STK_POISON = ");
            snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(STK_POISON), STK_POISON);
            fprintf(LogFile, "%s\n\n", str_value);
    
            fprintf(LogFile, "STK_CANARY_1 = 0x%zX\n", (size_t)STK_CANARY_1);
            fprintf(LogFile, "STK_CANARY_2 = 0x%zX\n\n", (size_t)STK_CANARY_2);  
    
            fprintf(LogFile, "%s {\n", stk->id.name);
    
            // Канарейка 1
            snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(stk->canary_1), stk->canary_1);
            fprintf(LogFile, "\tcanary_1 = %s; %s\n", str_value, 
                    (stk->canary_1 == STK_CANARY_1) ? "(OK)" : "(WRONG!)");
    
            fprintf(LogFile, "\tdata[%p] = {\n", stk->data);
    
            // Вывод данных стека
            for (ssize_t i = 0; i < stk->capacity; ++i)
            {
                snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(stk->data[i]), stk->data[i]);
                
                const char* annotation = "";
                if (i < stk->size) {
                    annotation = "(value)";
                } else {
                    annotation = (stk->data[i] == STK_POISON) ? "(POISON)" : "(CORRUPTED!)";
                }
                
                fprintf(LogFile, "\t\t[%zd] = %s; %s\n", i, str_value, annotation);
                
                // Ограничиваем вывод для больших стеков
                if (i >= 50 && i < stk->capacity - 5) {
                    fprintf(LogFile, "\t\t... (%zd more elements) ...\n", stk->capacity - i - 5);
                    i = stk->capacity - 6;
                }
            }
    
            fprintf(LogFile, "\t};\n");
    
            fprintf(LogFile, "\tsize = %zd; %s\n", stk->size,
                    (stk->size >= 0 && stk->size <= stk->capacity) ? "(OK)" : "(WRONG!)");
                    
            fprintf(LogFile, "\tcapacity = %zd; %s\n", stk->capacity,
                    (stk->capacity >= 0) ? "(OK)" : "(WRONG!)");
                    
            fprintf(LogFile, "\thash = 0x%zx; %s\n", stk->hash,
                    (StkHashFunc(stk) == stk->hash) ? "(OK)" : "(WRONG!)");
    
            // Канарейка 2
            snprintf(str_value, sizeof(str_value), FORMAT_SPECIFIER(stk->canary_2), stk->canary_2);
            fprintf(LogFile, "\tcanary_2 = %s; %s\n", str_value,
                    (stk->canary_2 == STK_CANARY_2) ? "(OK)" : "(WRONG!)");
    
            // Информация о создании стека
            if (stk->id.name) {
                fprintf(LogFile, "\n\tCreated at: %s:%d in %s()\n",
                        stk->id.file, stk->id.line, stk->id.func);
            }
    
            fprintf(LogFile, "}\n");
    
            // Статистика
            fprintf(LogFile, "\nStack statistics:\n");
            fprintf(LogFile, "- Fill rate: %.1f%% (%zd/%zd)\n",
                    (stk->capacity > 0) ? (100.0 * stk->size / stk->capacity) : 0.0,
                    stk->size, stk->capacity);
            fprintf(LogFile, "- Element size: %zu bytes\n", sizeof(stackElem_T));
            fprintf(LogFile, "- Total memory: %zu bytes\n", 
                    sizeof(*stk) + (size_t)stk->capacity * sizeof(stackElem_T));
            
            fprintf(LogFile, "---------------------------------------------------------------------------------------------------------------------------\n");
        }
        else
        {
            // Дамп даже при плохом указателе на данные
            fprintf(LogFile, "---------------------------------------------------------------------------------------------------------------------------\n");
            fprintf(LogFile, "DUMP stack \"%s\" [%p] from %s() at %s:%d:\n\n", 
                    stk->id.name ? stk->id.name : "UNKNOWN", stk, func, file, line);
            fprintf(LogFile, "ERROR: Bad data pointer (%p)\n", stk->data);
            fprintf(LogFile, "size = %zd, capacity = %zd\n", stk->size, stk->capacity);
            fprintf(LogFile, "---------------------------------------------------------------------------------------------------------------------------\n");
        }
    )

    #undef PRINT_ERRCODE_
        
    return STK_SUCCESS;
}

#endif