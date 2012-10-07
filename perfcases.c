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
    char dataToStore[4096] = TEST_STR;
    UINT32 ret = 0;
    
    ret = WriteNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToStore);
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
    
    memset(dataToRead, 0, TEST_STR_LENGTH);
    
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
    
    PRINT("(%s SUCCESSFUL) NVRead Perf Succeed! \n", __func__);
}