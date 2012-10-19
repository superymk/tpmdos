/* 
 * log.h -- Well, it controls how to log and what to log. And it include 
 * fail safe stuffs to recover execution in case of interruption.
 * NOTE: One test may contains multiple runs. For example, in the NVWrite test,
 * it may contains 1 define op. and 1 write op.
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
// NOTE: <last_run_type> is also used to fetch the item recorded in <TPMDOS_META>
#define G_RUN_CNT       0
#define L_TEST          1  // This one is only used for test purpose
#define WRITE_40BYTES   2
#define READ_40BYTES    3
#define DEFINE_NVRAM    4
#define INVALID_BOUND   5  // Increase this bound to include new items. 

// Record the latest finished run result.
typedef struct
{
    // Record the experiment category.
    uint32_t run_type;
    
    // Record the date-time
    char timestamp[50];
    
    // Record the perf result (running time (us))
    uint64_t perf_result[10];
    
} TPMDOS_CURRENT_RUN, *PTPMDOS_CURRENT_RUN;

// Counters to record test times. (Used for restarting app. on failure)
typedef struct
{
    uint64_t g_test_cnt;
    uint64_t L_test;// Test purpose only
    uint64_t Write_40bytes;
    uint64_t Read_40bytes;
    uint64_t Define_NVRAM;
    
} TPMDOS_META, *PTPMDOS_META;

// Increament coutners in the global <TPMDOS_META>
//~ extern void IncGlobalMeta(int last_run_type);

// Reason:Some tests should not increment run counter, for example, tpm_define
// ops. within the tpm_write ops. But we should also record their perf measure.
// So the developer of perfcases should decide when to increment the run 
// counter and keep it correct.
extern void IncGlobalTestCounter(void);
extern uint64_t GetGlobalTestCounter(void);

// Get the corresponding value stored in the position <last_run_type>
extern uint64_t GetMetaValue(int last_run_type);

// Start a run.
extern void StartNewRun(int run_type); 
// Finish a run.
extern void EndCurrentRun(void);
// Return the type of current run.
extern int GetCurrentRunType(void);
// Translate the <run_type> into <str>
extern void TranslateRunType(int run_type, char* str);
// Return the timestamp of the current run 
extern const char* GetTimeStamp(void);


// Start a performance metric at perf slot <index>
extern void BeginPerf(int index);
// Finish and record the metric at perf slot <index> 
extern void EndPerf(int index);
// Retrieve the result at perf slot <index>
extern uint64_t GetPerf(int index);
// Avoid use this API. It is only used for read timer result from TPM driver
// and report the result in user app.
extern void SetPerf(int index, uint64_t result);


// Read metafile if exist, otherwise all are 0. 
extern void ReadMetaFile(void);


//plain
extern void LogVerbose(char* fmt, ...);

// Force Exit due to FATAL ERROR!!
extern void FATAL_ERROR();

#endif
