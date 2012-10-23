//Basic includes look like this:
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <tss/tspi.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>

#include "log.h"
#include "tpm.h"
#include "cpu.h"
#include "perfcases.h"
#include "tpm_driver_comm.h"

static TSS_HCONTEXT    hContext = 0;
static TSS_HTPM        hTPM = 0;
static TSS_HKEY        hSRK = 0;
static TSS_HPOLICY     hSRKPolicy = 0;
    
// Main entry of test cases.
static void RunPerfCases(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM);

void FinalizeTPMDOS(void)
{
    CloseComm();
    FinalizeTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    
    RestoreAffinity();
}

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        PRINT("Received SIGINT\n");
        FinalizeTPMDOS();
    }
}

int main(int argc, char** argv)
{
    UINT32          ret = 0;
    
    // Set CPU Affinity
    ret = SetAffinityCpu0();
    if (ret)
    {
        PRINT("SetAffinityCpu0 Error Return.\n");
        return -1;
    }
    
    setpriority(PRIO_PROCESS, 0, -20);
    
    // Init
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        PRINT("Can't catch SIGINT\n");
    
    InitTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    CreateComm();
    ReadMetaFile();
    
    // Run Perf Cases.
    RunPerfCases(&hContext, &hTPM);

    // Finalize
    FinalizeTPMDOS();
    
    return 0;
}

// Main entry of perf cases.
void RunPerfCases(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM)
{
    while(1)
    {
        //PerfNVWrite40bytes();
        //PerfNVRead40bytes();
        
        //PerfNVWrite705bytes();
        //PerfNVRead705bytes();
        PerfNVWriteAllIFX1212();
        PerfNVReadAllIFX1212();
    }
}