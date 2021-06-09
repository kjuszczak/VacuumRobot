#ifndef LOG_THREAD_H
#define LOG_THREAD_H

#include <mqueue.h>

extern mqd_t loggerMQueue;
extern const char* componentStr;

int initLogger(const char* cmpStr);
void createFile();
void *tLoggerThreadFunc(void *cookie);

#endif // LOG_THREAD_H
