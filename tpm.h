#ifndef _TPM_H
#define _TPM_H

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
#define DBG(message,tResult) if(DEBUG) {printf("[tpm.c] (Line %d, %s) %s returned 0x%08x. %s.\n", __LINE__ , __func__ , message , tResult , Trspi_Error_String(tResult));}

#define OWNER_PASSWD  "a"
#define OWNER_PASSWD_LENGTH  (strlen(OWNER_PASSWD))

extern void InitTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
);

void FinalizeTPM(
    TSS_HCONTEXT* hContext, 
    TSS_HTPM* hTPM,
    TSS_HKEY* hSRK,
    TSS_HPOLICY* hSRKPolicy
);

extern int WriteNVRAM(   
TSS_HCONTEXT hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
);

extern int ReadNVRAM(
    TSS_HCONTEXT hContext, 
    UINT32 space_size, 
    UINT32 nv_index, 
    UINT32 ulDataLength, 
    BYTE* data
);

#endif