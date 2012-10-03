#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <stdlib.h>

#include "log.h"
#include "cpu.h"

#define CPU0_MASK   0

static cpu_set_t default_set;
static pid_t pid = 0;

int SetAffinityCpu0()
{
    static cpu_set_t new_set;
    int ret = 0, i;
    
    CPU_ZERO(&default_set);
    ret = sched_getaffinity(0, sizeof(default_set), &default_set);
    if (ret == -1) {
		PRINT("could not get pid %d's affinity.\n", pid);
		return -1;
	}
    
    CPU_ZERO(&new_set);
    // Allow to use CPU0
    CPU_SET(0, &new_set);
    
    // Forbid to use other CPUs.
    for (i = 1; i< CPU_SETSIZE; i++)
    {
        CPU_CLR(i, &new_set);
    }
    
    ret = sched_setaffinity(0, sizeof(new_set), &new_set);
    if (ret == -1)
    {
		PRINT("could not set pid %d's affinity.\n", pid);
		return -1;
	}
    
    return 0;
}

int RestoreAffinity()
{
    int ret = 0;
    
    ret = sched_setaffinity(0, sizeof(default_set), &default_set);
    if (ret == -1)
    {
		PRINT("could not set pid %d's affinity.\n", pid);
		return -1;
	}

	return 0;
}