#include "log.h"
#include "file.h"
#include "timer.h"
#include "log_gnuplot.h"
#include "log_msg.h"

#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

// Should NEVER expose <g_meta>
static TPMDOS_META          g_meta;
static TPMDOS_CURRENT_RUN   g_cur_run;

// Write. Create if not exist.
static void WriteMetaFile()
{
    char meta_fn[MAX_FILEPATH_LEN];
    char str[256] = {0};
    
    // NOTE: I do not want to use binary serialization here because I think
    // it good to keep human readable.
    sprintf(str, "%llu,%llu,%llu,%llu,%llu", 
            (long long unsigned int)g_meta.g_test_cnt,
            (long long unsigned int)g_meta.L_test,
            (long long unsigned int)g_meta.Write_40bytes,
            (long long unsigned int)g_meta.Read_40bytes,
            (long long unsigned int)g_meta.Define_NVRAM
    );
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    WriteFile(meta_fn, str, strlen(str));
}

// The main entry for save results to files.
static void SubmitLog(int run_type)
{
    //For General Log
    {
        char msg[LOG_MSG_LENGTH];
        
        FormatMsg(LOG_GENERAL, msg);
        LogVerbose("%s", msg);
    }
    
    //For GnuPlot
    {
        char msg[LOG_MSG_LENGTH];
        
        FormatMsg(LOG_GNUPLOT, msg);
        GNUPLOT_SubmitResult(msg);
    }
}

static void IncGlobalMeta(int run_type)
{
    if ( (run_type <= G_RUN_CNT) || (run_type >= INVALID_BOUND) )
    {
        PRINT("Invalid Update Item.\n");
        FATAL_ERROR();
    }
    
    ((uint64_t*)&g_meta)[run_type]++;
    
    WriteMetaFile();
}

// Get the corresponding value stored in the position <run_type>
uint64_t GetMetaValue(int run_type)
{
    if ( (run_type <= G_RUN_CNT) || (run_type >= INVALID_BOUND) )
    {
        PRINT("Invalid <run_type>.\n");
        FATAL_ERROR();
    }
    
    return ((uint64_t*)&g_meta)[run_type];
}

// Reason:Some tests should not increment run counter, for example, tpm_define
// ops. within the tpm_write ops. But we should also record their perf measure.
// So the developer of perfcases should decide when to increment the run 
// counter and keep it correct.
void IncGlobalTestCounter()
{
    g_meta.g_test_cnt++;
}

uint64_t GetGlobalTestCounter(void)
{
    return g_meta.g_test_cnt;
}

int GetCurrentRunType()
{
    return g_cur_run.run_type;
}

// Used to start a run.
void StartNewRun(int run_type)
{
    
    g_cur_run.run_type = run_type;
    
    // Generate time stamp
    GetCurrentTime(g_cur_run.timestamp);
}

// Update the test run counter and Finish a run.
void EndCurrentRun(void)
{
    int cur_type = GetCurrentRunType();
    
    IncGlobalMeta(cur_type);
    SubmitLog(cur_type);
}

// Return the timestamp of the current run 
const char* GetTimeStamp(void)
{
    return g_cur_run.timestamp;
}


// Start a performance metric at perf slot <index>
void BeginPerf(int index)
{
    QueryPerformanceCounter(&g_cur_run.perf_result[index]);
}

// Finish and record the metric at perf slot <index> 
void EndPerf(int index)
{
    uint64_t result = 0;
    
    QueryPerformanceCounter(&result);
    g_cur_run.perf_result[index] = result - g_cur_run.perf_result[index];
}

// Retrieve the result at perf slot <index>
uint64_t GetPerf(int index)
{
    return g_cur_run.perf_result[index];
}

// Translate the <run_type> into <str>
void TranslateRunType(int run_type, char* str)
{
    char *run_type_str[]= 
    { NULL, "L_TEST","WRITE_40BYTES","READ_40BYTES","DEFINE_NVRAM"};
    char *result;

    if ( (run_type <= G_RUN_CNT) || (run_type >= INVALID_BOUND) )
    {
        PRINT("Invalid Update Item.\n");
        FATAL_ERROR();
    }
    
    result = run_type_str[run_type];
    memcpy(str, result, strlen(result));
}

// Read metafile if exist, otherwise all are 0. This function is used in 
// initialization
void ReadMetaFile()
{
    FILE *fp;
    char meta_fn[MAX_FILEPATH_LEN];
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    fp = fopen(meta_fn, "r");
    if (fp == NULL) {
        memset(&g_meta, 0, sizeof(g_meta));
        return;
    }
    
    fscanf(fp, "%llu,%llu,%llu,%llu,%llu", 
            (long long unsigned int*)&g_meta.g_test_cnt,
            (long long unsigned int*)&g_meta.L_test,
            (long long unsigned int*)&g_meta.Write_40bytes,
            (long long unsigned int*)&g_meta.Read_40bytes,
            (long long unsigned int*)&g_meta.Define_NVRAM
    );
    
    fclose(fp);
}

//~ static void Log_SubmitResult()
//~ {
    //~ LogVerbose("Result: run_type: %d, total_globalrun: %llu, total_localrun: %llu, perf_result: %llu\n",
            //~ g_cur_run.run_type,
            //~ GetMetaValue(G_RUN_CNT), 
            //~ GetMetaValue(GetCurrentRunType()),
            //~ g_cur_run.last_perf_result
    //~ );
//~ }

void LogVerbose(char* fmt,...)
{
	va_list args;
	char temp_str[LOG_MSG_LENGTH] = { 0 };
    char final_str[LOG_MSG_LENGTH] = {0};
    char log_fn[MAX_FILEPATH_LEN];

	va_start (args, fmt);
	vsprintf (temp_str, fmt, args);
    
    GenerateFilePath (log_fn, VERBOSELOG_FILEPATH);
    GetCurrentTime(final_str);
    
    strcat(final_str, temp_str);
    final_str[LOG_MSG_LENGTH - 1] = '\0';
    
    AppendFile (log_fn, final_str, strlen(final_str));
    va_end(args);
}

// Force Exit due to FATAL ERROR!!
void FATAL_ERROR()
{
    PRINT("FATAL ERROR OCCURS!\n");
    exit(-1);
}