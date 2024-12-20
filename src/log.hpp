#pragma once

#include <cstdarg>
#include <cstdio>

// Use "make debug" when compiling to make it available
inline void log(const char* fmt, ...)
{
    #ifdef DEBUG
    va_list args;
    va_start(args, fmt);
    std::printf("[LOG] ");
    std::vfprintf(stdout, fmt, args);
    va_end(args);
    #endif
}
