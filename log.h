#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <stdint.h>

#define PRINT(fmt, ...)  printf("[%s:%d] ", __FILE__, __LINE__); printf(fmt, ##__VA_ARGS__)

// Counters to record test times. (Used for restarting app. on failure)
typedef struct
{
    uint64_t g_run_cnt;
    
} TPMDOS_META, *PTPMDOS_META;

TPMDOS_META g_meta;

#endif
