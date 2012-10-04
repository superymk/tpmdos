//Basic includes look like this:
#include <stdio.h>
#include <string.h>
#include <sched.h>
#include <tss/tspi.h>

#include "log.h"
#include "tpm.h"
#include "cpu.h"


// Main entry of test cases.
static int RunTestCases(void);

// List all the test cases.
static int TestNVWrite(TSS_HCONTEXT hContext);
static int TestNVRead(TSS_HCONTEXT hContext);

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
        printf("SetAffinityCpu0 Error Return.\n");
        return -1;
    }
    
    // Init
    InitTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    ReadMeta();
    
    // Run Test Cases.
    RunTestCases();

    // Finalize
    FinalizeTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    
    RestoreAffinity();
    
    return 0;
}


//TestCases
#define TEST_STR    "This is some test."
#define TEST_STR_LENGTH  (strlen(TEST_STR))
int TestNVWrite(TSS_HCONTEXT hContext)
{
    char dataToStore[4096]=TEST_STR;
    return WriteNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToStore);
}

int TestNVRead(TSS_HCONTEXT hContext)
{
    char dataToRead[4096]={0};
    UINT32 ret = 0;
    
    memset(dataToRead, 0, TEST_STR_LENGTH);
    
    ret = ReadNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToRead);
    if(ret == -1)
    {
        printf("(Line %d, %s) ReadNVRAM() Error.\n", __LINE__ , __func__ );
        return -1;
    }
    
    if(strcmp(dataToRead, TEST_STR))
    {
        printf("(Line %d, %s) Read Result Error; dataToRead: %s.\n", __LINE__ , __func__, dataToRead);
        return -1;
    }
    
    return 0;
    
}

// Main entry of test cases.
int RunTestCases()
{
    UINT32 ret = 0;
    
    // Test 1.
    while(1)
    {
        PRINT("aaa\n");
        IncGlobalMeta(L_TEST);
        
        // Other Tests.
        //~ ret = TestNVWrite(hContext);
        //~ if (ret == -1)
        //~ {
            //~ printf("(Line %d, %s) NVWrite Test Error Return.\n", __LINE__ , __func__ );
            //~ return -1;
        //~ }
        //~ else
        //~ { 
            //~ printf("(Line %d, %s) NVWrite Test Succeed.\n", __LINE__ , __func__ );
        //~ }
        
        //~ ret = TestNVRead(hContext);
        //~ if (ret)
        //~ {
            //~ printf("(Line %d, %s) NVRead Test Error Return.\n", __LINE__ , __func__ );
            //~ return -1;
        //~ }
        //~ else
        //~ { 
            //~ printf("(Line %d, %s) NVRead Test Succeed.\n", __LINE__ , __func__ );
        //~ }
    }
    
    
}