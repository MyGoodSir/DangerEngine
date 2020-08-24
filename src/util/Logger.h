/*\
 *
 * Logging helper functions
 * TODO: make this less garbage
 * 
\*/
#pragma once

#include <iostream>
#include "LoggerFormatting.h"

#ifdef DGR_DEBUG_MODE
//debug macros
#endif

namespace DGREngine::util::debug
{
    template <typename... ArgTypes>
    inline void clogger(ArgTypes... args)
    {
        
        using expand_variadic_pack = int[];
        
        (void)expand_variadic_pack{0, ((std::cout << args), void(), 0)...};
    }

    using namespace formatting;
#define DGR_LOG(filename, line, color, fmt, ...)                           \
    clogger("\e" color "DangerLog: [", filename, "] -> <", line, "> :: "); \
    clogger(color fmt RESET, ##__VA_ARGS__);                               \
    clogger("\n");

#define DGR_LOG_INFO(fmt, ...) DGR_LOG(__FILE__, __LINE__, CYN, fmt, ##__VA_ARGS__)

#define DGR_LOG_WARNING(fmt, ...) DGR_LOG(__FILE__, __LINE__, YEL, fmt, ##__VA_ARGS__)

#define DGR_LOG_ERROR(fmt, ...) DGR_LOG(__FILE__, __LINE__, RED, fmt, ##__VA_ARGS__)

#define DGR_LOG_FATAL(fmt, ...) DGR_LOG(__FILE__, __LINE__, BHRED, fmt, ##__VA_ARGS__)

#define DGR_LOG_UNDEFINED(fmt, ...) DGR_LOG(__FILE__, __LINE__, BHWHT, fmt, ##__VA_ARGS__)
}
