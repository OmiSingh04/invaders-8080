#ifndef STATE_8080_H_
#define STATE_8080_H_

#include <stdbool.h>
#include <stdint.h>

extern int instructions_ran;

typedef struct flag_byte{
	uint8_t s	:1;
	uint8_t z	:1;
	uint8_t		:1;
	uint8_t ac	:1;
	uint8_t		:1;
	uint8_t p	:1;
	uint8_t		:1;
	uint8_t c	:1;
} flag_byte; //i might make it a union with another char
 
typedef struct state_8080{ //registers
	uint8_t B;
	uint8_t C;
	uint8_t D;
	uint8_t E;
	uint8_t H;
	uint8_t L;
	uint8_t A;

	bool interrupt;

	flag_byte flag;

	uint16_t pc;//program counter
	uint16_t sp;//stack pointer
	uint8_t shift_offset;//when reading from port 3, this register is read, and the 8 bits after this offset are given.
	uint16_t shift_register;//additional shift register was added to the space invaders game as there are no shift instructions.
	
	uint8_t* memory;

	unsigned long cycles_consumed;
	unsigned long instructions_ran;

	bool halted;

} state_8080;

int init_state(state_8080*);
int parity_check(uint8_t);
int emulate_8080(state_8080*, int*);
bool generate_vblank(state_8080*, uint8_t);
//IN and OUT instructions, with respect to the space invaders.
void machine_in(state_8080*, int);
void machine_out(state_8080*, int);
void deinit_state(state_8080*);
#endif
