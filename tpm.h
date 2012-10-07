#ifndef _TPM_H
#define _TPM_H

#include "log.h"

#include <stdio.h>
#include <string.h>
#include <tss/tss_error.h>
#include <tss/platform.h>
#include <tss/tss_defines.h>
#include <tss/tss_typedef.h>
#include <tss/tss_structs.h>
#include <tss/tspi.h>
#include <trousers/trousers.h>

#define LOG_TPM(message,tResult) {PRINT("%s returned 0x%08x. %s.\n", message , tResult , Trspi_Error_String(tResult));}

#define OWNER_PASSWD  "cylab"
#define OWNER_PASSWD_LENGTH  (strlen(OWNER_PASSWD))

#define TPMUTIL_GETCAP_ERROR    -1

#define TPM_ATTIBUTE_ERROR   -1
#define TPM_NVREAD_ERROR   -2
#define TPM_NVWRITE_ERROR   -3
#define TPM_NVDEFINE_ERROR   -4
#define TPM_POLICY_ERROR    -5

extern void InitTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
);

extern void FinalizeTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
);

// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_POLICY_ERROR> on tpm policy related errors.
// return <TPM_NVDEFINE_ERROR> on define operation error.
// return 0 if normal.
extern int WriteNVRAM(   
TSS_HCONTEXT* hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 attribute,
    UINT32 ulDataLength, 
    BYTE* data
);

// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_NVREAD_ERROR> on read operation error.
// return 0 if normal.
extern int ReadNVRAM(
    TSS_HCONTEXT* hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
);

// Define a space in NVRAM. 
// NOTE: It is the caller's responsibility to check if the index is available
// return <TPM_ATTIBUTE_ERROR> on tspi attribute related errors.
// return <TPM_POLICY_ERROR> on tpm policy related errors.
// return <TPM_NVDEFINE_ERROR> on define operation error.
// return 0 if normal.
extern int DefineNVRAM(
    TSS_HCONTEXT *hContext, 
    TSS_HTPM* hTPM,
    UINT32 space_size, 
    UINT32 nv_index,
    UINT32 attribute
);

// Return 1 - defined. 0- undefined. 
// Return TPMUTIL_GETCAP_ERROR on error.
extern int IsNVIndexDefined(
    TSS_HTPM* hTpm, 
    UINT32 nv_index
);

#endif