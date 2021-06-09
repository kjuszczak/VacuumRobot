#include "log.h"

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <mqueue.h>

#include "logThread.h"

void logText(const uint8_t lvl,
             const char* file,
             const unsigned int line, 
             const char* fmt, ...)
{
    loggerStruct logger;
    int num = logAddHeader(logger.buffer, lvl, file, line);
    if (num)
    {
        va_list args;
        va_start(args, fmt);
        num += vsnprintf(&logger.buffer[num], LG_MAX_BUF_LEN - num, fmt, args);
        num += snprintf(&logger.buffer[num], 2, "\n");
        va_end(args);
        logger.num = num;
        mq_send(loggerMQueue, (const char *)&logger, sizeof(loggerStruct), 0);
    }
}

int logAddHeader(logBuff buff, const uint8_t lvl, const char* file, const unsigned int line)
{
    struct timespec timer;
    struct tm t;
    if (clock_gettime(CLOCK_REALTIME, &timer))
    {
        return 0;
    }
    if (localtime_r(&(timer.tv_sec), &t) == NULL)
    {
        return 0;
    }
    int bytes = strftime(buff, LG_MAX_BUF_LEN, "%F %T", &t);
    if (bytes)
    {
        bytes += snprintf(&buff[bytes], LG_MAX_BUF_LEN - bytes - 1, ".%09lu [%s] %s %s[%i] ", timer.tv_nsec, componentStr, logLvlAsString(lvl), file, line);
    }
    return bytes;
}

const char* logLvlAsString(const uint8_t lvl)
{
    static const char* lvlAsString[4] = {"INF", "WRN", "ERR", "invalid"};
    if (lvl < 3)
    {
        return lvlAsString[lvl];
    }
    return lvlAsString[3];
}
