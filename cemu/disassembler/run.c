#include<stdio.h>
#include<stdint.h>
#include<stdlib.h>
#include "disassembler.h"


long file_size(FILE* file){
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	return size;
}



unsigned char *read_rom(FILE* rom, size_t size){
	unsigned char* buffer = malloc(size);
	if(buffer == NULL) {printf("NULL"); return buffer;}
	size = fread(buffer, sizeof(char), size, rom);
	printf("%u bytes read\n", size);
	return buffer;
}
	
int main(int argc, char* argv[]){
	FILE* file;
	puts(argv[1]);

	#if __linux__
		file = fopen(argv[1], "r");
	#elif _WIN32
		fopen_s(&file, argv[1], "rb");
	#endif
	size_t size = 0;
	if (!file) {
		perror("Could Not Open File");
		exit(1);
	}
	size = file_size(file);
	printf("%u bytes file\n", size);
	unsigned char* buffer = read_rom(file, size);	//returns a heap allocated string containing the rom
	uint16_t pc = 0;
	while (pc < size) 
		pc += disassembler_8080(buffer, pc);
}
