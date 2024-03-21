#include "logger.h"
#include "state.h"
#include "disassembler.h"

static FILE* log_files[LOG_FILES];
static char str[6];


static void set_level(int level) {
	switch (level) {
		case TRACE:{
			strcpy(str, "TRACE");
			break;
		}
		case ERROR:{
			strcpy(str, "ERROR");
			break;
		}
		case DEBUG:{
			strcpy(str, "DEBUG");
			break;
		}
		case BDOS:{
			strcpy(str, "BDOS");
		}
	}
}

void init_logger() {
	log_files[STATUS] = fopen("emulator_status.txt", "w");
	log_files[INSTRUCTIONS] = fopen("instructions.txt", "w");
}

void debug_print(const char* message, int level) {
	set_level(level);
	printf("[%s] : %s\n", str, message);
}

void log_instruction(state_8080 state, int cycles, int start_instruction, int end_instruction) {
	if (instructions_ran >= start_instruction && instructions_ran <= end_instruction) {
		fprintf(log_files[INSTRUCTIONS], "%d instructions ran!\n", instructions_ran);
		fprintf(log_files[INSTRUCTIONS], "Cycles run - %ld\n", cycles);
		fprintf(log_files[INSTRUCTIONS], "Registers - B - %02X C - %02X D - %02X E - %02X H - %02X L - %02X A - %02X\n", state.B, state.C, state.D, state.E, state.H, state.L, state.A);
		fprintf(log_files[INSTRUCTIONS], "flag c|p|s|z|ac- %d %d %d %d %d\n", state.flag.c, state.flag.p, state.flag.s, state.flag.z, state.flag.ac);
		uint8_t bits = 0x00;
		bits |= (state.flag.s << 7);
		bits |= (state.flag.z << 6);
		bits |= (state.flag.ac << 4);
		bits |= (1 << 1);
		bits |= (state.flag.c);
		fprintf(log_files[INSTRUCTIONS], "flag %02X\n", bits);
		fprintf(log_files[INSTRUCTIONS], "pc - %04X\n", state.pc);
		fprintf(log_files[INSTRUCTIONS], "sp - %04X\n", state.sp);
		fprintf(log_files[INSTRUCTIONS], "mem[stack] = %02X\n", state.memory[state.sp]);
		fprintf(log_files[INSTRUCTIONS], "mem[stack + 1] = %02X\n", state.memory[(state.sp + 1) & 0xFFFF]);
		fprintf(log_files[INSTRUCTIONS], "*********************************************************\n");
	}
}

void log_emulator_status(const char* message, int level) {
	set_level(level);
	fprintf(log_files[STATUS], "[%s] : %s", str, message);
}

void terminate_logger() {
	fflush(log_files[STATUS]);
	fclose(log_files[STATUS]);

	fflush(log_files[INSTRUCTIONS]);
	fclose(log_files[INSTRUCTIONS]);
}
