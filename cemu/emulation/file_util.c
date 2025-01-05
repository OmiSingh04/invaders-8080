#include<stdio.h>
#include<stdbool.h>
#include <allegro5/allegro.h>
#include<stdint.h>
#include<string.h>
#include "state.h"
#include "logger.h"
#include "file_util.h"

static FILE* file = NULL;

static bool read_file(FILE* file, uint8_t* buffer, int offset){
	long size = file_size(file);
	if (size > 65536) {
		debug_print("File too large", ERROR); 
		return false;
	}
	memset(buffer, 0, size);
	long read_size = fread(buffer + offset, sizeof(uint8_t), size, file);
	char str[30];
	sprintf(str, "%u - bytes read...", read_size);
	debug_print(str, DEBUG);
	if (read_size != size) {
		sprintf(str, "File size and bytes read differ in size!");
		debug_print(str, ERROR);
		return false;
	}
	return true;
}

static long file_size(FILE* file){
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);
	printf("%ld - Size of the file\n", size);
	return size;
}

int load_file(state_8080* cpu, char type){
	int offset = 0x100;
	switch (type) {
		case CPUDIAG:
			file = fopen(CPUDIAG_PATH, "rb");
			break;
		case EXM: 
			file = fopen(EXM_PATH, "rb");
			break;
		case PRE: 
			file = fopen(PRE_PATH, "rb");
			break;
		case CPUTEST: 
			file = fopen(DIAG_PATH, "rb");
			break;
		case ROM: 
			file = fopen(ROM_PATH, "rb");
			offset = 0;
			break;
		case EXER: 
			file = fopen(EXER_PATH, "rb");
			break;

		default:
			file = fopen(ROM_PATH, "rb");
			offset = 0;
	}
	//if (offset == 0x100) {
	//	cpu->memory[0] = 0xD3;
	//	cpu->memory[0x0005] = 0xD3;
	//	cpu->memory[0x0006] = 0x01;
	//	cpu->memory[0x0007] = 0xC9;
	//}

	bool x = read_file(file, cpu->memory, offset);
	if (type == CPUDIAG) {
		cpu->memory[368] = 0x7;    
		//the following will skip DAA test
		cpu->memory[0x59C] = 0xC3; //JMP    
		cpu->memory[0x59D] = 0xC2;    
		cpu->memory[0x59E] = 0x05;    
	}
	if (type != ROM) {
		cpu->memory[0] = 0xC3; // jump instruction
		cpu->memory[2] = 0x01; //CPM - jump to $100
		cpu->memory[5] = 0xC9; //CPM - call to $5 - return back
	}
	return (x) ? offset : -1;
}
