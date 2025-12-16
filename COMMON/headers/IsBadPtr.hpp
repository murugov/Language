#ifndef IS_BAD_PTR_HPP
#define IS_BAD_PTR_HPP

#include <stdio.h>
#include <stdbool.h>
#include <mach/mach.h>
#include <mach/mach_vm.h>
#include <mach/vm_map.h>

#ifdef __APPLE__
    #include <mach/mach.h>
    #include <mach/mach_vm.h>

#elif defined(_WIN32)
    #include <windows.h>

#elif defined(__linux__) || defined(__unix__)
    #include <unistd.h>
    #include <sys/mman.h>
    
#endif

bool IsBadPtr(void* ptr);

#define IS_BAD_PTR(ptr) IsBadPtr((void*)ptr)

#endif