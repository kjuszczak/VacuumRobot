#ifndef LOG_H
#define LOG_H

#include <stdint.h>

#define LG_MAX_BUF_LEN 1000
typedef char logBuff[LG_MAX_BUF_LEN];

typedef struct logger
{
    logBuff buffer;
    int num;
} loggerStruct;

#define LG_LVL_INF 0
#define LG_LVL_WRN 1
#define LG_LVL_ERR 2

extern void logText(const uint8_t lvl,
             const char* file,
             const unsigned int line,
             const char* fmt, ...);

#define LOG(LVL, ...) logText(LVL, __FILE__, __LINE__, __VA_ARGS__)

#define LG_INF(...) LOG(LG_LVL_INF, __VA_ARGS__)
#define LG_WRN(...) LOG(LG_LVL_WRN, __VA_ARGS__)
#define LG_ERR(...) LOG(LG_LVL_ERR, __VA_ARGS__)

int logAddHeader(logBuff buff, const uint8_t lvl, const char* file, const unsigned int line);
const char* logLvlAsString(const uint8_t lvl);

#endif // LOG_H
