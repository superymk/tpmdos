//Basic includes look like this:
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <tss/tspi.h>

#include "log.h"
#include "tpm.h"
#include "cpu.h"
#include "perfcases.h"

// Main entry of test cases.
static void RunPerfCases(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM);

int main(int argc, char** argv)
{
    TSS_HCONTEXT    hContext = 0;
    TSS_HTPM        hTPM = 0;
    TSS_HKEY        hSRK = 0;
    TSS_HPOLICY     hSRKPolicy = 0;
    
    UINT32          ret = 0;
    
    // Set CPU Affinity
    ret = SetAffinityCpu0();
    if (ret)
    {
        PRINT("SetAffinityCpu0 Error Return.\n");
        return -1;
    }
    
    // Init
    InitTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    ReadMetaFile();
    
    // Run Perf Cases.
    RunPerfCases(&hContext, &hTPM);

    // Finalize
    FinalizeTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    
    RestoreAffinity();
    
    return 0;
}

// Main entry of perf cases.
void RunPerfCases(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM)
{
    while(1)
    {
        //IncGlobalMeta(L_TEST);
        //extern void Log_SubmitResult(TPMDOS_LAST_RUN* result);
        //void GNUPLOT_SubmitResult(TPMDOS_LAST_RUN* result);
        
        PerfNVWrite40bytes(hContext, hTPM);
        PerfNVRead40bytes(hContext);
    }
}