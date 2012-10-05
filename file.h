#ifndef _FILE_H
#define _FILE_H

#define MAX_FILEPATH_LEN    512

// Append. And create file if not exist.
extern void AppendFile(char* filepath, char* str, int len);

// Write. And create file if not exist.
extern void WriteFile(char* filepath, char* str, int len);

// Use this function to extend the result's file name to identify each machine.
extern void GenerateFilePath(char* src, char* filename);

extern unsigned int GetFileSize(char* filepath);

extern int IsFileExist(char* filepath);
#endif