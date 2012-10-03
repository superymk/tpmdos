#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#define PRINT(fmt, ...)  printf("[%s:%d] ", __FILE__, __LINE__); printf(fmt, ##__VA_ARGS__)

#endif
