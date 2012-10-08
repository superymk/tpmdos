#ifndef _STR_H
#define _STR_H

#include <stdio.h>
#include <string.h>

#define FLIP_BYTES(str, len)  if(str[0]){memset(str, 0, len);} else {memset(str, 0xFF, len);} 

#endif
