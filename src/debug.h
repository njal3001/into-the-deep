#pragma once
#include <iostream>

namespace ITD {

#ifndef NDEBUG
#include <stdlib.h>
#    define ITD_ASSERT(condition, message)                                   \
        do                                                                   \
        {                                                                    \
            if (!(condition))                                                \
            {                                                                \
                Log::error("%s\n\tin %s:%d", (message), __FILE__, __LINE__); \
                abort();                                                     \
            }                                                                \
        } while (0)
#else
#    define ITD_ASSERT(condition, message)
#endif

namespace Log {

#define MESSAGE_MAX 1024

    void info(const char *format, ...);
    void warn(const char *format, ...);
    void error(const char *format, ...);

}  // namespace Log
}  // namespace ITD
