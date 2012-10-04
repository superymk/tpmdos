#ifndef _FILE_H
#define _FILE_H

// Append. And create file if not exist.
extern void AppendFile(char* filepath, char* str, int str_len);

// Write. Create if not exist.
extern void WriteMeta();

// Read metafile if exist, otherwise all are 0. 
extern void ReadMeta();

// Update the log daily to FTP server.
extern void DailySendFile(char* filepath, char* target);

#endif