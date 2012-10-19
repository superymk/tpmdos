#include "tpm_driver_comm.h"
#include "log.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096

static int configfd = 0;
static char* address = NULL;

// Create Communication with TPM Driver.
void CreateComm(void)
{
    // If exist, there should be something wrong.
    if(configfd) 
        FATAL_ERROR();
    
    configfd = open("/sys/kernel/debug/tpmdos_tis", O_RDWR);
	if(configfd < 0) {
		PRINT("Open Error");
		FATAL_ERROR();
	}
    
    address = mmap(NULL, PAGE_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, configfd, 0);
	if (address == MAP_FAILED) {
		PRINT("MMAP Error");
		FATAL_ERROR();
	}
}

// Close Communication with TPM Driver.
void CloseComm(void)
{
    close(configfd);
    configfd = 0;
}


void ReadDriverNVWriteResult(TPM_DRIVER_NVWRITE_TIMER* result)
{
    memcpy(result, address, sizeof(TPM_DRIVER_NVWRITE_TIMER));
}
