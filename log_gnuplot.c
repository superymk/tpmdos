#include "log_gnuplot.h"
#include "file.h"
#include <string.h>

// This is only used for test purpose
static void GNUPLOT_Submit_Test(TPMDOS_LAST_RUN* result)
{
    char log_fn[MAX_FILEPATH_LEN];
    
    GenerateFilePath(log_fn, GNUPLOT_TEST_FILEPATH);
    if ( IsFileExist(log_fn) == -1 || GetFileSize(log_fn) == 0)
    {
        // The log file is or will be newly generated.
        char* header = "#\t(Date/Time)\t(Total Run)\t(Run for this type)\t(Elapsed Time (us))";
        
        AppendFile (log_fn, header, strlen(header));
    }
    else
    {
        char str[LOG_MSG_LENGTH] = {0};
        
        sprintf (str, "\t%s\t%llu\t%llu\t%llu", result->timestamp, 
                result->last_total_globalrun, result->last_total_localrun, 
                result->last_perf_result);
        
        AppendFile (log_fn, str, strlen(str));
    }
}

void GNUPLOT_SubmitResult(TPMDOS_LAST_RUN* result)
{
    switch(result->last_run_type)
    {
        //todo: Add new log output methods here for new test cases.
        case L_TEST:
            GNUPLOT_Submit_Test(result);
            break;
        default:
            PRINT("Invalid Update Item.\n");
            return;
    }
}