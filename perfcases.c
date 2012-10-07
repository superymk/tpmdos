#include "perfcases.h"
#include "tpm.h"
#include "log.h"

//PerfCases
#define TEST_STR    "This is some test."
#define TEST_STR_LENGTH  (strlen(TEST_STR))

/*
 * NAME
 *	PerfNVWrite
 *
 * DESCRIPTION
 *	This experiment measures the NVRAM Write performance by 
 * writing a small amount of bytes.
 *
 * ALGORITHM
 *	Setup:
 *		Tspi_Context_Create
 *		Tspi_Context_Connect
 *		Tspi_Context_CreateObject(NV object)
 *		Setsecret to TPM policy with the correct owner passwd
 *		Tspi_SetAttribUint32(Index, permission, datasize)
 *		(The Index is 0x00011101)
 *		(The Permission is OWNERWRITE)
 *		(The datasize is 40)
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures and 
 *      NVRAM write failure.
 *
 */
void PerfNVWrite40bytes(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM)
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 space_sz = 40;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    
    char dataToStore[4096] = TEST_STR;
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    // Init
    ret = IsNVIndexDefined(hTPM, nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(hContext, hTPM, space_sz, nv_index, nv_attribute);
        if(ret)
        {
            PRINT("(%s FAILED) DefineNVRAM.\n", __func__ );
            return;
        }
        
        EndCurrentRun();
    }
    else if (ret == TPMUTIL_GETCAP_ERROR)
    {
        PRINT("(%s FAILED) TPMUTIL_GETCAP_ERROR.\n", __func__ );
        return;
    }
    
    // Run
    // Mark run type first
    StartNewRun(WRITE_40BYTES);
    
    ret = WriteNVRAM(hContext, space_sz, nv_index, nv_attribute, 
        TEST_STR_LENGTH, (BYTE*)dataToStore);
    if ( ret == TPM_NVWRITE_ERROR)
    {
        PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
        return;
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        return;
    }
    
    EndCurrentRun();
    PRINT("(%s SUCCESSFUL) NVWrite Perf Succeed! \n", __func__);
}

/*
 * NAME
 *	PerfNVRead
 *
 * DESCRIPTION
 *	This experiment measures the NVRAM Write performance by 
 * reading a small amount of bytes.
 *
 * ALGORITHM
 *	Setup:
 *		Tspi_Context_Create
 *		Tspi_Context_Connect
 *		Tspi_Context_CreateObject(NV object)
 *		Setsecret to TPM policy with the correct owner passwd
 *		Tspi_SetAttribUint32(Index, permission, datasize)
 *		(The Index is 0x00011101)
 *		(The Permission is OWNERWRITE)
 *		(The datasize is 40)
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures, NVRAM
 *      read failure and string comparison failure.
 *
 */
void PerfNVRead40bytes(TSS_HCONTEXT* hContext)
{
    char dataToRead[4096]={0};
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    // Mark run type first
    StartNewRun(READ_40BYTES);
    
    // Init
    memset(dataToRead, 0, TEST_STR_LENGTH);
    
    // Run
    ret = ReadNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToRead);
    if (ret == TPM_NVREAD_ERROR)
    {
        PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
        
        return;
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        return;
    }
    
    if(strcmp(dataToRead, TEST_STR))
    {
        PRINT("(%s FAILED) Read Result Error: src_str:%s, dest_str:%s.\n", 
                __func__, TEST_STR, dataToRead);
        
        return;
    }
    
    EndCurrentRun();
    PRINT("(%s SUCCESSFUL) NVRead Perf Succeed! \n", __func__);
}