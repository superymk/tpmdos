#include "timer.h"

#include <sys/time.h>
#include <stddef.h>
#include <assert.h>

#define usec_per_sec 1000000;

void QueryPerformanceFrequency(uint64_t *frequency)
{
    /* Sanity check. */
    assert(frequency != NULL);

    /* gettimeofday reports to microsecond accuracy. */
    *frequency = usec_per_sec;
}

void QueryPerformanceCounter(uint64_t *performance_count)
{
    struct timeval time;

    /* Sanity check. */
    assert(performance_count != NULL);

    /* Grab the current time. */
    gettimeofday(&time, NULL);
    *performance_count = time.tv_usec + /* Microseconds. */
                         time.tv_sec * usec_per_sec; /* Seconds. */
}