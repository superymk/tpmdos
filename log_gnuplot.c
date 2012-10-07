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
        char* common_header = "#\t(Date/Time)\t(Total Run)\t(Run for this type)\t(Elapsed Time (us))";
        
        strncpy( header, common_header, LOG_MSG_LENGTH);
        
        if(special_header)
        {
            strncat( header, "\t", 1);
            strncat( header, special_header, LOG_MSG_LENGTH);
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
            break;
        
        case WRITE_40BYTES:
            GenerateFilePath(log_fn, GNUPLOT_WRITE_40BYTES_FILEPATH);
            break;
        
        case READ_40BYTES:
            GenerateFilePath(log_fn, GNUPLOT_READ_40BYTES_FILEPATH);
            break;
        
        case DEFINE_NVRAM:
            GenerateFilePath(log_fn, GNUPLOT_DEFINE_NVRAM_FILEPATH);
            break;
        
        default:
            PRINT("Invalid Update Item.\n");
            FATAL_ERROR();
            return;
    }
    
    
    GenerateHeader(log_fn, NULL);

    AppendFile (log_fn, result, strlen(result));
}