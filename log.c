#include "log.h"
#include "file.h"
#include <string.h>
#include <inttypes.h>

TPMDOS_META g_meta;
    
// Write. Create if not exist.
static void WriteMeta()
{
    char meta_fn[MAX_FILEPATH_LEN];
    char str[256] = {0};
    
    sprintf(str, "%llu", 
            g_meta.g_run_cnt
    );
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    WriteFile(meta_fn, str, strlen(str));
}

void IncGlobalMeta(int item)
{
    g_meta.g_run_cnt++;
    
    switch(item)
    {
        case L_TEST:
            g_meta.l_test++;
            break;

        default:
            PRINT("Invalid Update Item.\n");
            g_meta.g_run_cnt--; // We should not count this test.
            return;
    }
    
    WriteMeta();
}


// Read metafile if exist, otherwise all are 0. 
void ReadMeta()
{
    FILE *fp;
    char meta_fn[MAX_FILEPATH_LEN];
    
    GenerateFilePath(meta_fn, META_FILEPATH);
    fp = fopen(meta_fn, "r");
    if (fp == NULL) {
        memset(&g_meta, 0, sizeof(g_meta));
        return;
    }
    
    fscanf(fp, "%llu", 
            &g_meta.g_run_cnt
    );
    
    fclose(fp);
}