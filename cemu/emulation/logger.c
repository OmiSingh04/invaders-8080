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


void log_instruction_state_ret(state_8080 state) {
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

	fprintf(log_files[INSTRUCTIONS], "\nAF = %04X BC = %04X DE = %04X HL = %04X PC = %04X SP = %04X",
		(state.A << 8 | bits), (state.B << 8 | state.C), (state.D << 8 | state.E), (state.H << 8 | state.L), 0x0005, (state.sp - 2) & 0xFFFF);

	fprintf(log_files[INSTRUCTIONS], " (%02X %02X %02X %02X)", 0xC9, 0, 0, 0);

	fprintf(log_files[INSTRUCTIONS], "\nF = %d SF = %d ZF = %d HF = %d PF = %d CF = %d", bits, state.flag.s, state.flag.z, 
		state.flag.ac, state.flag.p, state.flag.c);

	//fprintf(log_files[INSTRUCTIONS], "%s\n", instruction_str);
	//fprintf(log_files[INSTRUCTIONS], "\n - %s", instructions[readByte(s, state.pc)].disassemble);
	fprintf(log_files[INSTRUCTIONS], "\n");
}



void log_instruction_state_2(state_8080 state) {
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

	fprintf(log_files[INSTRUCTIONS], "\nAF = %04X BC = %04X DE = %04X HL = %04X PC = %04X SP = %04X",
		(state.A << 8 | bits), (state.B << 8 | state.C), (state.D << 8 | state.E), (state.H << 8 | state.L), state.pc, state.sp);

	fprintf(log_files[INSTRUCTIONS], " (%02X %02X %02X %02X)",
	state.memory[state.pc], state.memory[(state.pc + 1) & 0xFFFF], 
	state.memory[(state.pc + 2) & 0xFFFF], state.memory[(state.pc + 3) & 0xFFFF]);

	fprintf(log_files[INSTRUCTIONS], "\nF = %d SF = %d ZF = %d HF = %d PF = %d CF = %d", bits, state.flag.s, state.flag.z, 
		state.flag.ac, state.flag.p, state.flag.c);


	//fprintf(log_files[INSTRUCTIONS], "%s\n", instruction_str);
	//fprintf(log_files[INSTRUCTIONS], "\n - %s", instructions[readByte(s, state.pc)].disassemble);
	fprintf(log_files[INSTRUCTIONS], "\n");

}

void terminate_logger() {
	fflush(log_files[STATUS]);
	fclose(log_files[STATUS]);

	fflush(log_files[INSTRUCTIONS]);
	fclose(log_files[INSTRUCTIONS]);
}

void log_instruction_state(state_8080 state) {

	if (!continue_log)
		return;

	disassemble_8080(state.memory, state.pc, instruction_str);
	fprintf(log_files[INSTRUCTIONS], "%d instructions ran!\n", state.instructions_ran);
	fprintf(log_files[INSTRUCTIONS], "Cycles Consumed - %ld\n", state.cycles_consumed);
	fprintf(log_files[INSTRUCTIONS], "Registers - B - %02X C - %02X D - %02X E - %02X H - %02X L - %02X A - %02X\n", state.B, state.C, state.D, state.E, state.H, state.L, state.A);
	fprintf(log_files[INSTRUCTIONS], "flag c|p|s|z|ac- %d %d %d %d %d\n", state.flag.c, state.flag.p, state.flag.s, state.flag.z, state.flag.ac);
	uint8_t bits = 0x00;
	bits |= (state.flag.s << 7);
	bits |= (state.flag.z << 6);
	bits |= (state.flag.ac << 4);
	bits |= (state.flag.p << 2);
	bits |= (1 << 1);
	bits |= (state.flag.c);
	fprintf(log_files[INSTRUCTIONS], "flag %02X\n", bits);
	fprintf(log_files[INSTRUCTIONS], "pc - %04X\n", state.pc);
	fprintf(log_files[INSTRUCTIONS], "sp - %04X\n", state.sp);
	fprintf(log_files[INSTRUCTIONS], "mem[stack] = %02X\n", state.memory[state.sp]);
	fprintf(log_files[INSTRUCTIONS], "mem[stack + 1] = %02X\n", state.memory[(state.sp + 1) & 0xFFFF]);
	fprintf(log_files[INSTRUCTIONS], "*********************************************************\n\n");
	fprintf(log_files[INSTRUCTIONS], "%s\n", instruction_str);
}
