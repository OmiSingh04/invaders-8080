#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>
#include "state.h"


long file_size(FILE* file){
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	printf("%ld - Size of the file\n", size);
	return size;
}

uint8_t *read_rom(FILE* rom, long size){
	uint8_t* buffer = malloc(size);
	if(buffer == NULL) {
		printf("NULL"); 
		return buffer;
	}
	memset(buffer, 0, size);
	

	size = fread(buffer, sizeof(uint8_t), size, rom);
	printf("Read %u bytes of data...\n", size);
	return buffer;
}
