//Basic includes look like this:
#include <stdio.h>
#include <string.h>
#include <tss/tss_error.h>
#include <tss/platform.h>
#include <tss/tss_defines.h>
#include <tss/tss_typedef.h>
#include <tss/tss_structs.h>
#include <tss/tspi.h>
#include <trousers/trousers.h>

#define DEBUG 1
#define DBG(message,tResult) if(DEBUG) {printf("(Line %d, %s) %s returned 0x%08x. %s.\n", __LINE__ , __func__ , message , tResult , Trspi_Error_String(tResult));}

#define SRK_PASSWD  "a"
#define SRK_PASSWD_LENGTH  (strlen(SRK_PASSWD))

static int TestNVWrite(TSS_HCONTEXT hContext);
static int TestNVRead(TSS_HCONTEXT hContext);

int WriteNVRAM(
    TSS_HCONTEXT hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
) 
{
    TSS_RESULT      result;
    TSS_HNVSTORE    hNVStore;
    TSS_HPOLICY     hNewPolicy;
    //char dataToStore[19]="This is some dat1.";
    
    /* Create a NVRAM object */
    result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_Context_CreateObject: %x\n",result); 
        return 1; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    result = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 index %x\n",result); 
        return 1; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    result = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, TPM_NV_PER_OWNERWRITE);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 auth %x\n",result); 
        return 1; 
    }
    
    /* next it holds 40 bytes of data */
    result = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE,0, space_size);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 size%x\n",result); 
        return 1; 
    }
        
    /* Set Policy for the NVRAM object using the Owner Auth */
    result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE, &hNewPolicy);
    result = Tspi_Policy_SetSecret(hNewPolicy, TSS_SECRET_MODE_PLAIN, SRK_PASSWD_LENGTH, (BYTE*)SRK_PASSWD);
    result = Tspi_Policy_AssignToObject(hNewPolicy,hNVStore);

    /* Write to the NVRAM space */
    result = Tspi_NV_WriteValue(hNVStore, 0, ulDataLength, data);
    if (result!=TSS_SUCCESS) 
    { 
        DBG(" Tspi_NV_WriteValue: %x\n",result); 
        return 1;
    }
    
    return 0;
}

int ReadNVRAM(
    TSS_HCONTEXT hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
)
{
    TSS_HNVSTORE    hNVStore;
    TSS_RESULT      result;
    BYTE*           rdata = 0;
    
    /* Create a NVRAM object */
    result = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_Context_CreateObject: %x\n",result); 
        return 1; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    result = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 index %x\n",result); 
        return 1; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    result = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, TPM_NV_PER_OWNERWRITE);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 auth %x\n",result); 
        return 1; 
    }
    
    /* next it holds 40 bytes of data */
    result = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE, 0, space_size);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 size%x\n",result); 
        return 1; 
    }
    
    /* No authorization needed to read from this NVRAM the way it was created. */
    /* Read from the NVRAM space */
    result = Tspi_NV_ReadValue(hNVStore,0, &ulDataLength, &rdata);
    if (result!=TSS_SUCCESS) 
    { 
        DBG("Tspi_NV_ReadValue: %x\n",result); 
        return 1; 
    }
    
    memcpy(data, rdata, ulDataLength);
    return 0;
}

void InitTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
)
{
    TSS_UUID          SRK_UUID = TSS_UUID_SRK;
    TSS_RESULT        result;
    BYTE              wks[20]; // Place to put the well known secret

    memset(wks,0,20);// Set wks to the well known secret of 20 bytes of all zeros
    
    // Pick the TPM you are talking to in this case the system TPM (which you connect to with ¡°NULL¡±)
    result =Tspi_Context_Create(hContext); 
    DBG(" Create a Context\n",result); 
    result=Tspi_Context_Connect(*hContext, NULL); 
    DBG("Connect to TPM\n", result);
        
    // Get the TPM handle
    result=Tspi_Context_GetTpmObject(*hContext, hTPM); 
    DBG("GetTPMHandle\n",result); 

    //Get the SRK handle
    result=Tspi_Context_LoadKeyByUUID(*hContext, TSS_PS_TYPE_SYSTEM, SRK_UUID, hSRK); 
    DBG("Tspi_Context_Connect\n",result);

    //Get the SRK policy
    result=Tspi_GetPolicyObject(*hSRK, TSS_POLICY_USAGE, hSRKPolicy); 
    DBG("Get TPM Policy\n" ,result);
    
    // Then we set the SRK policy to be the well known secret
    result=Tspi_Policy_SetSecret(*hSRKPolicy,TSS_SECRET_MODE_SHA1,20, wks); 
    // Note: TSS_SECRET_MODE_SHA1 says ¡°Don¡¯t hash this. Just use the 20 bytes as is.
    DBG("Tspi_Policy_Set_Secret\n",result);
}

void FinalizeTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
)
{
    /* Clean up */
    Tspi_Context_Close (*hTPM);
    Tspi_Context_Close (*hSRK);
    Tspi_Context_Close (*hSRKPolicy);
    // this frees memory that was automatically allocated for you
    Tspi_Context_FreeMemory(*hContext, NULL);
    
    Tspi_Context_Close(*hContext);
}
int main()
{
    TSS_HCONTEXT    hContext = 0;
    TSS_HTPM        hTPM = 0;
    TSS_HKEY        hSRK = 0;
    TSS_HPOLICY     hSRKPolicy = 0;
    
    UINT32          result = 0;

    // Init
    InitTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    
    // Run Test Cases.
    result = TestNVWrite(hContext);
    if(result)
    {
        printf("(Line %d, %s) NVWrite Test Error Return.\n", __LINE__ , __func__ );
        return 1;
    }
    else
    { 
        printf("(Line %d, %s) NVWrite Test Succeed.\n", __LINE__ , __func__ );
    }
    
    result = TestNVRead(hContext);
    if(result)
    {
        printf("(Line %d, %s) NVRead Test Error Return.\n", __LINE__ , __func__ );
        return 1;
    }
    else
    { 
        printf("(Line %d, %s) NVRead Test Succeed.\n", __LINE__ , __func__ );
    }

    // Finalize
    FinalizeTPM(&hContext, &hTPM, &hSRK, &hSRKPolicy);
    
    
    return 0;
}

//TestCases
#define TEST_STR    "This is some daa5."
#define TEST_STR_LENGTH  (strlen(TEST_STR))
int TestNVWrite(TSS_HCONTEXT hContext)
{
    char dataToStore[4096]=TEST_STR;
    return WriteNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToStore);
}

int TestNVRead(TSS_HCONTEXT hContext)
{
    char dataToRead[4096]={0};
    UINT32 status = 0;
    
    //memset(dataToRead, 0, TEST_STR_LENGTH);
    
    status = ReadNVRAM(hContext, 40, 0x00011101, TEST_STR_LENGTH, (BYTE*)dataToRead);
    printf("(Line %d, %s) dataToRead: %s\n", __LINE__ , __func__, dataToRead);
    if(status)
    {
        printf("(Line %d, %s) ReadNVRAM() Error.\n", __LINE__ , __func__ );
        return 1;
    }
    
    if(strcmp(dataToRead, TEST_STR))
    {
        printf("(Line %d, %s) Read Result Error; dataToRead: %s.\n", __LINE__ , __func__, dataToRead);
        return 1;
    }
    
    return 0;
    
}