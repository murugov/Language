#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <stdio.h>
#include <time.h>
#include "IsBadPtr.hpp"

extern FILE *LogFile;
extern const char* verdict_strings[];

enum log_verdict_t
{
    INFO  = 0x00,
    DEBUG = 0x01,
    WARN  = 0x02, 
    ERROR = 0x03
};

int LogFileOpener(const char* path);
int LogFileCloser();

#define LOG(verdict, ...) \
    do { \
        if (!IsBadPtr((void*)LogFile)) \
        { \
            time_t now = time(NULL); \
            struct tm *timeinfo = localtime(&now); \
            char time_str[20] = {0}; \
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo); \
            if (verdict == ERROR) \
                fprintf(LogFile, "[%s] %-5s %s:%d: ", time_str, verdict_strings[verdict], __FILE__, __LINE__); \
            else \
                fprintf(LogFile, "[%s] %-5s ", time_str, verdict_strings[verdict]); \
            fprintf(LogFile, __VA_ARGS__); \
            fprintf(LogFile, "\n"); \
            fflush(LogFile); \
        } \
    } while(0)

#define ERR_MSG_FORMAT(name) name " in %s:%d: from %s"
#define ERR_MSG_PARAMS __FILE__, __LINE__, __func__

#endif