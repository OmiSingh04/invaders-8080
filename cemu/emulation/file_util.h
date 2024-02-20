#ifndef FILE_UTIL_H_
#define FILE_UTIL_H_

#include <string.h>
long file_size(FILE*);
uint8_t* read_rom(FILE*, long);
#endif
