#include "log_gnuplot.h"
#include "log.h"
#include "file.h"
#include <string.h>

// Generate the header line conform to the GnuPlot format. Then write to file.
static void GenerateHeader(char* log_fn, char* special_header)
{
    if ( IsFileExist(log_fn) == -1 || GetFileSize(log_fn) == 0)
    {
        // The log file is or will be newly generated.
        char header[LOG_MSG_LENGTH] = {0};
        char* common_header = "#\t(Date/Time)\t(Total Run)\t(Run for this type)";
        int common_header_len = strlen(common_header);
        
        strncpy( header, common_header, common_header_len);
        
        if(special_header)
        {
            strncat( header, "\t", 1);
            strncat( header, special_header, (LOG_MSG_LENGTH - 1 - common_header_len));
        }
        
        strncat( header, "\n", 1);
        AppendFile (log_fn, header, strlen(header));
    }
}

//void GNUPLOT_SubmitResult(TPMDOS_LAST_RUN* result)
void GNUPLOT_SubmitResult(char* result)
{
    char log_fn[MAX_FILEPATH_LEN];
    int cur_run_type = GetCurrentRunType();
    
    // Decide the output file name.
    switch(cur_run_type)
    {
        //todo: Add new log output methods here for new test cases.
        case L_TEST:
            GenerateFilePath(log_fn, GNUPLOT_TEST_FILEPATH);
            GenerateHeader(log_fn, NULL);
            break;
        
        case WRITE_40BYTES:
            GenerateFilePath(log_fn, GNUPLOT_WRITE_40BYTES_FILEPATH);
            GenerateHeader(
                log_fn, 
                "(WRITE_NVWRITE_PERF (us))\t(WRITE_ATTRIB_PERF (us))\t(WRITE_POLICY_PERF (us))"
            );
            break;
        
        case READ_40BYTES:
            GenerateFilePath(log_fn, GNUPLOT_READ_40BYTES_FILEPATH);
            GenerateHeader(
                log_fn, 
                "(READ_NVREAD_PERF (us))\t(READ_ATTRIB_PERF (us))"
            );
            break;
        
        case DEFINE_NVRAM:
            GenerateFilePath(log_fn, GNUPLOT_DEFINE_NVRAM_FILEPATH);
            GenerateHeader(
                log_fn, 
                "(DEFINE_NVDEFINE_PERF (us))\t(DEFINE_ATTRIB_PERF (us))\t(DEFINE_POLICY_PERF (us))"
            );
            break;
        
        default:
            PRINT("Invalid Update Item.\n");
            FATAL_ERROR();
            return;
    }
    
    AppendFile (log_fn, result, strlen(result));
}