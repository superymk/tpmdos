#include "tpm.h"
#include "log_msg.h"
#include "tpm_driver_comm.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/*********The following parts come from tpm-tools **************/

// Export these headers manully import the function from tpm-tools-1.3.8 
extern TSS_RESULT getCapability(TSS_HTPM a_hTpm,
			 TSS_FLAG a_fCapArea,
			 UINT32 a_uiSubCapLen,
			 BYTE * a_pSubCap,
			 UINT32 * a_uiResultLen, BYTE ** a_pResult);

extern TSS_RESULT unloadNVDataPublic(UINT64 *offset, BYTE *blob, UINT32 bloblen,
                              TPM_NV_DATA_PUBLIC *v);

// Forward declaration in this file.
static void freeNVDataPublic(TPM_NV_DATA_PUBLIC *pub);

static TSS_RESULT getNVDataPublic(TSS_HTPM hTpm, TPM_NV_INDEX nvindex,
                           TPM_NV_DATA_PUBLIC **pub)
{
	TSS_RESULT res;
	UINT32 ulResultLen;
	BYTE *pResult;
	UINT64 off = 0;

	res = getCapability(hTpm, TSS_TPMCAP_NV_INDEX, sizeof(UINT32),
			    (BYTE *)&nvindex, &ulResultLen, &pResult);

	if (res != TSS_SUCCESS)
		return res;

	*pub = malloc(sizeof(TPM_NV_DATA_PUBLIC));
	if (*pub == NULL) {
		res = TSS_E_OUTOFMEMORY;
		goto err_exit;
	}

	res = unloadNVDataPublic(&off, pResult, ulResultLen, *pub);

	if (res != TSS_SUCCESS) {
		freeNVDataPublic(*pub);
		*pub = NULL;
	}
    
err_exit:
	return res;
}

static void freeNVDataPublic(TPM_NV_DATA_PUBLIC *pub)
{
	if (!pub)
		return;

	free(pub->pcrInfoRead.pcrSelection.pcrSelect);
	free(pub->pcrInfoWrite.pcrSelection.pcrSelect);
	free(pub);
}

static inline UINT32
Decode_UINT32(BYTE * y)
{
        UINT32 x = 0;

        x = y[0];
        x = ((x << 8) | (y[1] & 0xFF));
        x = ((x << 8) | (y[2] & 0xFF));
        x = ((x << 8) | (y[3] & 0xFF));

        return x;
}
/*******************tpm-tools Finished *************************/

// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_POLICY_ERROR> on tpm policy related errors.
// return <TPM_NVDEFINE_ERROR> on define operation error.
// return 0 if normal.
int WriteNVRAM(
    TSS_HCONTEXT* hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 attribute,
    UINT32 ulDataLength, 
    BYTE* data
) 
{
    TSS_RESULT      ret;
    TSS_HNVSTORE    hNVStore;
    TSS_HPOLICY     hNewPolicy, hDataPolicy;
    unsigned int    bytesToWrite = 0, off = 0;
    //char dataToStore[19]="This is some dat1.";
    
    *(unsigned int*)data = WRITE_MAGIC_HEADER;
    BeginPerf(WRITE_ATTRIB_PERF);
    
    /* Create a NVRAM object */
    ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Context_CreateObject: %x\n",ret); 
        
        EndPerf(WRITE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 index %x\n",ret); 
        
        EndPerf(WRITE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    ret = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, attribute);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 auth %x\n",ret); 
        
        EndPerf(WRITE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* next it holds <space_size> bytes of data */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE,0, space_size);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 size%x\n",ret); 
        
        EndPerf(WRITE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    EndPerf(WRITE_ATTRIB_PERF);
    BeginPerf(WRITE_POLICY_PERF);
    
    /* Set Policy for the NVRAM object using the Owner Auth */
    ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE, &hNewPolicy);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Context_CreateObject: %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    ret = Tspi_Policy_SetSecret(hNewPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Policy_SetSecret: %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    ret = Tspi_Policy_AssignToObject(hNewPolicy,hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM(" Tspi_Policy_AssignToObject: %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    /* Set Data Policy for the NVRAM object using the Owner Auth */
    ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE, &hDataPolicy);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Context_CreateObject (DataPolicy): %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    ret = Tspi_Policy_SetSecret(hDataPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Policy_SetSecret (DataPolicy): %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    ret = Tspi_Policy_AssignToObject(hDataPolicy,hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM(" Tspi_Policy_AssignToObject (DataPolicy): %x\n",ret); 
        
        EndPerf(WRITE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }

    EndPerf(WRITE_POLICY_PERF);
    BeginPerf(WRITE_NVWRITE_PERF);
    
    /* Write to the NVRAM space */
    //ret = Tspi_NV_WriteValue(hNVStore, 0, ulDataLength, data);
    #define WRITE_CHUNK_SIZE   1024
    bytesToWrite = ulDataLength;
	while (bytesToWrite > 0) {
		UINT32 chunk = (bytesToWrite > WRITE_CHUNK_SIZE)
			       ? WRITE_CHUNK_SIZE
			       : bytesToWrite;
            ret = Tspi_NV_WriteValue(hNVStore, off, chunk, &data[off]);
            if (ret != TSS_SUCCESS) 
            { 
                LOG_TPM("Tspi_NV_WriteValue: %x\n",ret); 
                
                EndPerf(WRITE_NVWRITE_PERF);
                return TPM_NVWRITE_ERROR;
            }

		bytesToWrite -= chunk;
		off += chunk;
	}
    
    EndPerf(WRITE_NVWRITE_PERF);
    
    /* Read the NVWrite performance in kernel mode */
    {
        TPM_DRIVER_NVWRITE_TIMER result;
        
        ReadDriverNVWriteResult(&result);
        SetPerf(WRITE_KERNEL_PERF, result.timer_result);
        SetPerf(WRITE_KERNEL_TIMEOUT_JIFFIES, result.max_jiffies);
    }
    
    return 0;
}

// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_NVREAD_ERROR> on read operation error.
// return 0 if normal.
int ReadNVRAM(
    TSS_HCONTEXT *hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    UINT32 needAuth,
    BYTE* data
)
{
    TSS_HNVSTORE    hNVStore;
    TSS_HTPM        hTPM;
    TSS_HPOLICY     hNewPolicy, hDataPolicy;
    TSS_RESULT      ret;
    BYTE*           rdata = 0;
    
    BeginPerf(READ_ATTRIB_PERF);
    
    /* Create a NVRAM object */
    ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Context_CreateObject: %x\n",ret); 
        
        EndPerf(READ_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 index %x\n",ret); 
        
        EndPerf(READ_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    ret = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0, TPM_NV_PER_OWNERWRITE);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 auth %x\n",ret); 
        
        EndPerf(READ_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* next it holds <space_size> bytes of data */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE, 0, space_size);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 size%x\n",ret); 
        
        EndPerf(READ_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    EndPerf(READ_ATTRIB_PERF);
    
    if(needAuth)
    {
        BeginPerf(READ_POLICY_PERF);
        
        ret = Tspi_Context_GetTpmObject(*hContext, &hTPM);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM("Tspi_Context_GetTpmObject: %x\n",ret); 
            
            EndPerf(READ_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }
        
        /* Set Policy for the NVRAM object using the Owner Auth */
        ret = Tspi_GetPolicyObject(hTPM, TSS_POLICY_USAGE, &hNewPolicy);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM("Tspi_GetPolicyObject: %x\n",ret); 
            
            EndPerf(READ_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }
        
        ret = Tspi_Policy_SetSecret(hNewPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM("Tspi_Policy_SetSecret: %x\n",ret); 
            
            EndPerf(READ_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }
        
        /* Set Data Policy for the NVRAM object using the Owner Auth */
        ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_POLICY, TSS_POLICY_USAGE, &hDataPolicy);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM("Tspi_Context_CreateObject (DataPolicy): %x\n",ret); 
            
            EndPerf(WRITE_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }
        
        ret = Tspi_Policy_SetSecret(hDataPolicy, TSS_SECRET_MODE_PLAIN, OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM("Tspi_Policy_SetSecret (DataPolicy): %x\n",ret); 
            
            EndPerf(WRITE_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }
        
        ret = Tspi_Policy_AssignToObject(hDataPolicy,hNVStore);
        if (ret!=TSS_SUCCESS) 
        { 
            LOG_TPM(" Tspi_Policy_AssignToObject (DataPolicy): %x\n",ret); 
            
            EndPerf(WRITE_POLICY_PERF);
            return TPM_POLICY_ERROR; 
        }

        EndPerf(READ_POLICY_PERF);
    }

    BeginPerf(READ_NVREAD_PERF);
    
    /* No authorization needed to read from this NVRAM the way it was created. */
    /* Read from the NVRAM space */
    ret = Tspi_NV_ReadValue(hNVStore,0, &ulDataLength, &rdata);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_NV_ReadValue: %x\n",ret);

        EndPerf(READ_NVREAD_PERF);        
        return TPM_NVREAD_ERROR; 
    }
    
    EndPerf(READ_NVREAD_PERF);
    
    memcpy(data, rdata, ulDataLength);
    return 0;
}

// Define a space in NVRAM. 
// NOTE: It is the caller's responsibility to check if the index is available
// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_POLICY_ERROR> on tpm policy related errors.
// return <TPM_NVDEFINE_ERROR> on define operation error.
// return 0 if normal.
int DefineNVRAM(
    TSS_HCONTEXT *hContext, 
    TSS_HTPM* hTPM, 
    UINT32 space_size, 
    UINT32 nv_index,
    UINT32 attribute
)
{
    TSS_HNVSTORE hNVStore;
    TSS_RESULT      ret;
    TSS_HPOLICY     hTPMPolicy;
    
    BeginPerf(DEFINE_ATTRIB_PERF);
    
    /* Create a NVRAM object */
    ret = Tspi_Context_CreateObject(*hContext, TSS_OBJECT_TYPE_NV, 0, &hNVStore);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Context_CreateObject: %x\n",ret); 
        
        EndPerf(DEFINE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /*Next its arbitrary index will be 0x00011101 (00-FF are taken, along with 00011600). */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_INDEX,0, nv_index);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 index %x\n",ret); 
        
        EndPerf(DEFINE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* set its Attributes. First it is only writeable by the owner */
    ret = Tspi_SetAttribUint32(hNVStore,TSS_TSPATTRIB_NV_PERMISSIONS, 0,
        attribute);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 auth %x\n",ret); 
        
        EndPerf(DEFINE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    /* next it holds 40 bytes of data */
    ret = Tspi_SetAttribUint32(hNVStore, TSS_TSPATTRIB_NV_DATASIZE,
        0, space_size);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_SetAttribUint32 size%x\n",ret); 
        
        EndPerf(DEFINE_ATTRIB_PERF);
        return TPM_ATTIBUTE_ERROR; 
    }
    
    EndPerf(DEFINE_ATTRIB_PERF);
    BeginPerf(DEFINE_POLICY_PERF);
    
    /* In order to either instantiate or write to the NVRAM location in NVRAM, owner_auth is required. In the case of NVRAM, owner_auth comes from the TPM's policy object. We will put it in here. */
    /* First we get a TPM policy object*/
    ret = Tspi_GetPolicyObject(*hTPM, TSS_POLICY_USAGE, &hTPMPolicy);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_GetPolicyObject: %x\n",ret); 
        
        EndPerf(DEFINE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    /* Then we set the Owner's Authorization as its secret */
    ret = Tspi_Policy_SetSecret(hTPMPolicy, TSS_SECRET_MODE_PLAIN, 
        OWNER_PASSWD_LENGTH, (BYTE*)OWNER_PASSWD);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM("Tspi_Policy_SetSecret: %x\n",ret); 
        
        EndPerf(DEFINE_POLICY_PERF);
        return TPM_POLICY_ERROR; 
    }
    
    EndPerf(DEFINE_POLICY_PERF);
    BeginPerf(DEFINE_NVDEFINE_PERF);
    
    /* Create the NVRAM space */
    ret = Tspi_NV_DefineSpace(hNVStore,0,0);
    if (ret!=TSS_SUCCESS) 
    { 
        LOG_TPM(" Tspi_NV_DefineSpace: %x\n",ret); 
        
        EndPerf(DEFINE_NVDEFINE_PERF);
        return TPM_NVDEFINE_ERROR; 
    }
    
    EndPerf(DEFINE_NVDEFINE_PERF);
    
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

    // Sanity Checks
    assert(hContext != NULL);
    assert(hTPM != NULL);
    assert(hSRK != NULL);
    assert(hSRKPolicy != NULL);
    
    memset(wks,0,20);// Set wks to the well known secret of 20 bytes of all zeros
    
    // Pick the TPM you are talking to in this case the system TPM (which you connect to with ¡°NULL¡±)
    ret = Tspi_Context_Create(hContext); 
    LOG_TPM("Create a Context\n",ret); 
    ret = Tspi_Context_Connect(*hContext, NULL); 
    LOG_TPM("Connect to TPM\n", ret);
        
    // Get the TPM handle
    ret = Tspi_Context_GetTpmObject(*hContext, hTPM); 
    LOG_TPM("GetTPMHandle\n",ret); 

    //Get the SRK handle
    ret = Tspi_Context_LoadKeyByUUID(*hContext, TSS_PS_TYPE_SYSTEM, SRK_UUID, hSRK); 
    LOG_TPM("Tspi_Context_Connect\n",ret);

    //Get the SRK policy
    ret = Tspi_GetPolicyObject(*hSRK, TSS_POLICY_USAGE, hSRKPolicy); 
    LOG_TPM("Get TPM Policy\n" ,ret);
    
    // Then we set the SRK policy to be the well known secret
    ret = Tspi_Policy_SetSecret(*hSRKPolicy,TSS_SECRET_MODE_SHA1,20, wks); 
    // Note: TSS_SECRET_MODE_SHA1 says ¡°Don¡¯t hash this. Just use the 20 bytes as is.
    LOG_TPM("Tspi_Policy_Set_Secret\n",ret);
}

void FinalizeTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
)
{
    // Sanity Checks
    assert(hContext != NULL);
    assert(hTPM != NULL);
    assert(hSRK != NULL);
    assert(hSRKPolicy != NULL);
    
    /* Clean up */
    Tspi_Context_Close (*hTPM);
    Tspi_Context_Close (*hSRK);
    Tspi_Context_Close (*hSRKPolicy);
    // this frees memory that was automatically allocated for you
    Tspi_Context_FreeMemory(*hContext, NULL);
    
    Tspi_Context_Close(*hContext);
}

// Return 1 - defined. 0- undefined. 
// Return TPMUTIL_GETCAP_ERROR on error.
int IsNVIndexDefined(TSS_HTPM* hTpm, UINT32 nv_index)
{
    UINT32 i, ulResultLen;
	BYTE *pResult = NULL;
    
    if (getCapability(*hTpm, TSS_TPMCAP_NV_LIST, 0, NULL,
			  &ulResultLen, &pResult) != TSS_SUCCESS) {
		PRINT("GetCapability Error\n");
        return TPMUTIL_GETCAP_ERROR;
	}

	for (i = 0; i < ulResultLen/sizeof(UINT32); i++) {
		UINT32 nvi;
		nvi = Decode_UINT32(pResult + i * sizeof(UINT32));

        if (nvi == (UINT32)nv_index)
        {
            return 1;
        }
	}
    
    return 0;
}
