#ifndef _TPM_DRIVER_COMM
#define _TPM_DRIVER_COMM
#include <inttypes.h>

typedef struct
{
	uint64_t timer_result;
	unsigned long max_jiffies;
} TPM_DRIVER_NVWRITE_TIMER, *PTPM_DRIVER_NVWRITE_TIMER;

// Create Communication with TPM Driver.
extern void CreateComm(void);
// Close Communication with TPM Driver.
extern void CloseComm(void);

extern void ReadDriverNVWriteResult(TPM_DRIVER_NVWRITE_TIMER* result);

#endif
