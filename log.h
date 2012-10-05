/* 
 * log.h -- Well, it controls how to log and what to log. And it include 
 * fail safe stuffs to recover execution in case of interruption.
 */

#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>
#include <inttypes.h>

#ifdef DEBUG
#define PRINT(fmt, ...)  printf("[%s:%d] ", __FILE__, __LINE__); printf(fmt, ##__VA_ARGS__)
#else
#define PRINT(fmt, ...)  LogVerbose(fmt, ##__VA_ARGS__)
#endif

#define LOG_MSG_LENGTH          1024

// File names for different logs
#define META_FILEPATH           "metadata"
#define VERBOSELOG_FILEPATH     "log"

// Available <last_run_type>
#define L_TEST      0   // This one is only used for test purpose
//~ #define G_RUN_CNT   1


// Record the latest finished run result.
typedef struct
{
    // Record the experiment category.
    int last_run_type;
    
    // Record the date-time
    char timestamp[50];
    
    // Record the value of "total run"
    long long unsigned int last_total_globalrun;
    
    // Record the value of "total run for this type"
    long long unsigned int last_total_localrun;
    
    // Record the last result (running time (us))
    long long unsigned int last_perf_result;
    
    // Record test specific perf times. (Tests need to define their meanings)
    long long unsigned int last_specific_perf[8];
    
} TPMDOS_LAST_RUN, *PTPMDOS_LAST_RUN;

// Counters to record test times. (Used for restarting app. on failure)
typedef struct
{
    long long unsigned int g_run_cnt;
    long long unsigned int l_test;//
    
} TPMDOS_META, *PTPMDOS_META;

// Increament coutners in the global <TPMDOS_META>
extern void IncGlobalMeta(int last_run_type);

// Read metafile if exist, otherwise all are 0. 
extern void ReadMeta();

// Format and output the result
extern void Log_SubmitResult(TPMDOS_LAST_RUN* result);

//plain
extern void LogVerbose(char* fmt, ...);


#endif
