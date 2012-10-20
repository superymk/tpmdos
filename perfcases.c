#include "perfcases.h"
#include "str.h"
#include "tpm.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

//PerfCases

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
 #define WRITE40_SPACE_SZ   40
void PerfNVWrite40bytes(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM)
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    static char dataToStore[WRITE40_SPACE_SZ + 1];
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    ret = IsNVIndexDefined(hTPM, nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(hContext, hTPM, WRITE40_SPACE_SZ, nv_index, nv_attribute);
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
    
    PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore);
    ret = WriteNVRAM(hContext, WRITE40_SPACE_SZ, nv_index, nv_attribute, 
        WRITE40_SPACE_SZ, (BYTE*)dataToStore);
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
    
    // Finalize
    // Generate a comprehensive string for next write.
    FLIP_BYTES(dataToStore, WRITE40_SPACE_SZ);
    dataToStore[WRITE40_SPACE_SZ] = '\0';
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
 #define READ40_SPACE_SZ    40
void PerfNVRead40bytes(TSS_HCONTEXT* hContext)
{
    const UINT32 nv_index = 0x00011101;
    UINT32 ret = 0;
    char dataToRead[READ40_SPACE_SZ];
    
    // Mark the new test
    IncGlobalTestCounter();
    // Mark run type first
    StartNewRun(READ_40BYTES);
    
    // Init
    memset(dataToRead, 0, READ40_SPACE_SZ);
    
    // Run
    ret = ReadNVRAM(hContext, READ40_SPACE_SZ, nv_index, READ40_SPACE_SZ, (BYTE*)dataToRead);
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
    
    PRINT("(%s INFO) Read Result: dest_str:%s.\n", __func__, dataToRead);
    
    EndCurrentRun();
    PRINT("(%s SUCCESSFUL) NVRead Perf Succeed! \n", __func__);
}

/*
 * NAME
 *	PerfNVWrite
 *
 * DESCRIPTION
 *	This experiment measures the NVRAM Write performance by 
 * writing 705 bytes.
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
 *		(The datasize is 705)
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures and 
 *      NVRAM write failure.
 *
 */
#define WRITE705_SPACE_SZ   705
static char dataToStore705[WRITE705_SPACE_SZ + 1];
void PerfNVWrite705bytes(TSS_HCONTEXT* hContext, TSS_HTPM* hTPM)
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    ret = IsNVIndexDefined(hTPM, nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(hContext, hTPM, WRITE705_SPACE_SZ, nv_index, nv_attribute);
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
    StartNewRun(WRITE_705BYTES);
    
    PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore705);
    ret = WriteNVRAM(hContext, WRITE705_SPACE_SZ, nv_index, nv_attribute, 
        WRITE705_SPACE_SZ, (BYTE*)dataToStore705);
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
    
    // Finalize
    // Generate a comprehensive string for next write.
    FLIP_BYTES(dataToStore705, WRITE705_SPACE_SZ);
    dataToStore705[WRITE705_SPACE_SZ] = '\0';
}

/*
 * NAME
 *	PerfNVRead
 *
 * DESCRIPTION
 *	This experiment measures the NVRAM Write performance by 
 * reading 705 bytes.
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
 *		(The datasize is 705)
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures, NVRAM
 *      read failure and string comparison failure.
 *
 */
 #define READ705_SPACE_SZ    705
void PerfNVRead705bytes(TSS_HCONTEXT* hContext)
{
    const UINT32 nv_index = 0x00011101;
    UINT32 ret = 0;
    char dataToRead[READ705_SPACE_SZ];
    
    // Mark the new test
    IncGlobalTestCounter();
    // Mark run type first
    StartNewRun(READ_705BYTES);
    
    // Init
    memset(dataToRead, 0, READ705_SPACE_SZ);
    
    // Run
    ret = ReadNVRAM(hContext, READ705_SPACE_SZ, nv_index, READ705_SPACE_SZ, (BYTE*)dataToRead);
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
    else if (!strncmp(dataToRead, dataToStore705, READ705_SPACE_SZ))
    {
        PRINT("(%s FAILED) TPM NVRAM Fails.\n", __func__ );
        FATAL_ERROR();
    }
    
    PRINT("(%s INFO) Read Result: dest_str:%s.\n", __func__, dataToRead);
    
    EndCurrentRun();
    PRINT("(%s SUCCESSFUL) NVRead Perf Succeed! \n", __func__);
}