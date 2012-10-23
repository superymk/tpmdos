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
void PerfNVWrite40bytes()
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    static char dataToStore[WRITE40_SPACE_SZ + 1];
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    ret = IsNVIndexDefined(nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(WRITE40_SPACE_SZ, nv_index, nv_attribute);
        if(ret)
        {
            PRINT("(%s FAILED) DefineNVRAM.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    else if (ret == TPMUTIL_GETCAP_ERROR)
    {
        PRINT("(%s FAILED) TPMUTIL_GETCAP_ERROR.\n", __func__ );
        FATAL_ERROR();
    }
    
    // Run
    // Mark run type first
    StartNewRun(WRITE_40BYTES);
    
    PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore);
    ret = WriteNVRAM(WRITE40_SPACE_SZ, nv_index, nv_attribute, 
        WRITE40_SPACE_SZ, (BYTE*)dataToStore);
    if ( ret == TPM_NVWRITE_ERROR)
    {
        PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
        FATAL_ERROR();
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        FATAL_ERROR();
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
void PerfNVRead40bytes()
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
    ret = ReadNVRAM(READ40_SPACE_SZ, nv_index, READ40_SPACE_SZ, FALSE, (BYTE*)dataToRead);
    if (ret == TPM_NVREAD_ERROR)
    {
        PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
        FATAL_ERROR();
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        FATAL_ERROR();
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
void PerfNVWrite705bytes()
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    ret = IsNVIndexDefined(nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(WRITE705_SPACE_SZ, nv_index, nv_attribute);
        if(ret)
        {
            PRINT("(%s FAILED) DefineNVRAM.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    else if (ret == TPMUTIL_GETCAP_ERROR)
    {
        PRINT("(%s FAILED) TPMUTIL_GETCAP_ERROR.\n", __func__ );
        FATAL_ERROR();
    }
    
    // Run
    // Generate a comprehensive string for next write.
    FLIP_BYTES((dataToStore705), (WRITE705_SPACE_SZ));
    dataToStore705[WRITE705_SPACE_SZ] = '\0';
    
    // Mark run type first
    StartNewRun(WRITE_705BYTES);
    
    PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore705);
    ret = WriteNVRAM(WRITE705_SPACE_SZ, nv_index, nv_attribute, 
        WRITE705_SPACE_SZ, (BYTE*)dataToStore705);
    if ( ret == TPM_NVWRITE_ERROR)
    {
        PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
        FATAL_ERROR();
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        FATAL_ERROR();
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
void PerfNVRead705bytes()
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
    ret = ReadNVRAM(READ705_SPACE_SZ, nv_index, READ705_SPACE_SZ, FALSE, (BYTE*)dataToRead);
    if (ret == TPM_NVREAD_ERROR)
    {
        PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
        FATAL_ERROR();
    }
    else if ( ret == TPM_ATTIBUTE_ERROR)
    {
        PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
        FATAL_ERROR();
    }
    else if (strncmp((dataToRead), dataToStore705, READ705_SPACE_SZ))
    {
        PRINT("(%s FAILED) TPM NVRAM Fails.\n", __func__ );
        FATAL_ERROR();
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
 * writing (ALL - 118) bytes for IFX1212.
 *
 * ALGORITHM
 *	Setup:
 *		Tspi_Context_Create
 *		Tspi_Context_Connect
 *		Tspi_Context_CreateObject(NV object)
 *		Setsecret to TPM policy with the correct owner passwd
 *		Tspi_SetAttribUint32(Index, permission, datasize)
 *		(The Index is 0x00011101, 0x30000001, 0x1000f000)
 *		(The Permission is OWNERWRITE)
 *		(The datasize is (ALL - 118))
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures and 
 *      NVRAM write failure.
 *
 */
#define WRITE1704_SPACE_SZ      1704
static char dataToStore1704[WRITE1704_SPACE_SZ + 1];
#define WRITE576_SPACE_SZ       576
static char dataToStore576[WRITE576_SPACE_SZ + 1];
void PerfNVWriteAllIFX1212()
{
    // Setup
    const UINT32 nv_index = 0x00011101;
    const UINT32 nv_attribute = TPM_NV_PER_OWNERWRITE;
    
    UINT32 ret = 0;
    
    // Mark the new test
    IncGlobalTestCounter();
    
    ret = IsNVIndexDefined(nv_index);
    if(!ret)
    {
        // Mark run type first - We'd like to get the perf of DEFINE_NVRAM at 
        // the same time
        StartNewRun(DEFINE_NVRAM);
        ret = DefineNVRAM(WRITE705_SPACE_SZ, nv_index, nv_attribute);
        if(ret)
        {
            PRINT("(%s FAILED) DefineNVRAM.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    else if (ret == TPMUTIL_GETCAP_ERROR)
    {
        PRINT("(%s FAILED) TPMUTIL_GETCAP_ERROR.\n", __func__ );
        FATAL_ERROR();
    }
    
    // Run
    // Generate a comprehensive string for next write.
    FLIP_BYTES(dataToStore705, (WRITE705_SPACE_SZ));
    dataToStore705[WRITE705_SPACE_SZ] = '\0';
    FLIP_BYTES(dataToStore1704, WRITE1704_SPACE_SZ);
    dataToStore1704[WRITE1704_SPACE_SZ] = '\0';
    FLIP_BYTES(dataToStore576, WRITE576_SPACE_SZ);
    dataToStore576[WRITE576_SPACE_SZ] = '\0';
    
    {
        // Mark run type first
        StartNewRun(WRITE_705BYTES);
        
        PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore705+4);
        ret = WriteNVRAM(WRITE705_SPACE_SZ, 0x00011101, nv_attribute, 
            WRITE705_SPACE_SZ, (BYTE*)dataToStore705);
        if ( ret == TPM_NVWRITE_ERROR)
        {
            PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR)
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    
    {
        // Mark run type first
        StartNewRun(WRITE_1704BYTES);
        
        //PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore1704);
        ret = WriteNVRAM(WRITE705_SPACE_SZ, 0x1000f000, nv_attribute, 
            WRITE1704_SPACE_SZ, (BYTE*)dataToStore1704);
        if ( ret == TPM_NVWRITE_ERROR)
        {
            PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR)
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    
    {
        // Mark run type first
        StartNewRun(WRITE_576BYTES);
        
        //PRINT("(%s INFO) NVWrite will write content:%s \n", __func__, dataToStore576);
        ret = WriteNVRAM(WRITE576_SPACE_SZ, 0x30000001, nv_attribute, 
            WRITE576_SPACE_SZ, (BYTE*)dataToStore576);
        if ( ret == TPM_NVWRITE_ERROR)
        {
            PRINT("(%s FAILED) NVWrite Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR)
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        
        EndCurrentRun();
    }
    
    
    PRINT("(%s SUCCESSFUL) NVWriteAllIFX1212 Perf Succeed! \n", __func__);
}

/*
 * NAME
 *	PerfNVRead
 *
 * DESCRIPTION
 *	This experiment measures the NVRAM Write performance by 
 * reading (ALL - 118) bytes.
 *
 * ALGORITHM
 *	Setup:
 *		Tspi_Context_Create
 *		Tspi_Context_Connect
 *		Tspi_Context_CreateObject(NV object)
 *		Setsecret to TPM policy with the correct owner passwd
 *		Tspi_SetAttribUint32(Index, permission, datasize)
 *		(The Index is 0x00011101, 0x30000001, 0x1000f000)
 *		(The Permission is OWNERWRITE)
 *		(The datasize is (ALL - 118))
 *		Tspi_NV_ReleaseSpace
 *
 * Run:	
 *		Evaluation fails on Attibute setting failures, NVRAM
 *      read failure and string comparison failure.
 *
 */
#define READ1704_SPACE_SZ       1704
#define READ576_SPACE_SZ        576
void PerfNVReadAllIFX1212()
{
    UINT32 ret = 0;
    char dataToRead[READ705_SPACE_SZ];
    char dataToRead1704[READ1704_SPACE_SZ];
    char dataToRead576[READ576_SPACE_SZ];
    
    // Mark the new test
    IncGlobalTestCounter();
    
    {
        // Mark run type first
        StartNewRun(READ_705BYTES);
        
        // Init
        memset(dataToRead, 0, READ705_SPACE_SZ);
        
        // Run
        ret = ReadNVRAM(READ705_SPACE_SZ, 0x00011101, READ705_SPACE_SZ, FALSE, (BYTE*)dataToRead);
        if (ret == TPM_NVREAD_ERROR)
        {
            PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR )
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if (strncmp((dataToRead), (dataToStore705), (READ705_SPACE_SZ)))
        {
            PRINT("(%s FAILED) TPM NVRAM Fails.\n", __func__ );
            FATAL_ERROR();
        }
        
        //PRINT("(%s INFO) Read Result: dest_str:%s.\n", __func__, dataToRead);
        
        EndCurrentRun();
    }
    
    {
        // Mark run type first
        StartNewRun(READ_1704BYTES);
        
        // Init
        memset(dataToRead1704, 0, READ705_SPACE_SZ);
        
        // Run
        ret = ReadNVRAM(READ1704_SPACE_SZ, 0x1000f000, READ1704_SPACE_SZ, TRUE, (BYTE*)dataToRead1704);
        if (ret == TPM_NVREAD_ERROR)
        {
            PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR )
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if (strncmp(dataToRead1704, dataToStore1704, READ1704_SPACE_SZ))
        {
            PRINT("(%s FAILED) TPM NVRAM Fails.\n", __func__ );
            FATAL_ERROR();
        }
        
        //PRINT("(%s INFO) Read Result: dest_str:%s.\n", __func__, dataToRead);
        
        EndCurrentRun();
    }
    
    {
        // Mark run type first
        StartNewRun(READ_576BYTES);
        
        // Init
        memset(dataToRead576, 0, READ576_SPACE_SZ);
        
        // Run
        ret = ReadNVRAM(READ576_SPACE_SZ, 0x30000001, READ576_SPACE_SZ, FALSE, (BYTE*)dataToRead576);
        if (ret == TPM_NVREAD_ERROR)
        {
            PRINT("(%s FAILED) NVRead Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if ( ret == TPM_ATTIBUTE_ERROR )
        {
            PRINT("(%s FAILED) TPM Attribute Fail.\n", __func__ );
            FATAL_ERROR();
        }
        else if (strncmp(dataToRead576, dataToRead576, READ576_SPACE_SZ))
        {
            PRINT("(%s FAILED) TPM NVRAM Fails.\n", __func__ );
            FATAL_ERROR();
        }
        
        //PRINT("(%s INFO) Read Result: dest_str:%s.\n", __func__, dataToRead);
        
        EndCurrentRun();
    }
    
    PRINT("(%s SUCCESSFUL) NVReadAllIFX1212 Perf Succeed! \n", __func__);
}
