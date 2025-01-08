#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "state.h"
#include "input.h"
#include "sound.h"
#include "logger.h"

static bool cpm_call = false;
extern bool game;




int init_state(state_8080* state){ //the state will be defined in the main
	//init the state of the cpu too!!
	state->memory = malloc(sizeof(char) * 65536);
	if (state->memory == NULL) {
		debug_print("Could not allocate heap.", ERROR);
		return false;
	}
	memset(state->memory, 0, 65536);
	debug_print("Allocated emulator memory.", DEBUG);
	return true;
}

void deinit_state(state_8080* state) {
	free(state->memory);
}


int parity_check(uint8_t byte){
	//i guess ill just take bit by bit
	uint8_t bits = 0;

	while(byte != 0){
		bits += byte % 2;//LSB
		byte = byte >> 1;
	}
	return (bits % 2 == 0);
}

/*
;      (2) INSTRUCTIONS NOT TESTED ARE "HLT","DI","EI","RIM","SIM",
;          AND "RST 0" THRU "RST 7"
;
*/


/* In the 8080 - 
* IN - 8 bit data byte is read from a device (a number in the second byte of the instruction) and put it in the accumulator.
* OUT - 8 bit data byte is written from the accumulator to a device (a number in the second byte of the instruction)
*/


bool generate_vblank(state_8080* state, uint8_t opcode){//emulate the generation of vblank interrupts from the video controller
	//actually the 
	//opcode - the generated 
	//https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html#:~:text=is%20RST%2010.-,If,-I%20understand%20this
	//the controller generates CF at half screen and D7 at the end.
	if (!state->interrupt)
		return false;
	uint8_t exp = opcode & 0x38;
	uint16_t ret = state->pc + 1;
	state->memory[state->sp - 1] = ret >> 8; 
	state->memory[state->sp - 2] = ret & 0xFF;
	state->pc = exp;		
	state->sp -= 2;
	return true;
}

//the below two functions make this 8080 emulation impure, away from 8080 and towards space invaders emulation
void machine_in(state_8080* state, int port) {
	//READING FROM PORT 3 RETURNS 8 BITS OFF THE SHIFT REGISTER BASED ON THE OFFSET.
	switch(port){
		case 0: {
			//never used by the code - 
			printf("This was indeed used by the code it seems...");
			//https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html#inputs:~:text=Read%0A%20%20%2000%20%20%20%20%20%20%20%20INPUTS%20(-,Mapped,-in%20hardware%20but
			break;
		}
		case 1: {
			//https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html#:~:text=demux%20port%207%20%3F-,Port,-1%0A%20bit%200
			//coin 2p_start 1p_start 1 1p_fire 1p_left 1p_right nil
			uint8_t byte = 0;
		
			byte |= ((!key_state[COIN_INSERT]) << 0);
			byte |= (key_state[P2_START] << 1);
			byte |= (key_state[P1_START] << 2);
			byte |= (1 << 3);
			byte |= (key_state[P1_FIRE] << 4);
			byte |= (key_state[P1_LEFT] << 5);
			byte |= (key_state[P1_RIGHT] << 6);
			/*
			int place = 1;
			for (int i = 0; i < 8; i++, place <<= 1) {
				printf("%d ", (byte & place) != 0);
			}
			*/
			state->A = byte;
			break;
		}
		case 2: {
			//DIP_1(life) DIP_2(life) TILT DIP_3(extra ship at) P2_shot P2_left p2_right dip_7(toggle coin info)
			//THESE DIPSWITCHES ARE OWNER CONTROLLED I THINK? ill just set them to constants
			//i have put the settings - 3 ships per life, no tilt, extra ship at 1000, coin info shall be displayed
			
			uint8_t byte = 0;

			byte |= (1 << 3);
			byte |= (key_state[P2_FIRE] << 4);
			byte |= (key_state[P2_LEFT] << 5);
			byte |= (key_state[P2_RIGHT] << 6);
			byte |= (0 << 7);
	
			state->A = byte;
			break;
		}

		case 3: {
			//pass 8 bits based on the offset to the accumulator - Asm Manual - Input/Output Page 2
			//https://computerarcheology.com/Arcade/SpaceInvaders/Hardware.html
			state->A = (state->shift_register << state->shift_offset) >> 8;
			break;
		}
	}
}

void machine_out(state_8080* state, int port) {
	//WRITING TO - 
	switch (port) {
		// Port 2 specifies the shift amount
		case 2: {
			state->shift_offset = state->A & 0x07;//0111b - only bit 0, 1 and 2
			break;
		}

		case 3: {
			//emulating this is as easy as reading the individual bits and playing the necessary sounds yourself. (i think)
			if (state->A & 0x1) play_sound(UFO);
			if (state->A & 0x2) play_sound(SHOOT);
			if (state->A & 0x4) play_sound(DIE);
			if (state->A & 0x8) play_sound(KILL);
			break;
		}
		case 5: {
			if (state->A & 0x1) play_sound(FLEET_MOVEMENT_1);
			if (state->A & 0x2) play_sound(FLEET_MOVEMENT_2);
			if (state->A & 0x4) play_sound(FLEET_MOVEMENT_3);
			if (state->A & 0x8)	play_sound(FLEET_MOVEMENT_4);
			break;
		}
		// Port 4 shifts the register. 
		case 4: {
			state->shift_register = state->A << 8 | (state->shift_register >> 8);
			break;
		}

	}
	
}


static void log_cpm(const char* str) {
	//assumes str is '$' terminated.
	int length = 0;
	char* ptr = str;
	while (*ptr != '$') {
		printf("%c", *ptr);
		ptr++;
		length++;
	}
	length++;//should not overflow
	char *output = malloc(length);
	if (output == NULL)
		debug_print("Could not alloc for string", ERROR);
	else {
		memset(output, 0, length);
		char* ptr = output;
		while (*str != '$') {
			ptr++;
			str++;
			*ptr = *str;
		}
		log_emulator_status(output, BDOS);
		printf("%s", output);
	}
	free(output);
}

//perhaps having seperate functions for 8 bit addition and subtraction may help...

