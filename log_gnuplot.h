#ifndef _LOG_GNUPLOT_H
#define _LOG_GNUPLOT_H

#include "log.h"

/* 
The log format is: 

FileName: (TypeName)
#   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))     (Specific Time 1(us))       (Specific Time 2(us)) .....
    yyyymmdd-HH:MM:SS:MMM    %llu          %llu                      %llu                    %llu                        %llu

*/

#define GNUPLOT_TEST_FILEPATH               "gnuplot-test"
#define GNUPLOT_WRITE_40BYTES_FILEPATH      "gnuplot-write40"
#define GNUPLOT_READ_40BYTES_FILEPATH       "gnuplot-read40"
#define GNUPLOT_WRITE_705BYTES_FILEPATH      "gnuplot-write705"
#define GNUPLOT_READ_705BYTES_FILEPATH       "gnuplot-read705"
#define GNUPLOT_DEFINE_NVRAM_FILEPATH       "gnuplot-defineNV"

// Format and output the result
extern void GNUPLOT_SubmitResult(char* result);

#endif

