#include<stdio.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include "logger.h"

static long file_size(FILE* file){
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	char str[30];
	sprintf(str, "%ld - Size of the file", size);
	debug_print(str, DEBUG);
	return size;
}

bool read_file(FILE* file, uint8_t* buffer, int offset){
	long size = file_size(file);

	if (size > 65536) {
		debug_print("Cannot load file to memory", ERROR); 
		return false;
	}
	memset(buffer, 0, size);
	long read_size = fread(buffer + offset, sizeof(uint8_t), size, file);
	char str[30];
	sprintf(str, "%u - bytes read...", read_size);
	debug_print(str, DEBUG);
	if (read_size != size) {
		sprintf(str, "File Size and Bytes read differ in size!");
		debug_print(str, ERROR);
		return false;
	}

	return true;
}

