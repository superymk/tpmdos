#include "tpm.h"

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
    result = Tspi_Policy_SetSecret(hNewPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
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
