#ifndef _LOG_GNUPLOT_H
#define _LOG_GNUPLOT_H

#include "log.h"

/* 
The log format is: 

FileName: (TypeName)
#   (Date/Time)         (Total Run)    (Run for this type)      (Elapsed Time (us))
    yyyymmdd-HH:MM:SS:MMM    %llu          %llu                      %llu

*/

#define GNUPLOT_TEST_FILEPATH     "gnuplot-test"

// Format and output the result
extern void GNUPLOT_SubmitResult(TPMDOS_LAST_RUN* result);

#endif

