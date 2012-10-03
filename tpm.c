#include "tpm.h"

int WriteNVRAM(
    TSS_HCONTEXT hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
) 
{
    TSS_RESULT      ret;
    TSS_HNVSTORE    hNVStore;
    TSS_HPOLICY     hNewPolicy;
    //char dataToStore[19]="This is some dat1.";
    
    /* Create a NVRAM object */
    ret = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_Context_CreateObject: %x\n",ret); 
        return -1; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 index %x\n",ret); 
        return -1; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    ret = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, TPM_NV_PER_OWNERWRITE);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 auth %x\n",ret); 
        return -1; 
    }
    
    /* next it holds 40 bytes of data */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE,0, space_size);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 size%x\n",ret); 
        return -1; 
    }
        
    /* Set Policy for the NVRAM object using the Owner Auth */
    ret = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE, &hNewPolicy);
    ret = Tspi_Policy_SetSecret(hNewPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
    ret = Tspi_Policy_AssignToObject(hNewPolicy,hNVStore);

    /* Write to the NVRAM space */
    ret = Tspi_NV_WriteValue(hNVStore, 0, ulDataLength, data);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_NV_WriteValue: %x\n",ret); 
        return -1;
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
    TSS_RESULT      ret;
    BYTE*           rdata = 0;
    
    /* Create a NVRAM object */
    ret = Tspi_Context_CreateObject(hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_Context_CreateObject: %x\n",ret); 
        return -1; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 index %x\n",ret); 
        return -1; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    ret = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, TPM_NV_PER_OWNERWRITE);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 auth %x\n",ret); 
        return -1; 
    }
    
    /* next it holds 40 bytes of data */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE, 0, space_size);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_SetAttribUint32 size%x\n",ret); 
        return -1; 
    }
    
    /* No authorization needed to read from this NVRAM the way it was created. */
    /* Read from the NVRAM space */
    ret = Tspi_NV_ReadValue(hNVStore,0, &ulDataLength, &rdata);
    if (ret!=TSS_SUCCESS) 
    { 
        DBG("Tspi_NV_ReadValue: %x\n",ret); 
        return -1; 
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
    TSS_RESULT        ret;
    BYTE              wks[20]; // Place to put the well known secret

    memset(wks,0,20);// Set wks to the well known secret of 20 bytes of all zeros
    
    // Pick the TPM you are talking to in this case the system TPM (which you connect to with ¡°NULL¡±)
    ret =Tspi_Context_Create(hContext); 
    DBG(" Create a Context\n",ret); 
    ret = Tspi_Context_Connect(*hContext, NULL); 
    DBG("Connect to TPM\n", ret);
        
    // Get the TPM handle
    ret = Tspi_Context_GetTpmObject(*hContext, hTPM); 
    DBG("GetTPMHandle\n",ret); 

    //Get the SRK handle
    ret = Tspi_Context_LoadKeyByUUID(*hContext, TSS_PS_TYPE_SYSTEM, SRK_UUID, hSRK); 
    DBG("Tspi_Context_Connect\n",ret);

    //Get the SRK policy
    ret = Tspi_GetPolicyObject(*hSRK, TSS_POLICY_USAGE, hSRKPolicy); 
    DBG("Get TPM Policy\n" ,ret);
    
    // Then we set the SRK policy to be the well known secret
    ret = Tspi_Policy_SetSecret(*hSRKPolicy,TSS_SECRET_MODE_SHA1,20, wks); 
    // Note: TSS_SECRET_MODE_SHA1 says ¡°Don¡¯t hash this. Just use the 20 bytes as is.
    DBG("Tspi_Policy_Set_Secret\n",ret);
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
