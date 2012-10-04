#include "file.h"
#include "log.h"
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>

#define MAX_HOSTNAME_LEN    256

// Append. And create file if not exist.
void AppendFile(char* filepath, char* str, int len)
{  
    int fd, ret;

    /* Open the file.  Clobber it if it exists. */
    fd = open (filepath, O_WRONLY | O_CREAT | O_APPEND, S_IRWXU | S_IRWXG | S_IRWXO);
    
    if(fd == -1)
    {
        PRINT("Error in openning file\n");
        exit(1);
    }
    
    /* Seek the beginning of the file */
    //lseek (fd, 0, SEEK_END);
    
    ret = write(fd, str, len);
    PRINT("%d",ret);
    fsync(fd);
    close (fd);
}

// Write. And create file if not exist.
void WriteFile(char* filepath, char* str, int len)
{  
    int fd, ret;

    /* Open the file.  Clobber it if it exists. */
    fd = open (filepath, O_RDWR | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
    
    if(fd == -1)
    {
        PRINT("Error in openning file\n");
        exit(1);
    }
    
    ret = write(fd, str, len);
    PRINT("%d",ret);
    fsync(fd);
    close (fd);
}

void GenerateFilePath(char* src, char* filename)
{
    char hostname[MAX_HOSTNAME_LEN];
    char* home;
    
    home = getenv("HOME");
    
    gethostname(hostname, MAX_HOSTNAME_LEN);
    hostname[MAX_HOSTNAME_LEN - 1] = '\0';
    
    strcpy(src, home);
    strcat(src, "/tpmdos/tpm-data/");
    strcat(src, filename);
    strcat(src, "-");
    strcat(src, hostname);
    src[MAX_FILEPATH_LEN - 1] = '\0';
}