//CYCLES CONSUMED - CLOCK CYCLES. not MACHINE cycles.
int emulate_8080(state_8080* state, int* cycles_consumed){//per instruction
	//going to take in 1 instruction... it is going to perform that instruction
	uint8_t* opcode = &state->memory[state->pc];
	*cycles_consumed = 1;//by default, it is 1. Now where it does consume more, i will simply change it.
	int bytes = 1;//assume 1 byte instruction
	switch (*opcode) {
			/* NOP (or undefined by the Reference Card)*/
		case 0x00:
		case 0x10:
		case 0x08:
		case 0x18:
		case 0x20:
		case 0x28:
		case 0x30:
		case 0x38:
		case 0xCB:
		case 0xD9:
		case 0xDD:
		case 0xED:
		case 0xFD:
			*cycles_consumed = 4;
			break;
			//59
			/*LXI*/
		case 0x01:
			state->B = opcode[2];
			state->C = opcode[1];
			bytes = 3;
			*cycles_consumed = 10;
			break;

		case 0x11:
			state->D = opcode[2];
			state->E = opcode[1];
			*cycles_consumed = 10;
			bytes = 3;
			break;

		case 0x21:
			state->H = opcode[2];
			state->L = opcode[1];
			*cycles_consumed = 10;
			bytes = 3;
			break;

		case 0x31:
			state->sp = opcode[2] << 8 | opcode[1];
			bytes = 3;
			*cycles_consumed = 10;
			break;

			/*MVI*/

		case 0x06:
			state->B = opcode[1];
			bytes = 2;
			*cycles_consumed = 7;
			break;
		case 0x0E:
			state->C = opcode[1];
			bytes = 2;
			*cycles_consumed = 7;
			break;
		case 0x16:
			state->D = opcode[1];
			*cycles_consumed = 7;
			bytes = 2;
			break;
		case 0x1E:
			state->E = opcode[1];
			*cycles_consumed = 7;
			bytes = 2;
			break;
		case 0x26:
			state->H = opcode[1];
			*cycles_consumed = 7;
			bytes = 2;
			break;
		case 0x2E:
			state->L = opcode[1];
			*cycles_consumed = 7;
			bytes = 2;
			break;
		case 0x36:
			state->memory[state->H << 8 | state->L] = opcode[1];
			*cycles_consumed = 10;
			bytes = 2;
			break;
		case 0x3E:
			state->A = opcode[1];
			*cycles_consumed = 7;
			bytes = 2;
			break;


			/* MOV instructions	*/
			/*simply move values between registers!	*/

		case 0x40:
			*cycles_consumed = 5;
			break;

		case 0x41:
			state->B = state->C;
			*cycles_consumed = 5;
			break;

		case 0x42:
			state->B = state->D;
			*cycles_consumed = 5;
			break;

		case 0x43:
			state->B = state->E;
			*cycles_consumed = 5;
			break;

		case 0x44:
			state->B = state->H;
			*cycles_consumed = 5;
			break;

		case 0x45:
			state->B = state->L;
			*cycles_consumed = 5;
			break;

		case 0x46:
			state->B = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x47:
			state->B = state->A;
			*cycles_consumed = 5;
			break;



			/* C	*/
		case 0x48:
			state->C = state->B;
			*cycles_consumed = 5;
			break;

		case 0x49:
			state->C = state->C;
			*cycles_consumed = 5;
			break;

		case 0x4A:
			state->C = state->D;
			*cycles_consumed = 5;
			break;

		case 0x4B:
			state->C = state->E;
			*cycles_consumed = 5;
			break;

		case 0x4C:
			state->C = state->H;
			*cycles_consumed = 5;
			break;

		case 0x4D:
			state->C = state->L;
			*cycles_consumed = 5;
			break;

		case 0x4E:
			state->C = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x4F:
			state->C = state->A;
			*cycles_consumed = 5;
			break;


			/* D	*/

		case 0x50:
			state->D = state->B;
			*cycles_consumed = 5;
			break;

		case 0x51:
			state->D = state->C;
			*cycles_consumed = 5;
			break;

		case 0x52:
			state->D = state->D;
			*cycles_consumed = 5;
			break;

		case 0x53:
			state->D = state->E;
			*cycles_consumed = 5;
			break;

		case 0x54:
			state->D = state->H;
			*cycles_consumed = 5;
			break;

		case 0x55:
			state->D = state->L;
			*cycles_consumed = 5;
			break;

		case 0x56:
			state->D = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x57:
			state->D = state->A;
			*cycles_consumed = 5;
			break;

			/* E	*/


		case 0x58:
			state->E = state->B;
			*cycles_consumed = 5;
			break;

		case 0x59:
			state->E = state->C;
			*cycles_consumed = 5;
			break;

		case 0x5A:
			state->E = state->D;
			*cycles_consumed = 5;
			break;

		case 0x5B:
			state->E = state->E;
			*cycles_consumed = 5;
			break;

		case 0x5C:
			state->E = state->H;
			*cycles_consumed = 5;
			break;

		case 0x5D:
			state->E = state->L;
			*cycles_consumed = 5;
			break;

		case 0x5E:
			state->E = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x5F:
			state->E = state->A;
			*cycles_consumed = 5;
			break;


			/* H	*/


		case 0x60:
			state->H = state->B;
			*cycles_consumed = 5;
			break;

		case 0x61:
			state->H = state->C;
			*cycles_consumed = 5;
			break;

		case 0x62:
			state->H = state->D;
			*cycles_consumed = 5;
			break;

		case 0x63:
			state->H = state->E;
			*cycles_consumed = 5;
			break;

		case 0x64:
			state->H = state->H;
			*cycles_consumed = 5;
			break;

		case 0x65:
			state->H = state->L;
			*cycles_consumed = 5;
			break;

		case 0x66:
			state->H = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x67:
			state->H = state->A;
			*cycles_consumed = 5;
			break;



			/* L	*/

		case 0x68:
			state->L = state->B;
			*cycles_consumed = 5;
			break;

		case 0x69:
			state->L = state->C;
			*cycles_consumed = 5;
			break;

		case 0x6A:
			state->L = state->D;
			*cycles_consumed = 5;
			break;

		case 0x6B:
			state->L = state->E;
			*cycles_consumed = 5;
			break;

		case 0x6C:
			state->L = state->H;
			*cycles_consumed = 5;
			break;

		case 0x6D:
			state->L = state->L;
			*cycles_consumed = 5;
			break;

		case 0x6E:
			state->L = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x6F:
			state->L = state->A;
			*cycles_consumed = 5;
			break;

			/*	M	*/
		case 0x70:
			state->memory[state->H << 8 | state->L] = state->B;
			*cycles_consumed = 7;
			break;

		case 0x71:
			state->memory[state->H << 8 | state->L] = state->C;
			*cycles_consumed = 7;
			break;

		case 0x72:
			state->memory[state->H << 8 | state->L] = state->D;
			*cycles_consumed = 7;
			break;

		case 0x73:
			state->memory[state->H << 8 | state->L] = state->E;
			*cycles_consumed = 7;
			break;

		case 0x74:
			state->memory[state->H << 8 | state->L] = state->H;
			*cycles_consumed = 7;
			break;

		case 0x75:
			state->memory[state->H << 8 | state->L] = state->L;
			*cycles_consumed = 7;
			break;

		case 0x77:
			state->memory[state->H << 8 | state->L] = state->A;
			*cycles_consumed = 7;
			break;



			/* A	*/

		case 0x78:
			state->A = state->B;
			*cycles_consumed = 5;
			break;

		case 0x79:
			state->A = state->C;
			*cycles_consumed = 5;
			break;

		case 0x7A:
			state->A = state->D;
			*cycles_consumed = 5;
			break;

		case 0x7B:
			state->A = state->E;
			*cycles_consumed = 5;
			break;

		case 0x7C:
			state->A = state->H;
			*cycles_consumed = 5;
			break;

		case 0x7D:
			state->A = state->L;
			*cycles_consumed = 5;
			break;

		case 0x7E:
			state->A = state->memory[state->H << 8 | state->L];
			*cycles_consumed = 7;
			break;

		case 0x7F:
			state->A = state->A;
			*cycles_consumed = 5;
			break;



			/* INR increment register	*/
		case 0x04:
			state->B++;
			state->flag.ac = (state->B & 0xF) == 0;
			state->flag.z = state->B == 0;
			state->flag.s = (state->B & 0x80) != 0;
			state->flag.p = parity_check(state->B);
			*cycles_consumed = 5;
			break;

		case 0x0C:
			state->C++;
			state->flag.ac = (state->C & 0xF) == 0x0;
			state->flag.z = state->C == 0;
			state->flag.s = (state->C & 0x80) != 0;
			state->flag.p = parity_check(state->C);
			*cycles_consumed = 5;
			break;

		case 0x14:
			state->D++;
			state->flag.ac = (state->D & 0xF) == 0x0;
			state->flag.z = state->D == 0;
			state->flag.s = (state->D & 0x80) != 0;
			state->flag.p = parity_check(state->D);
			*cycles_consumed = 5;
			break;

		case 0x1C:
			state->E++;
			state->flag.ac = (state->E & 0xF) == 0x0;
			state->flag.z = state->E == 0;
			state->flag.s = (state->E & 0x80) != 0;
			state->flag.p = parity_check(state->E);
			*cycles_consumed = 5;
			break;

		case 0x24:
			state->H++;
			state->flag.ac = (state->H & 0xF) == 0x0;
			state->flag.z = state->H == 0;
			state->flag.s = (state->H & 0x80) != 0;
			state->flag.p = parity_check(state->H);
			*cycles_consumed = 5;
			break;

		case 0x2C:
			state->L++;
			state->flag.ac = (state->L & 0xF) == 0x0;
			state->flag.z = state->L == 0;
			state->flag.s = (state->L & 0x80) != 0;
			state->flag.p = parity_check(state->L);
			*cycles_consumed = 5;
			break;

		case 0x34:
			state->memory[state->H << 8 | state->L]++;
			state->flag.ac = (state->memory[state->H << 8 | state->L] & 0xF) == 0x0;
			state->flag.z = state->memory[state->H << 8 | state->L] == 0;
			state->flag.s = (state->memory[state->H << 8 | state->L] & 0x80) != 0;
			state->flag.p = parity_check(state->memory[state->H << 8 | state->L]);
			*cycles_consumed = 10;
			break;

		case 0x3C:
			state->A++;
			state->flag.ac = (state->A & 0xF) == 0x0;
			state->flag.z = state->A == 0;
			state->flag.s = (state->A & 0x80) != 0;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 5;
			break;



			/* INX	*/
		case 0x03: {
			uint16_t ans = (state->B << 8 | state->C);
			ans++;
			state->B = ans >> 8;
			state->C = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}

		case 0x13: {
			uint16_t ans = state->D << 8 | state->E;
			ans++;
			state->D = ans >> 8;
			state->E = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}
		case 0x23: {
			uint16_t ans = state->H << 8 | state->L;
			ans++;
			state->H = ans >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}
		case 0x33:
			state->sp++;
			*cycles_consumed = 5;
			break;

			/* DCR */
		case 0x05:
			state->B--;
			state->flag.ac = !((state->B & 0xF) == 0xF);
			state->flag.z = (state->B == 0);
			state->flag.s = (state->B >> 7);
			state->flag.p = parity_check(state->B);
			*cycles_consumed = 5;
			break;

		case 0x0D:
			state->C--;
			state->flag.ac = !((state->C & 0xF) == 0xF);
			state->flag.z = (state->C == 0);
			state->flag.s = (state->C >> 7);
			state->flag.p = parity_check(state->C);
			*cycles_consumed = 5;
			break;

		case 0x15:
			state->D--;
			state->flag.ac = !((state->D & 0xF) == 0xF);
			state->flag.z = (state->D == 0);
			state->flag.s = (state->D >> 7);
			state->flag.p = parity_check(state->D);
			*cycles_consumed = 5;
			break;

		case 0x1D:
			state->E--;
			state->flag.ac = !((state->E & 0xF) == 0xF);
			state->flag.z = (state->E == 0);
			state->flag.s = (state->E >> 7);
			state->flag.p = parity_check(state->E);
			*cycles_consumed = 5;
			break;


		case 0x25:
			state->H--;
			state->flag.ac = !((state->H & 0xF) == 0xF);
			state->flag.z = (state->H == 0);
			state->flag.s = (state->H >> 7);
			state->flag.p = parity_check(state->H);
			*cycles_consumed = 5;
			break;


		case 0x2D:
			state->L--;
			state->flag.ac = !((state->L & 0xF) == 0xF);
			state->flag.z = (state->L == 0);
			state->flag.s = (state->L >> 7);
			state->flag.p = parity_check(state->L);
			*cycles_consumed = 5;
			break;


		case 0x35:
			state->memory[state->H << 8 | state->L]--;
			state->flag.ac = !((state->memory[state->H << 8 | state->L] & 0xF) == 0xF);
			state->flag.z = (state->memory[state->H << 8 | state->L] == 0);
			state->flag.s = (state->memory[state->H << 8 | state->L] >> 7);
			state->flag.p = parity_check(state->memory[state->H << 8 | state->L]);
			*cycles_consumed = 10;
			break;
			
		case 0x3D:
			state->A--;
			state->flag.ac = !((state->A & 0xF) == 0xF);
			state->flag.z = (state->A == 0);
			state->flag.s = (state->A >> 7);
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 5;
			break;


		/* DCX	*/
		case 0x0B:{
			uint16_t ans = state->B << 8 | state->C;
			ans--;
			state->B = ans >> 8;
			state->C = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}

		case 0x1B:{
			uint16_t ans = state->D << 8 | state->E;
			ans--;
			state->D = ans >> 8;
			state->E = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}

		case 0x2B:{
			uint16_t ans = state->H << 8 | state->L;
			ans--;
			state->H = ans >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 5;
			break;
		}

		case 0x3B:
			state->sp--;
			*cycles_consumed = 5;
			break;

		
		/*	DAD instruction	*/

		case 0x09:{
			uint32_t ans = state->H << 8 | state->L;
			ans += (state->B << 8 | state->C);
			//if result is larger than 0xFFFF, state is set
			state->flag.c = (ans >> 16) & 1;
			state->H = (ans & 0xFF00) >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 10;
			break;
		}

		case 0x19:{
			uint32_t ans = state->H << 8 | state->L;
			ans += state->D << 8 | state->E;
			//if result is larger than 16 bits, state is set
			state->flag.c = (ans >> 16) & 1;
			state->H = (ans & 0xFF00) >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 10;
			break;
		}

		case 0x29:{
			uint32_t ans = (state->H << 8 | state->L);
			ans += ans;
			//if result is larger than 16 bits, state is set
			state->flag.c = (ans >> 16) & 1;
			state->H = (ans & 0xFF00) >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 10;
			break;
		}

		case 0x39:{
			uint32_t ans = state->H << 8 | state->L;
			ans += state->sp;
			state->flag.c = (ans >> 16) & 1;
			state->H = (ans & 0xFF00) >> 8;
			state->L = ans & 0xFF;
			*cycles_consumed = 10;
			break;
		}
		/* ROTATE ACCUMULATOR	*/

		/* RLC	*/
		case 0x07:
			//carry bit is equal to the high-order bit of the accumulator
			state->flag.c = state->A >> 7;
			state->A = (state->A << 1) | state->flag.c;
			*cycles_consumed = 4;
			break;

		/* RRC	*/
		case 0x0F: 
			state->flag.c = state->A & 0x01; //LSB 
			state->A = (state->A >> 1) | (state->flag.c << 7);
			*cycles_consumed = 4;
			break;
			
		/* RAL	*/
		case 0x17:{
			//send carry to LSB while sending MSB to the carry
			uint8_t carry = state->flag.c;
			state->flag.c = state->A >> 7;
			state->A = (state->A << 1) | (carry & 0x01);
			*cycles_consumed = 4;
			break;
		}

		/* RAR*/
		case 0x1F:{
			uint8_t carry = state->flag.c;
			state->flag.c = state->A & 0x01;
			state->A = (state->A >> 1) | (carry << 7);
			*cycles_consumed = 4;
			break;
		}
		

		/* SHLD	*/
		case 0x22:
			state->memory[opcode[2] << 8 | opcode[1]] = state->L;
			state->memory[((opcode[2] << 8 | opcode[1]) + 1) & 0xFFFF] = state->H;
			bytes = 3;
			*cycles_consumed = 16;
			break;

		
		/* LHLD	*/	
		case 0x2A:
			state->L = state->memory[opcode[2] << 8 | opcode[1]];
			state->H = state->memory[((opcode[2] << 8 | opcode[1]) + 1) & 0xFFFF];
			bytes = 3;
			*cycles_consumed = 16;
			break;


		/* STA	*/
		case 0x32:
			state->memory[opcode[2] << 8 | opcode[1]] = state->A;
			bytes = 3;
			*cycles_consumed = 13;
			break;
	
		/* LDA	*/
		case 0x3A:
			state->A = state->memory[opcode[2] << 8 | opcode[1]];
			bytes = 3;
			*cycles_consumed = 13;
			break;

		/* LDAX	*/
		case 0x0A:
			state->A = state->memory[state->B << 8 | state->C];
			*cycles_consumed = 7;
			break;
		case 0x1A:
			state->A = state->memory[state->D << 8 | state->E];		
			*cycles_consumed = 7;
			break;



		/* STAX	*/

		case 0x02:
			state->memory[state->B << 8 | state->C] = state->A;
			*cycles_consumed = 7;
			break;
		
		case 0x12:
			state->memory[state->D << 8 | state->E] = state->A;
			*cycles_consumed = 7;
			break;



		/* JUMPS	*/
		/* PCHL	*/
		case 0xE9:
			state->pc = state->H << 8 | state->L;
			bytes = 0;
			*cycles_consumed = 10;
			break;

		/* JMP	*/
		case 0xC3:
			if (((opcode[2] << 8 | opcode[1]) == 0) && (!game)) {
				printf("%d ", state->instructions_ran);
				printf("JUMP TO 0");
				exit(0);
			}
			state->pc = opcode[2] << 8 | opcode[1];
			bytes = 0;
			*cycles_consumed = 10;
			break;
	

		/* JNZ	*/
		case 0xC2:
			if (!state->flag.z) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else
				bytes = 3;
			*cycles_consumed = 10;
			break;

		/* JZ	*/
		case 0xCA:
			if (state->flag.z) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;

		/* JNC	*/
		case 0xD2:
			if (!state->flag.c) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;
		
		/*	JC	*/
		case 0xDA:

			if (state->flag.c) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;

		/* JPO	*/
		case 0xE2:
			if (!state->flag.p) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;
				
		
		/* JPE	*/
		case 0xEA:
			if (state->flag.p) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;
				
		
		/* JP	*/
		case 0xF2:
			if (!state->flag.s) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;
	

		/* JM	*/
		case 0xFA:
			if (state->flag.s) {
				state->pc = opcode[2] << 8 | opcode[1];
				bytes = 0;
			}
			else bytes = 3;
			*cycles_consumed = 10;
			break;

		/* CALL instructions	*/


		/* CALL	*/
			//return address must be stored in the stack, through sp.
			//sp points to the currently last addr in the stack...


			/*
			
				BDOS CALLING CONVENTION in CP/M OS - 
				
				For string printing - 
				C is loaded with some value, which represents some function. 9 represents string printing.
				Which string? its the parameter loaded in the DE register pair before the CALL to 5. its mnemonic is bdos.
			
				ig the others dont matter. 
				In the exer8080, all the strings are ending with a $ sign. So print until $ encountered.
	
				EZ
			*/
		case 0xCD:{
			int val = ((opcode[2] << 8) | opcode[1]);
            if ((5 == val) && (!game)){
				bytes = 3;
                if (state->C == 9){
					cpm_call = true;
                    uint16_t offset = (state->D<<8) | (state->E);    
                    const char *str = &state->memory[offset];  //skip the prefix bytes    
					log_cpm(str);
                }    
				else if (state->C == 2) {
					cpm_call = true;
					log_emulator_status_ch(state->E);
				}
            }    
			{
				uint16_t ret = state->pc + 3; //after return, this address will be loaded in pc.
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->pc = (opcode[2] << 8 | opcode[1]);
				state->sp -= 2;
				bytes = 0;
			}
			*cycles_consumed = 17;
			break;
		}
			/* stack grows from bottom to top. so higher address to lower address*/
 
		/* CNZ	*/
		case 0xC4: {
			if (!state->flag.z) {
				uint16_t ret = state->pc + 3;
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->pc = (opcode[2] << 8 | opcode[1]);
				state->sp -= 2;
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				*cycles_consumed = 11;
				bytes = 3;
			}
			break;
		}

		/* CZ	*/
		case 0xCC: 
			if(state->flag.z){
				uint16_t ret = state->pc + 3; 
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->pc = (opcode[2] << 8 | opcode[1]);		
				state->sp -= 2;
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				bytes = 3;
				*cycles_consumed = 11;
			}
			break;
		
		/* CNC	*/
		case 0xD4:
			if (!state->flag.c) {
				uint16_t ret = state->pc + 3; 
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->pc = (opcode[2] << 8 | opcode[1]);
				state->sp -= 2;
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				bytes = 3;
				*cycles_consumed = 11;
			}
			break;
		
		/*	CC	*/
		case 0xDC:
			if (state->flag.c) {
				uint16_t ret = state->pc + 3;
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->pc = (opcode[2] << 8 | opcode[1]);
				state->sp -= 2;
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				bytes = 3;
				*cycles_consumed = 11;
			}
			break;

		/*	CPO	*/
		case 0xE4:
			if(!state->flag.p){
				uint16_t ret = state->pc + 3;
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->sp -= 2;
				state->pc = (opcode[2] << 8 | opcode[1]);		
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				bytes = 3;
				*cycles_consumed = 11;
			}
			break;
		
		/* CPE	*/
		case 0xEC:
			
			if(state->flag.p){
				uint16_t ret = state->pc + 3; 
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->sp -= 2;
				state->pc = (opcode[2] << 8 | opcode[1]);		
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				*cycles_consumed = 11;
				bytes = 3;
			}
			break;

		/* CP	*/
		case 0xF4:
			if(!state->flag.s){
				uint16_t ret = state->pc + 3; 
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->sp -= 2;
				state->pc = (opcode[2] << 8 | opcode[1]);		
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				bytes = 3;
				*cycles_consumed = 11;
			}
			break;


		/* CM	*/
		case 0xFC:
	
			if(state->flag.s){
				uint16_t ret = state->pc + 3; 
				state->memory[(state->sp - 1) & 0xFFFF] = ret >> 8;
				state->memory[(state->sp - 2) & 0xFFFF] = ret & 0xFF;
				state->sp -= 2;
				state->pc = (opcode[2] << 8 | opcode[1]);		
				*cycles_consumed = 17;
				bytes = 0;
			}
			else {
				*cycles_consumed = 11;
				bytes = 3;
			}
			break;


		/* RET instructions	*/
		
		/* RET	*/
		case 0xC9:
			//pc must be loaded with the address in the sp
			state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
			state->sp += 2;
			bytes = 0;
			*cycles_consumed = 10;
			//idk why that took so much brain power...  break; 
			break;
		
		/* RC	*/
		case 0xD8:
			if(state->flag.c){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;

		/* RNC	*/
		case 0xD0:
			if(!state->flag.c){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;

		/* RZ	*/
		case 0xC8:
			if(state->flag.z){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;
		/* RNZ	*/
		case 0xC0:
			if(!state->flag.z){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;

		/* RM	*/
		case 0xF8:
			if(state->flag.s){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;

		/* RP	*/	
		case 0xF0:
			if(!state->flag.s){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;

		/* RPE	*/
		case 0xE8:
			if(state->flag.p){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;
	
		/* RPO	*/
		case 0xE0:
			if(!state->flag.p){
				state->pc = (state->memory[(state->sp + 1) & 0xFFFF] << 8) | state->memory[state->sp];
				state->sp += 2;
				bytes = 0;
				*cycles_consumed = 11;
			}
			else
				*cycles_consumed = 5;
			break;



		// ACC instructions - 2 bytes	

		// ADI
		case 0xC6:{
			uint16_t ans = (uint16_t)state->A + (uint16_t)opcode[1];
			state->flag.c = ((ans ^ state->A ^ opcode[1]) >> 8) & 1;
			state->flag.s = (ans & 0x80) >> 7;
			state->flag.z = (ans & 0xff) == 0;
			state->flag.p = parity_check(ans & 0xff);
			state->flag.ac = ((ans ^ state->A ^ opcode[1]) >> 4) & 1;
			state->A = ans & 0xff;
			bytes = 2;
			*cycles_consumed = 7;
			break;
		}


		//ACI
		case 0xCE:{
			uint16_t ans = (uint16_t)state->A + (uint16_t)opcode[1] + state->flag.c;
			state->flag.c = ((ans ^ state->A ^ opcode[1]) >> 8) & 1;
			state->flag.s = (ans & 0x80) >> 7;
			state->flag.z = (ans & 0xff) == 0;
			state->flag.p = parity_check(ans & 0xff);
			state->flag.ac = ((ans ^ state->A ^ opcode[1]) >> 4) & 1;
			state->A = ans & 0xff;
			bytes = 2;
			*cycles_consumed = 7;
			break;
		}

		// SUI
		case 0xD6:{
			uint8_t result = state->A + ((~(opcode[1])) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(opcode[1])) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(opcode[1])) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(opcode[1])) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			bytes = 2;
			*cycles_consumed = 7;
			break;
		}
		// SBI
		case 0xDE:{
			uint8_t result = state->A + ((~(opcode[1])) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(opcode[1])) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(opcode[1])) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(opcode[1])) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			bytes = 2;
			*cycles_consumed = 7;
			break;
		}

		//ANI
		case 0xE6:
			state->flag.ac = ((state->A | opcode[1]) & 0x08) != 0;
			state->A = state->A & opcode[1];
			state->flag.c = 0;
			state->flag.z = state->A == 0;
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			bytes = 2;
			*cycles_consumed = 7;
			break;
		

		//XRI
		case 0xEE:
			state->A = state->A ^ opcode[1];
			state->flag.c = 0;
			state->flag.z = state->A == 0;
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			bytes = 2;
			*cycles_consumed = 7;
			break;

		//ORI
		case 0xF6:
			state->A = state->A | opcode[1];
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = state->A == 0;
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			bytes = 2;
			*cycles_consumed = 7;
			break;

		//CPI
		case 0xFE:{
			uint16_t result = state->A - opcode[1];
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ opcode[1]) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			bytes = 2;
			*cycles_consumed = 7;
			break;
		}
		//STACK PUSHHHHH
		
		//PUSH B
		case 0xC5:
			state->memory[(state->sp - 1) & 0xFFFF] = state->B;
			state->memory[(state->sp - 2) & 0xFFFF] = state->C;
			state->sp -= 2;
			*cycles_consumed = 11;
			break;
		

		//PUSH D
		case  0xD5:
			state->memory[(state->sp - 1) & 0xFFFF] = state->D;
			state->memory[(state->sp - 2) & 0xFFFF] = state->E;
			state->sp -= 2;
			*cycles_consumed = 11;
			break;
		

		//PUSH H
		case 0xE5: {
			state->memory[(state->sp - 1) & 0xFFFF] = state->H;
			state->memory[(state->sp - 2) & 0xFFFF] = state->L;
			state->sp -= 2;
			*cycles_consumed = 11;
			break;
		}

		//PSW
		case 0xF5: {
			state->memory[(state->sp - 1) & 0xFFFF] = state->A;
			uint8_t bits = 0x00;
			bits |= (state->flag.s << 7);
			bits |= (state->flag.z << 6);
			bits |= (state->flag.ac << 4);
			bits |= (state->flag.p << 2);
			bits |= (1 << 1);
			bits |= (state->flag.c);
			state->memory[(state->sp - 2) & 0xFFFF] = bits;
			state->sp -= 2;
			*cycles_consumed = 11;
			break;
		}

		//STACK POPPPP
			

		case 0xC1:
			state->C = state->memory[state->sp];
			state->B = state->memory[(state->sp + 1) & 0xFFFF];
			state->sp += 2;
			*cycles_consumed = 10;
			break;

		case 0xD1:
			state->E = state->memory[state->sp];
			state->D = state->memory[(state->sp + 1) & 0xFFFF];
			state->sp += 2;
			*cycles_consumed = 10;
			break;

		case 0xE1:
			state->L = state->memory[state->sp];
			state->H = state->memory[(state->sp + 1) & 0xFFFF];
			state->sp += 2;
			*cycles_consumed = 10;
			break;
			
		case 0xF1:{
			uint8_t bits = state->memory[state->sp];
			state->A = state->memory[(state->sp + 1) & 0xFFFF];
			state->sp += 2;
			//s | z | _ | ac | _ | p | 1 | c
			state->flag.s = bits >> 7;
			state->flag.z = (bits & 0x40) >> 6;
			state->flag.p = (bits & 0x04) >> 2;
			state->flag.c = (bits & 0x01);
			state->flag.ac = (bits & 0x10) >> 4;
			*cycles_consumed = 10;
			break;
		}

		//XCHG - exchange HL and DE register pairs	
		case 0xEB:{
			uint16_t temp = state->H << 8 | state->L;
			state->H = state->D;
			state->L = state->E;
			state->D = temp >> 8;
			state->E = temp & 0xFF;
			*cycles_consumed = 4;
			break;
		}
		
		//XTHL - 
		case 0xE3:{
			uint8_t bit = state->L;
			state->L = state->memory[state->sp];
			state->memory[state->sp] = bit;
			bit = state->H;
			state->H = state->memory[(state->sp + 1) & 0xFFFF];
			state->memory[(state->sp + 1) & 0xFFFF] = bit;
			*cycles_consumed = 18;
			break;
		}
		
		//SPHL
		case 0xF9:
			//state->memory[state->sp] = state->L;
			//state->memory[(state->sp + 1) & 0xFFFF] = state->H;
			state->sp = state->H << 8 | state->L;
			*cycles_consumed = 7;
			break;
		
		
		

		//Accumulator instructions -

		//ADD B, C, D, E, H, L, M, A
		case 0x80:{
			uint16_t res = (uint16_t) state->B;
			res += state->A;
			state->flag.c = ((state->A ^ state->B ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->B ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}

		case 0x81:{
			uint16_t res = (uint16_t) state->C;
			res += state->A;
			state->flag.c = ((state->A ^ state->C ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->C ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x82:{
			uint16_t res = (uint16_t) state->D;
			res += state->A;
			state->flag.c = ((state->A ^ state->D ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->D ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}

		case 0x83:{
			uint16_t res = (uint16_t) state->E;
			res += state->A;
			state->flag.c = ((state->A ^ state->E ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->E ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}		

		case 0x84:{
			uint16_t res = (uint16_t) state->H;
			res += state->A;
			state->flag.c = ((state->A ^ state->H ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->H ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x85:{
			uint16_t res = (uint16_t) state->L;
			res += state->A;
			state->flag.c = ((state->A ^ state->L ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->L ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}			

		case 0x86:{
			uint16_t res = (uint16_t) (state->memory[(state->H << 8 | state->L)]);
			res += state->A;
			state->flag.c = ((state->A ^ state->memory[state->H << 8 | state->L] ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->memory[state->H << 8 | state->L] ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 7;
			break;
		}
		case 0x87:{
			uint16_t res = (uint16_t) state->A;
			res += state->A;
			state->flag.c = ((state->A ^ state->A ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->A ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}

		//ADC B, C, D, E, H, L, M, A
		case 0x88:{
			uint16_t res = (uint16_t) state->B;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->B ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->B ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}

		case 0x89:{
			uint16_t res = (uint16_t) state->C;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->C ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->C ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x8A:{
			uint16_t res = (uint16_t) state->D;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->D ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->D ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x8B:{
			uint16_t res = (uint16_t) state->E;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->E ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->E ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x8C:{
			uint16_t res = (uint16_t) state->H;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->H ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->H ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
	
		case 0x8D:{
			uint16_t res = (uint16_t) state->L;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->L ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->L ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
			
		case 0x8E:{
			uint16_t res = (uint16_t) (state->memory[(state->H << 8 | state->L)]);
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->memory[state->H << 8 | state->L] ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->memory[state->H << 8 | state->L] ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 7;
			break;
		}
			
		case 0x8F:{
			uint16_t res = (uint16_t) state->A;
			res += state->A;
			res += state->flag.c;
			state->flag.c = ((state->A ^ state->A ^ res) >> 8) & 1;
			state->flag.s = (res & 0x80) >> 7;
			state->flag.z = ((res & 0xFF) == 0);
			state->flag.p = parity_check(res & 0xFF);
			state->flag.ac = ((state->A ^ state->A ^ res) >> 4) & 1;
			state->A = res & 0xFF;
			*cycles_consumed = 4;
			break;
		}
	
				
		// SUB instructions - SUB B, C, D, E, H, L, M, A
		case 0x90:{
			uint8_t result = state->A + ((~(state->B)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->B)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->B)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->B)) & 0xFF) ^ res) >> 8) & 1; 
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}
					
		case 0x91:{
			uint8_t result = state->A + ((~(state->C)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->C)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->C)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->C)) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}
		case 0x92:{
			uint8_t result = state->A + ((~(state->D)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->D)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->D)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->D)) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}
		case 0x93:{
			uint8_t result = state->A + ((~(state->E)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->E)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->E)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->E)) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x94:{
			uint8_t result = state->A + ((~(state->H)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->H)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->H)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->H)) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x95:{
			uint8_t result = state->A + ((~(state->L)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->L)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->L)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->L)) & 0xFF) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x96:{
			uint8_t result = state->A + ((~(state->memory[state->H << 8 | state->L])) & 0xFF) + 1;
			{
				uint16_t res = state->A + ~(state->memory[state->H << 8 | state->L]) + 1;
				state->flag.ac = ((state->A ^ ~(state->memory[state->H << 8 | state->L]) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ~(state->memory[state->H << 8 | state->L]) ^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x97:{
			uint8_t result = state->A + ((~(state->A)) & 0xFF) + 1;
			{
				uint16_t res = state->A + ((~(state->A)) & 0xFF) + 1;
				state->flag.ac = ((state->A ^ ((~(state->A)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->A)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}


		//SBB
		case 0x98:{
			uint8_t result = state->A + ((~(state->B)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->B)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->B)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->B)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x99:{
			uint8_t result = state->A + ((~(state->C)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->C)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->C)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->C)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x9A:{
			uint8_t result = state->A + ((~(state->D)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->D)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->D)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->D)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x9B:{
			uint8_t result = state->A + ((~(state->E)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->E)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->E)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->E)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x9C:{
			uint8_t result = state->A + ((~(state->H)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->H)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->H)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->H)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}
		case 0x9D:{
			uint8_t result = state->A + ((~(state->L)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->L)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->L)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->L)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x9E:{
			uint8_t result = state->A + ((~(state->memory[state->H << 8 | state->L])) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->memory[state->H << 8 | state->L])) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->memory[state->H << 8 | state->L])) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->memory[state->H << 8 | state->L])) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		case 0x9F:{
			uint8_t result = state->A + ((~(state->A)) & 0xFF) + !(state->flag.c);
			{
				uint16_t res = state->A + ((~(state->A)) & 0xFF) + !(state->flag.c);
				state->flag.ac = ((state->A ^ ((~(state->A)) & 0xFF) ^ res) >> 4) & 1;
				state->flag.c = ((state->A ^ ((~(state->A)) & 0xFF)^ res) >> 8) & 1;
				state->flag.c = !state->flag.c;
			}
			state->A = result;
			state->flag.z = (result == 0);
			state->flag.s = (result & 0x80) != 0;
			state->flag.p = parity_check(result);
			*cycles_consumed = 4;
			break;		
		}

		//ANA - and with accumulator
		case 0xA0:
			state->flag.ac = ((state->A | state->B) & 0x08) != 0;
			state->A = state->A & state->B;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;


		case 0xA1:
			state->flag.ac = ((state->A | state->C) & 0x08) != 0;
			state->A = state->A & state->C;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA2:
			state->flag.ac = ((state->A | state->D) & 0x08) != 0;
			state->A = state->A & state->D;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA3:
			state->flag.ac = ((state->A | state->E) & 0x08) != 0;
			state->A = state->A & state->E;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA4:
			state->flag.ac = ((state->A | state->H) & 0x08) != 0;
			state->A = state->A & state->H;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA5:
			state->flag.ac = ((state->A | state->L) & 0x08) != 0;
			state->A = state->A & state->L;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA6:
			state->flag.ac = ((state->A | state->memory[state->H << 8 | state->L]) & 0x08) != 0;
			state->A = state->A & (state->memory[state->H << 8 | state->L]);
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 7;
			break;

		case 0xA7:
			state->flag.ac = ((state->A | state->A) & 0x08) != 0;
			state->A = state->A & state->A;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;


		//XRA
		case 0xA8:
			state->A = state->A ^ state->B;
			state->flag.ac = 0;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xA9:
			state->A = state->A ^ state->C;
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xAA:
			state->A = state->A ^ state->D;
			state->flag.ac = 0;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xAB:
			state->A = state->A ^ state->E;
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xAC:
			state->A = state->A ^ state->H;
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xAD:
			state->A = state->A ^ state->L;
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;

		case 0xAE:
			state->A = state->A ^ state->memory[state->H << 8 | state->L];
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 7;
			break;

		case 0xAF:
			state->A = state->A ^ state->A;
			state->flag.c = 0;
			state->flag.ac = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			*cycles_consumed = 4;
			break;
		
		//ORA
		case 0xB0:
			state->A = state->A | state->B;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB1:
			state->A = state->A | state->C;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB2:
			state->A = state->A | state->D;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB3:
			state->A = state->A | state->E;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB4:
			state->A = state->A | state->H;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB5:
			state->A = state->A | state->L;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		case 0xB6:
			state->A = state->A | state->memory[state->H << 8 | state->L];
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 7;
			break;
		
		case 0xB7:
			state->A = state->A | state->A;
			state->flag.c = 0;
			state->flag.z = (state->A == 0);
			state->flag.s = state->A >> 7;
			state->flag.p = parity_check(state->A);
			state->flag.ac = 0;
			*cycles_consumed = 4;
			break;
		
		//CMP - 
		case 0xB8:{
			uint16_t result = state->A - state->B;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->B) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}

		case 0xB9:{
			uint16_t result = state->A - state->C;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->C) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}		
		case 0xBA:{
			uint16_t result = state->A - state->D;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->D) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}
						
		case 0xBB:{
			uint16_t result = state->A - state->E;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->E) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}
		case 0xBC:{
			uint16_t result = state->A - state->H;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->H) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}
						
		case 0xBD:{
			uint16_t result = state->A - state->L;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->L) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}		
		case 0xBE:{
			uint16_t result = state->A - state->memory[state->H << 8 | state->L];
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->memory[state->H << 8 | state->L]) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}		
		case 0xBF:{
			uint16_t result = state->A - state->A;
			state->flag.c = (result >> 8) & 1;
			state->flag.ac = (~(result ^ state->A ^ state->A) & 0x10) >> 4;
			state->flag.s = (result & 0x80) != 0;
			state->flag.z = ((result & 0xFF) == 0);
			state->flag.p = parity_check(result & 0xFF);
			*cycles_consumed = 4;
			break;
		}

		//honorable mentions ...

		//DAA
		case 0x27:{

			bool carry = state->flag.c;
			uint8_t perform_add = 0;
			uint8_t higher = state->A >> 4;
			uint8_t lower = state->A & 0x0F;


			if(lower > 9 || state->flag.ac)
				perform_add += 0x6;
			if (higher > 9 || (higher >= 9 && lower > 9) || state->flag.c) {
				perform_add += 0x60;
				carry = 1;
			}

			//perform an addition
			{
				uint8_t result = state->A + perform_add;
				state->flag.s = result >> 7;
				state->flag.z = result == 0;
				state->flag.p = parity_check(result);
				{//set the c and ac flags.
					uint16_t res = state->A + perform_add;
					uint16_t cy = res ^ state->A ^ perform_add;
					state->flag.c = (cy >> 8) & 1;
					state->flag.ac = (cy >> 4) & 1;
				}
				state->A = result;
			}
			state->flag.c = carry;
			*cycles_consumed = 4;
			break;
		}

		//CMA
		case 0x2F:
			state->A = ~state->A;
			*cycles_consumed = 4;
			break;

		//STC
		case 0x37:
			state->flag.c = 1;
			*cycles_consumed = 4;
			break;
		
		//CMC
	case 0x3F:
			state->flag.c = !state->flag.c;
			*cycles_consumed = 4;
			break;

		
		//EI
		case 0xFB:
			state->interrupt = true;	
			*cycles_consumed = 4;
			break;	
			
		//DI
		case 0xF3:
			state->interrupt = false;
			*cycles_consumed = 4;
			break;

		//1) push the current program counter on the stack.
		case 0xC7:
		case 0xCF:
		case 0xD7:
		case 0xDF:
		case 0xE7:
		case 0xEF:
		case 0xF7:
		case 0xFF:
			break;

		//HLT
		case 0x76:
			*cycles_consumed = 7;
			puts("HALT INSTRUCTION");
			//TODO: WAITS FOR A DAMN INTERRUPT
			bytes = 1;
			break;

		default: {
			debug_print("UNIMPLEMENTED INSTRUCTION!", ERROR);
			printf("instruction opcode - %02X", *opcode);
		}
	}
	state->cycles_consumed += *cycles_consumed;//cycles are updated
	state->instructions_ran++;//so are the total number of instructions ran
	state->pc += bytes;//total instructions to be run are also updated

}
