#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <inttypes.h>

#define META_FILEPATH  "metadata"

// Index for TPMDOS_META struct
//~ #define G_RUN_CNT   1
#define L_TEST      2


#define PRINT(fmt, ...)  printf("[%s:%d] ", __FILE__, __LINE__); printf(fmt, ##__VA_ARGS__)

// Counters to record test times. (Used for restarting app. on failure)
typedef struct
{
    long long unsigned int g_run_cnt;
    long long unsigned int l_test;
} TPMDOS_META, *PTPMDOS_META;

// Increament coutners in the global <TPMDOS_META>
extern void IncGlobalMeta(int item);

// Read metafile if exist, otherwise all are 0. 
extern void ReadMeta();

#endif
