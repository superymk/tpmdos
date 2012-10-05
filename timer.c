#include "timer.h"

#include <sys/time.h>
#include <time.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

#define USEC_PER_SEC    1000000
#define MS_PER_SEC      1000

void QueryPerformanceFrequency(uint64_t *frequency)
{
    /* Sanity check. */
    assert(frequency != NULL);

    /* gettimeofday reports to microsecond accuracy. */
    *frequency = USEC_PER_SEC;
}

void QueryPerformanceCounter(uint64_t *performance_count)
{
    struct timeval time;

    /* Sanity check. */
    assert(performance_count != NULL);

    /* Grab the current time. */
    gettimeofday(&time, NULL);
    *performance_count = time.tv_usec + /* Microseconds. */
                         time.tv_sec * USEC_PER_SEC; /* Seconds. */
}

void GetCurrentTime(char* str)
{
    time_t t = time(NULL);
    struct timeval tv;
    struct timezone tz;
    struct tm *tmS;
    struct tm *tmD = localtime(&t);
    
    gettimeofday(&tv, &tz);
    tmS=localtime(&tv.tv_sec);

    sprintf(str, "[%04d%02d%02d-%02d:%02d:%02d:%03u] ", tmD->tm_year + 1900, tmD->tm_mon + 1, tmD->tm_mday, tmS->tm_hour, tmS->tm_min, tmS->tm_sec, ((unsigned int)tv.tv_usec / MS_PER_SEC));
}