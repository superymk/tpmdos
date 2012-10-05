#include "log.h"
#include "file.h"
#include "timer.h"
#include <string.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

TPMDOS_META g_meta;
    
// Write. Create if not exist.
static void WriteMeta()
{
    char meta_fn[MAX_FILEPATH_LEN];
    char str[256] = {0};
    
    sprintf(str, "%llu", 
            g_meta.g_run_cnt
    );
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    WriteFile(meta_fn, str, strlen(str));
}

void IncGlobalMeta(int last_run_type)
{
    g_meta.g_run_cnt++;
    
    switch(last_run_type)
    {
        case L_TEST:
            g_meta.l_test++;
            break;

        default:
            PRINT("Invalid Update Item.\n");
            g_meta.g_run_cnt--; // We should not count this test.
            return;
    }
    
    WriteMeta();
}


// Read metafile if exist, otherwise all are 0. This function is used in 
// initialization
void ReadMeta()
{
    FILE *fp;
    char meta_fn[MAX_FILEPATH_LEN];
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    fp = fopen(meta_fn, "r");
    if (fp == NULL) {
        memset(&g_meta, 0, sizeof(g_meta));
        return;
    }
    
    fscanf(fp, "%llu", 
            &g_meta.g_run_cnt
    );
    
    fclose(fp);
}

void Log_SubmitResult(TPMDOS_LAST_RUN* result)
{
    LogVerbose("Result: run_type: %d, total_globalrun: %llu, total_localrun: %llu, perf_result: %llu\n",
            result->last_run_type,
            result->last_total_globalrun,
            result->last_total_localrun,
            result->last_perf_result
    );
}

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