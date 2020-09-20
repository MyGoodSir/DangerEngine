#pragma once

#include <sstream>
#include <iostream>
#include "LoggerFormatting.h"

#define DGR_DEBUG_MODE

#ifdef DGR_DEBUG_MODE
//debug macros
#define HALT() std::abort();
#define STR(x) #x
#define DGR_ASSERT(x, y) if(!x){ DGR_LOG_FATAL("ASSERT FAILED: ", STR(x), y); HALT();}
#endif


template <typename... ArgTypes>
inline void clogger(ArgTypes... args)
{
    std::stringstream ss;
    using expand_variadic_pack = int[];
    
    (void)expand_variadic_pack{0, ((ss << args), void(), 0)...};
    printf(ss.str().c_str());
}
#define DGR_LOG(filename, func, line, color, fmt, ...)                           \
    clogger(color "DangerLog: [", filename, "] -> <", line, "> :: "); \
    clogger(color fmt RESET, ##__VA_ARGS__);                               \
    clogger("\n");

#define DGR_LOG_INFO(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, CYN, fmt, ##__VA_ARGS__)

#define DGR_LOG_WARNING(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, YEL, fmt, ##__VA_ARGS__)

#define DGR_LOG_ERROR(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, RED, fmt, ##__VA_ARGS__)

#define DGR_LOG_FATAL(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, BHRED, fmt, ##__VA_ARGS__)

#define DGR_LOG_UNDEFINED(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, BHWHT, fmt, ##__VA_ARGS__)

