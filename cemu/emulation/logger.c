#include "logger.h"
#include "state.h"
#include "disassembler.h"

static FILE* log_files[LOG_FILES];
static char str[6];
static char instruction_str[20];

static bool continue_log = true;
bool continue_test = true;
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
	log_files[STATUS] = fopen("status_emulator.txt", "w");
	log_files[INSTRUCTIONS] = fopen("instructions.txt", "w");
}

void debug_print(const char* message, int level) {
	set_level(level);
	printf("[%s] : %s\n", str, message);
}

void log_emulator_status_ch(const char ch) {
	fprintf(log_files[STATUS], "%c", ch);//not printing for whatever reason. IM JACKED.
	printf("%c", ch);
}

void log_emulator_status(const char* message, int level) {
	set_level(level);
	fprintf(log_files[STATUS], "\n[%s] : %s\n", str, message);
}

//the current state, the number of cycles in the current instruction, and 


void log_instruction_state(state_8080 state) {
	if (!continue_log)
		return;
	uint8_t bits = 0x00;
	bits |= (state.flag.s << 7);
	bits |= (state.flag.z << 6);
	bits |= (state.flag.ac << 4);
	bits |= (state.flag.p << 2);
	bits |= (1 << 1);
	bits |= (state.flag.c);

	disassemble_8080(state.memory, state.pc, instruction_str);

	fprintf(log_files[INSTRUCTIONS], "%04X%04X%04X%04X%04X%04X",
		(state.A << 8 | bits), (state.B << 8 | state.C), (state.D << 8 | state.E), (state.H << 8 | state.L), state.pc, state.sp);
	fprintf(log_files[INSTRUCTIONS], "%02X", state.memory[state.pc]);

	fprintf(log_files[INSTRUCTIONS], "\n");

}

void terminate_logger() {
	fflush(log_files[STATUS]);
	fclose(log_files[STATUS]);

	fflush(log_files[INSTRUCTIONS]);
	fclose(log_files[INSTRUCTIONS]);
}
