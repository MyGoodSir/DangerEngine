#pragma once

#include <sstream>
#include <iostream>
#include "LoggerFormatting.h"

#include "DangerConfig.h"

#ifdef DGR_DEBUG_MODE
//debug macros
#define HALT() std::abort();
#define STR(x) #x
#define DGR_ASSERT(x, msg) if(!x){ DGR_LOG_FATAL("ASSERT FAILED: ", STR(x), msg); HALT();}
#endif


//clogger = custom logger
//this is what actually prints the logs
template <typename... ArgTypes>
inline void clogger(ArgTypes... args)
{
    std::stringstream ss;
    using expand = int[];

    //little trick using a fold expression in an initializer list and abusing the comma operator to exaluate an expression for each argument
    //(void) just silences an "unused variable" compiler warning
    //braced initialization lists evaluate their arguments in order of appearance
    //comma operator in the context of (expression, value) means "Evaluate expression then return value"
    (void)expand {0, ((ss << args), void(), 0)...};
    printf(ss.str().c_str());
}
//base logger define
#define DGR_LOG(filename, func, line, color, fmt, ...)                           \
    clogger(color "DangerLog: [", filename, "] -> <", line, "> :: "); \
    clogger(color fmt RESET, ##__VA_ARGS__);                               \
    clogger("\n");

//cyan information log
#define DGR_LOG_INFO(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, CYN, fmt, ##__VA_ARGS__)

//yellow warning log
#define DGR_LOG_WARNING(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, YEL, fmt, ##__VA_ARGS__)

//red error log
#define DGR_LOG_ERROR(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, RED, fmt, ##__VA_ARGS__)

//bold-highlighted red fatal error log
#define DGR_LOG_FATAL(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, BHRED, fmt, ##__VA_ARGS__)

//bold-highlighted white confused log
#define DGR_LOG_UNDEFINED(fmt, ...) DGR_LOG(__FILE__, __FUNCSIG__, __LINE__, BHWHT, fmt, ##__VA_ARGS__)

