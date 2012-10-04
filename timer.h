#ifndef _TIMER_H
#define _TIMER_H

#include <stdint.h>

/* These functions are written to match the win32
   signatures and behavior as closely as possible.
*/
extern void QueryPerformanceFrequency(uint64_t *frequency);
extern void QueryPerformanceCounter(uint64_t *performance_count);

#endif
