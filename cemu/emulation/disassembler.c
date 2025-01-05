#include<stdio.h>
#include<stdint.h>
int disassemble_8080(unsigned char* buffer, uint16_t pc, char* output){

	unsigned char* opcode = buffer + pc;
	
	//*opcode -= 48;//ascii character to an actual number, so we offset by 48, the buffer is in a text file.
	int bytes = 1;//assume it is a 1 byte instruction, no args.

	sprintf(output, "%04X ", pc);
	output += 5;

	switch(*opcode){


		/*control instructions */
		case 0x00: sprintf(output, "NOP"); break;
		case 0x08: sprintf(output, "NOP"); break;
		

		/* interrupt flip-flop instructions */

		/* opcode - 1111 | _ | 011
				   x ->	1 for EI
					0 for DI

			*/

		case 0xFB: sprintf(output, "EI"); break;//enable interrupt - enables cpu to respond to interrupts
		case 0xF3: sprintf(output, "DI"); break;//disable interrupt




		/* Input-Output instructions (2 byte instructions)	*/
	
		/* opcode - 	1101 | _ | 011	|	exp (8 bit)
				       x ->	1 - IN
						2 - OUT

			*/


		/*	IN - a byte is read from input device exp and replaces the contents of the accumulator*/
		/*	OUT - the accumulator value is sent to the output device exp	*/


		case 0xDB: sprintf(output, "IN $%02X", opcode[1]); bytes = 2; break;
		case 0xD3: sprintf(output, "OUT $%02X", opcode[1]); bytes = 2; break;





		/* LXI instructions - (3 byte instructions)*/
		/*	The third byte is loaded into the first register in the specified pair, 
			and the second byte is loaded into the second register in the specified pair	*/
		/* opcodes - 

			00 | __ | 0001 	
		 	     rp ->
				00 - BC
				01 - DE
				10 - HL
				11 - SP

		*/
		case 0x01: sprintf(output, "LXI	B, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x11: sprintf(output, "LXI	D, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x21: sprintf(output, "LXI H, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x31: sprintf(output, "LXI SP, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;



		/* MVI instructions - (2 byte instructions)*/
		/*	The byte of immediate data is stored in the specified register or memory byte	*/
		/* opcode - 
			00 | ___ | 110
			     reg ->
				000 - B
				001 - C
				010 - D
				011 - E
				100 - H
				101 - L
				110 - M
				111 - A

		*/
		case 0x06: sprintf(output, "MVI B, $%02X", opcode[1]); bytes = 2; break;
		case 0x0E: sprintf(output, "MVI	C, $%02X", opcode[1]); bytes = 2; break;
		case 0x16: sprintf(output, "MVI	D, $%02X", opcode[1]); bytes = 2; break;
		case 0x1E: sprintf(output, "MVI E, $%02X", opcode[1]); bytes = 2; break;
		case 0x26: sprintf(output, "MVI	H, $%02X", opcode[1]); bytes = 2; break;
		case 0x2E: sprintf(output, "MVI L, $%02X", opcode[1]); bytes = 2; break;
		case 0x36: sprintf(output, "MVI M, $%02X", opcode[1]); bytes = 2; break;
		case 0x3E: sprintf(output, "MVI A, $%02X", opcode[1]); bytes = 2; break;
	

			
		/* MOV instructions */


		/* opcode - 01 ___  | ___
			      dest    src*/

		/*
		dest/src - 	000 - B
				001 - C
				010 - D
				011 - E
				100 - H
				101 - L
				110 - M (mem)
				111 - A
		*/
		


		/*	MOV B <- src		*/
		case 0x40: sprintf(output, "MOV B, B"); break; //useless i guess...
		case 0x41: sprintf(output, "MOV B, C"); break;
		case 0x42: sprintf(output, "MOV B, D"); break;
		case 0x43: sprintf(output, "MOV B, E"); break;
		case 0x44: sprintf(output, "MOV B, H"); break;
		case 0x45: sprintf(output, "MOV B, L"); break;
		case 0x46: sprintf(output, "MOV B, M"); break;
		case 0x47: sprintf(output, "MOV B, A"); break;


		/*	MOV C <- src	*/
		case 0x48: sprintf(output, "MOV C, B"); break;
		case 0x49: sprintf(output, "MOV C, C"); break;
		case 0x4A: sprintf(output, "MOV C, D"); break;
		case 0x4B: sprintf(output, "MOV C, E"); break;
		case 0x4C: sprintf(output, "MOV C, H"); break;
		case 0x4D: sprintf(output, "MOV C, L"); break;
		case 0x4E: sprintf(output, "MOV C, M"); break;
		case 0x4F: sprintf(output, "MOV C, A"); break;

		/* 	MOV D <- src */
		case 0x50: sprintf(output, "MOV D, B"); break;
		case 0x51: sprintf(output, "MOV D, C"); break;
		case 0x52: sprintf(output, "MOV D, D"); break;
		case 0x53: sprintf(output, "MOV D, E"); break;
		case 0x54: sprintf(output, "MOV D, H"); break;
		case 0x55: sprintf(output, "MOV D, L"); break;
		case 0x56: sprintf(output, "MOV D, M"); break;
		case 0x57: sprintf(output, "MOV D, A"); break;



		/*	MOV E <- src	*/
		case 0x58: sprintf(output, "MOV E, B"); break;
		case 0x59: sprintf(output, "MOV E, C"); break;
		case 0x5A: sprintf(output, "MOV E, D"); break;
		case 0x5B: sprintf(output, "MOV E, E"); break;
		case 0x5C: sprintf(output, "MOV E, H"); break;
		case 0x5D: sprintf(output, "MOV E, L"); break;
		case 0x5E: sprintf(output, "MOV E, M"); break;
		case 0x5F: sprintf(output, "MOV E, A"); break;

	
		/*	MOV H <- src 	*/
		case 0x60: sprintf(output, "MOV H, B"); break;
		case 0x61: sprintf(output, "MOV H, C"); break;
		case 0x62: sprintf(output, "MOV H, D"); break;
		case 0x63: sprintf(output, "MOV H, E"); break;
		case 0x64: sprintf(output, "MOV H, H"); break;
		case 0x65: sprintf(output, "MOV H, L"); break;
		case 0x66: sprintf(output, "MOV H, M"); break;
		case 0x67: sprintf(output, "MOV H, A"); break;

		/*	MOV L <- src	*/
		case 0x68: sprintf(output, "MOV L, B"); break;
		case 0x69: sprintf(output, "MOV L, C"); break;
		case 0x6A: sprintf(output, "MOV L, D"); break;
		case 0x6B: sprintf(output, "MOV L, E"); break;
		case 0x6C: sprintf(output, "MOV L, H"); break;
		case 0x6D: sprintf(output, "MOV L, L"); break;
		case 0x6E: sprintf(output, "MOV L, M"); break;
		case 0x6F: sprintf(output, "MOV L, A"); break;

		/*	MOV M <- src	*/
		case 0x70: sprintf(output, "MOV M, B"); break;
		case 0x71: sprintf(output, "MOV M, C"); break;
		case 0x72: sprintf(output, "MOV M, D"); break;
		case 0x73: sprintf(output, "MOV M, E"); break;
		case 0x74: sprintf(output, "MOV M, H"); break;
		case 0x75: sprintf(output, "MOV M, L"); break;
		case 0x76: sprintf(output, "HLT"); break; //src and dest cant be M and M
		case 0x77: sprintf(output, "MOV M, A"); break;


		/*	MOV A <- src	*/
		case 0x78: sprintf(output, "MOV A, B"); break;
		case 0x79: sprintf(output, "MOV A, C"); break;
		case 0x7A: sprintf(output, "MOV A, D"); break;
		case 0x7B: sprintf(output, "MOV A, E"); break;
		case 0x7C: sprintf(output, "MOV A, H"); break;
		case 0x7D: sprintf(output, "MOV A, L"); break;
		case 0x7E: sprintf(output, "MOV A, M"); break;
		case 0x7F: sprintf(output, "MOV A, A"); break; //god i make so many typoes
		




		// increment instructions

		/* INR instructions (1 byte instructions) */

		/*	Increments a single memory byte or register by 1*/
		
		/* opcode - 

			00 | ___ | 100
			     reg -> 	000 - B
					001 - C
					010 - D
					011 - E
					100 - H
					101 - L
					110 - M
					111 - A
			
			*/
		
		case 0x04: sprintf(output, "INR, B"); break;
		case 0x0C: sprintf(output, "INR, C"); break;
		case 0x14: sprintf(output, "INR, D"); break;
		case 0x1C: sprintf(output, "INR, E"); break;
		case 0x24: sprintf(output, "INR, H"); break;
		case 0x2C: sprintf(output, "INR, L"); break;
		case 0x34: sprintf(output, "INR, M"); break; 	
		case 0x3C: sprintf(output, "INR, A"); break;
	

		/* INX instructions (1 byte instructions) */

		/* Increments the value in the register pair by 1*/

		/* opcode - 
		
			00 | __ | 0011
			     rp ->
				00 - BC
				01 - DE
				10 - HL
				11 - SP

			*/

		case 0x03: sprintf(output, "INX, B"); break;
		case 0x13: sprintf(output, "INX, D"); break;
		case 0x23: sprintf(output, "INX, H"); break;
		case 0x33: sprintf(output, "INX, SP"); break;


		
	
		//decrement instructions

		/* DCR Decrement isntructions - (1 byte instructions) */

		/* Decrement the specified single memory byte or register by 1 */
	
		/* opcode -

			00 | ___ | 101
			     reg ->	000 - B
					001 - C
					010 - D
					011 - E
					100 - H
					101 - L
					110 - M
					111 - A

			*/
				
		case 0x05: sprintf(output, "DCR, B"); break;
		case 0x0D: sprintf(output, "DCR, C"); break;
		case 0x15: sprintf(output, "DCR, D"); break;
		case 0x1D: sprintf(output, "DCR, E"); break;
		case 0x25: sprintf(output, "DCR, H"); break;
		case 0x2D: sprintf(output, "DCR, L"); break;
		case 0x35: sprintf(output, "DCR, M"); break;
		case 0x3D: sprintf(output, "DCR, A"); break;


		/* DCR instructions (1 byte instructions)	 */

		/* decrements the value in the register pair by 1 */

		/* opcode - 00 | __ | 1011
				 rp ->	00 - BC
					01 - DE
					10 - HL
					11 - SP
			*/

		
		case 0x0B: sprintf(output, "DCX	B"); break;
		case 0x1B: sprintf(output, "DCX	D"); break;
		case 0x2B: sprintf(output, "DCX	H"); break;
		case 0x3B: sprintf(output, "DCX	SP"); break;
	
		
		/* DAD instructions */

		/* adds the 16 bit number specified by the register pair in the HL pair with 2's complement arithmetic. */

		/* opcode - 
			00 | __ | 1001
			     rp -> 	00 - BC
					01 - DE
					10 - HL
					11 - SP
			*/



		case 0x09: sprintf(output, "DAD	B"); break;
		case 0x19: sprintf(output, "DAD	D"); break;
		case 0x29: sprintf(output, "DAD H"); break;
		case 0x39: sprintf(output, "DAD SP"); break;

		
		


		/*	ROTATE ACCUMULATOR (1 byte instructions)	*/
		
		/* opcode - 000 | __ | 111
				  op - 	00 - RLC
				  	01 - RRC
					10 - RAL
					11 - RAR
				  
			*/

		

		/*RLC - Sets the carry bit to the MSB of the accumulator AND rotates the value 
		such that the MSB of the accumulator is at the end. (1 bit rotation to the left) */

		/*RRC - Sets the carry bit to the LSB of the accumulator AND rotates the value 
		such that the LSB of the accumulator is at the beginning. (1 bit rotation to the right) */

		/*RAL - Rotates the bits by 1 to the left. MSB replaces the Carry bit, while carry bit's current value is sent to the LSB*/

		/*RAR - Rotates the bits by 1 to the right. LSB replaces the Carry bit, while carry bit's current value is sent to the MSB*/

		case 0x07: sprintf(output, "RLC"); break;
		case 0x0F: sprintf(output, "RRC"); break;
		case 0x17: sprintf(output, "RAL"); break;
		case 0x1F: sprintf(output, "RAR"); break;


		/* LOAD and STORE */
		

		/*	LDAX (1 byte instruction)	*/

		//load the contents of the memory location pointed to, by the BC register pair or the DE register pair into the accumulator.


		/* opcode - 000 | _ | 1010
				 rp -> 	0 - BC pair
					1 - DE pair

			*/


		case 0x0A: sprintf(output, "LDAX B"); break;
		case 0x1A: sprintf(output, "LDAX D"); break;



		/* Direct addressing instructions */

		/* opcode -  001 | __ | 010
				   op ->	10 - STA
						11 - LDA
						00 - SHLD
						01 - LHLD
			*/
		

		/*	LHLD -  (3 byte instruction)*/

		/*	opcode - 00101010 | low_add | hi_add	*/

		/* the byte at the memory address formed by concatenating the hi_add and low_add replace 
		the content of the H register. the next memory address replaces the contents of the L register.*/
	
	
		case 0x2A: sprintf(output, "LHLD $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;



		/*	SHLD - (3 byte instruction)	*/
		
		/* 	opcode - 00100010 | low_addr | high_addr	*/

		/*	the content of the L register is stored in the memory address formed by concatenating HIgh_addr and low_addr
			the content of the H register is stored in the next byte of memory	*/
		case 0x22: sprintf(output, "SHLD $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;

	

		/* LDA - (3 byte instruction) */

		/* opcode - 00111010 | low_add | high_add */

		/*The byte at the memory address formed by concatenating the hi_add and low_add replaces the contents of the accumulator*/


		case 0x3A: sprintf(output, "LDA $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;


		/*	STA instruction - (3 byte instruction)	*/

		/* 	opcode -  00110010 | low_addr | hi_addr	*/
		
		/* the content of the accumulator is stored at the address formed by concatenating high_addr and low_addr	*/
		
		case 0x32: sprintf(output, "STA $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;






		/* STAX instruction (1 byte instruction)	*/

		/* opcode - 000 | _ | 0010	
				  rp -> 0 - BC
					1 - DE
			*/


		case 0x02: sprintf(output, "STAX B"); break;
		case 0x12: sprintf(output, "STAX D"); break;




		/*	JUMP instructions (1 and 3 byte instructions)	*/

		/* opcode - 

			11101001 - PCHL - 1 byte instruction

			11 | ___ | 01 | _    | low_addr | hi_addr
			     000 - JMP (if last bit 1) or JNZ (if last bit 0)
			     001 - JZ
			     010 - JNC
			     011 - JC
			     100 - JPO
			     101 - JPE
			     110 - JP
			     111 - JM
			*/


		/* H-L are respectively loaded into the program counter, so the next instruction to be executed is now changed.	*/
		case 0xE9: sprintf(output, "PCHL"); break; 

		case 0xC3: sprintf(output, "JMP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump unconditionally
		case 0xC2: sprintf(output, "JNZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if zero bit is 0
		case 0xCA: sprintf(output, "JZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;// jump is zero bit is 1
		case 0xD2: sprintf(output, "JNC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if not carry bit
		case 0xDA: sprintf(output, "JC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if carry bit
		case 0xE2: sprintf(output, "JPO $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if parity bit is 0
		case 0xEA: sprintf(output, "JPE $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if parity bit is 1
		case 0xF2: sprintf(output, "JP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if sign bit is 0
		case 0xFA: sprintf(output, "JM $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if sign bit is 1



		
		/* CALL Instructions (3 byte instructions)*/
		
		/* similar to jump instructions, but the return address is pushed onto the stack	*/

		/*	opcode - 11 | ___ | 10 | _
				      op -> 	000 - CNZ
						001 - CZ (if last bit 0) or CALL (if last bit 1)
						010 - CNC
						011 - CC
						100 - CPO
						101 - CPE
						110 - CP
						111 - CM
			*/	


		case 0xCD: sprintf(output, "CALL $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //simple CALL
		case 0xC4: sprintf(output, "CNZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if zero bit is 0
		case 0xCC: sprintf(output, "CZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if zero bit is 1	
		case 0xD4: sprintf(output, "CNC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if carry bit is 0
		case 0xDC: sprintf(output, "CC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if carry bit is 1	
		case 0xE4: sprintf(output, "CPO $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if parity bit is 0
		case 0xEC: sprintf(output, "CPE $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if parity bit is 1	
		case 0xF4: sprintf(output, "CP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if sign bit is 0	
		case 0xFC: sprintf(output, "CM $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if sign bit is 1	


		/* Return instructions	(1 byte instructions)*/

		/*	pop the last address in the stack into the program counter, causing transfer of control	*/

		/*	opcode - 11 | ___ | 10 | _
				     op ->	000 - RNZ
						001 - RET (if last bit 1) or RZ (if last bit 0)
						010 - RNC
						011 - RC
						100 - RPO
						101 - RPE
						110 - RP
						111 - RM
			*/

		case 0xC9: sprintf(output, "RET"); break;
		case 0xD8: sprintf(output, "RC"); break; //ret if carry bit is 1
		case 0xD0: sprintf(output, "RNC"); break;//ret if carry bit is 0
		case 0xC8: sprintf(output, "RZ"); break;//ret if zero bit is 1
		case 0xC0: sprintf(output, "RNZ"); break;//ret if zero bit is 0
		case 0xF8: sprintf(output, "RM"); break;//ret is sign bit is 1
		case 0xF0: sprintf(output, "RP"); break;//ret if sign bit is 0
		case 0xE8: sprintf(output, "RPE"); break;//ret if parity bit is 1
		case 0xE0: sprintf(output, "RPO"); break;//ret if parity bit is 0


		/* ACCUMULATOR instructions (2 byte instructions)	*/

		/* opcode - 11 | ___ | 110
				 op ->
				000 - ADI - byte of immediate data is added to accumulator
				001 - ACI - byte of immediate data + carry bit is added to accumulator
				010 - SUI - byte of immediate data is subtracted from the accumulator
				011 - SBI - byte of immediate data + carry bit is subtracted from accumulator
				100 - ANI - byte of immediate data is logically AND'ed with accumulator. carry bit is reset (0)
				101 - XRI - byte of immediate data is XOR'ed with accumulator. carry bit is reset.
				110 - ORI - byte of immediate data is OR'ed with accumulator. carry bit is reset. zero, sign parity are set according to result...
				111 - CPI - byte of immedate data is compared with acc. zero bit is set to 1 if equal, 0 if unequal. carry bit is set to 1 if there is carry out of bit 7 => immediate data is greater.
			*/

		case 0xC6: sprintf(output, "ADI $%02X", opcode[1]); bytes = 2; break;
		case 0xCE: sprintf(output, "ACI $%02X", opcode[1]); bytes = 2; break;
		case 0xD6: sprintf(output, "SUI $%02X", opcode[1]); bytes = 2; break;
		case 0xDE: sprintf(output, "SBI $%02X", opcode[1]); bytes = 2; break;
		case 0xE6: sprintf(output, "ANI $%02X", opcode[1]); bytes = 2; break;
		case 0xEE: sprintf(output, "XRI $%02X", opcode[1]); bytes = 2; break;
		case 0xF6: sprintf(output, "ORI $%02X", opcode[1]); bytes = 2; break;
		case 0xFE: sprintf(output, "CPI $%02X", opcode[1]); bytes = 2; break;



		/*	RESTART instructions (1 byte instruction)	*/
		
		/* special purpose subroutine jumps - the contents of the program counter are pushed onto the stack, 
		providing a return address for later use.

		used in conjunction with eight 8-byte routines on the lower 64 words of memory, which are used to service interrupts.
		
		The interrupting device causes a particular RST instruction to execute, to transfer control to a subroutine to deal with the interrupt... after that, the RETURN instruction causes normal program execution, after having serviced the interrupt.


			*/

		/*opcode - 11 | ___ | 111
				exp -> 	000 to 111

			the program execution continues at 11|exp|111, thus constructing the address for the interrupt routines? i guess so...

			

			*/
		
		case 0xC7: sprintf(output, "RST 0"); break;
		case 0xCF: sprintf(output, "RST 1"); break;
		case 0xD7: sprintf(output, "RST 2"); break;
		case 0xDF: sprintf(output, "RST 3"); break;
		case 0xE7: sprintf(output, "RST 4"); break;
		case 0xEF: sprintf(output, "RST 5"); break;
		case 0xF7: sprintf(output, "RST 6"); break;
		case 0xFF: sprintf(output, "RST 7"); break;


		
		/*	STACK PUSH	*/

		/* opcode - 
				11 | rp | 0101
					rp -> 	00 - BC
						01 - DE
						10 - HL
						11 - flags and reg A??
		
			*/

		/*	the values of the specified register pair are stored in the stack. the first register 
			is stored at the address 1 less than the value of the stack pointer. the second register, 
			2 less than the value of the stack pointer.	*/
		
		/*	is PSW is specified - the first byte of the content stored is value of A register. 
			The second byte - the condition bits. - 
			S|Z|0|AC|0|P|1|C
			
			S - sign bit
			Z - zero bit
			AC - auxillary carry
			P - parity
			C - carry			

			SP is then decremented by 2

		*/

		case 0xC5: sprintf(output, "PUSH B"); break;
		case 0xD5: sprintf(output, "PUSH D"); break;
		case 0xE5: sprintf(output, "PUSH H"); break;
		case 0xF5: sprintf(output, "PUSH PSW"); break;



		/*	STACK POP	*/
		
		/* opcode - 11 | __ | 0001
				 rp -> 	00 - BC
					01 - DE
					10 - HL
					11 - PSW


			*/


		/*
			the contents of the specified register pair are restored to the resgister pairs. 
			the content at the address of SP is loaded into the second register in the pair.
			the content at the address of SP + 1 is loaded into the first register in the pair.


			PSW - restores the Accumulator and condition bits.

		*/


		case 0xC1: sprintf(output, "POP B"); break;
		case 0xD1: sprintf(output, "POP D"); break;
		case 0xE1: sprintf(output, "POP H"); break;
		case 0xF1: sprintf(output, "POP PSW"); break;


		

		/* XTHL */
		/*11100011*/
		/*	The contents of the L register are exchanged with the contents of the memory byte whose address is held in the SP.	
			the contents of the H register are excahgned with the contents of the memry bute whose address is held in the SP+1*/
		
		/* SPHL */
		//11111001 
		/* The 16 bits of data in HL are replaces by the contents of the SP. SP gets the HL value.	*/

		case 0xE3: sprintf(output, "XTHL"); break;
		case 0xF9: sprintf(output, "SPHL"); break;


		
		/* More accumulator instructions	*/

		
		/* ADD register or memory to accumulator	*/

		/*	opcode - 10000 | ___
					 reg -> 000 - B
						001 - C
						010 - D
						011 - E
						100 - H
						101 - L
						110 - M
						111 - A	
			*/

		case 0x80: sprintf(output, "ADD B"); break;
		case 0x81: sprintf(output, "ADD C"); break;
		case 0x82: sprintf(output, "ADD D"); break;
		case 0x83: sprintf(output, "ADD E"); break;
		case 0x84: sprintf(output, "ADD H"); break;
		case 0x85: sprintf(output, "ADD L"); break;
		case 0x86: sprintf(output, "ADD M"); break;
		case 0x87: sprintf(output, "ADD A"); break;


		/* ADC register to accumulator with carry	*/

		/*	opcode - 10001 | ___
					 reg -> 000 - B
						001 - C
						010 - D
						011 - E
						100 - H
						101 - L
						110 - M
						111 - A	
			*/



		case 0x88: sprintf(output, "ADC B"); break;
		case 0x89: sprintf(output, "ADC C"); break;
		case 0x8A: sprintf(output, "ADC D"); break;
		case 0x8B: sprintf(output, "ADC E"); break;
		case 0x8C: sprintf(output, "ADC H"); break;
		case 0x8D: sprintf(output, "ADC L"); break;
		case 0x8E: sprintf(output, "ADC M"); break;
		case 0x8F: sprintf(output, "ADC A"); break;


		/* SUB register from Accumulator	*/


		/*	opcode - 10010 | ___
					 reg -> 000 - B
						001 - C
						010 - D
						011 - E
						100 - H
						101 - L
						110 - M
						111 - A	
			*/


		case 0x90: sprintf(output, "SUB B"); break;
		case 0x91: sprintf(output, "SUB C"); break;
		case 0x92: sprintf(output, "SUB D"); break;
		case 0x93: sprintf(output, "SUB E"); break;
		case 0x94: sprintf(output, "SUB H"); break;
		case 0x95: sprintf(output, "SUB L"); break;
		case 0x96: sprintf(output, "SUB M"); break;
		case 0x97: sprintf(output, "SUB A"); break;

		/* SBB register + carry from Accumulator	*/

		/*	opcode - 10011 | ___
					 reg -> 000 - B
						001 - C
						010 - D
						011 - E
						100 - H
						101 - L
						110 - M
						111 - A	
			*/




		case 0x98: sprintf(output, "SBB B"); break;
		case 0x99: sprintf(output, "SBB C"); break;
		case 0x9A: sprintf(output, "SBB D"); break;
		case 0x9B: sprintf(output, "SBB E"); break;
		case 0x9C: sprintf(output, "SBB H"); break;
		case 0x9D: sprintf(output, "SBB L"); break;
		case 0x9E: sprintf(output, "SBB M"); break;
		case 0x9F: sprintf(output, "SBB A"); break;



		/* Logical and with accumulator (result in accummulator itself)	*/

		/* opcode - 10011 | ___	
				    reg 
			*/
		case 0xA0: sprintf(output, "ANA B"); break;
		case 0xA1: sprintf(output, "ANA C"); break;
		case 0xA2: sprintf(output, "ANA D"); break;
		case 0xA3: sprintf(output, "ANA E"); break;
		case 0xA4: sprintf(output, "ANA H"); break;
		case 0xA5: sprintf(output, "ANA L"); break;
		case 0xA6: sprintf(output, "ANA M"); break;
		case 0xA7: sprintf(output, "ANA A"); break;
	

		/* Logical XOR with accumulator	*/
		/* opcode - 10101 | ___	
				    reg 
			*/
		case 0xA8: sprintf(output, "XRA B"); break;
		case 0xA9: sprintf(output, "XRA C"); break;
		case 0xAA: sprintf(output, "XRA D"); break;
		case 0xAB: sprintf(output, "XRA E"); break;
		case 0xAC: sprintf(output, "XRA H"); break;
		case 0xAD: sprintf(output, "XRA L"); break;
		case 0xAE: sprintf(output, "XRA M"); break;
		case 0xAF: sprintf(output, "XRA A"); break;
		

		/* Logical OR with accumulator	*/
		/* opcode - 10110 | ___	
				    reg 
			*/
		case 0xB0: sprintf(output, "ORA B"); break;
		case 0xB1: sprintf(output, "ORA C"); break;
		case 0xB2: sprintf(output, "ORA D"); break;
		case 0xB3: sprintf(output, "ORA E"); break;
		case 0xB4: sprintf(output, "ORA H"); break;
		case 0xB5: sprintf(output, "ORA L"); break;
		case 0xB6: sprintf(output, "ORA M"); break;
		case 0xB7: sprintf(output, "ORA A"); break;
		


		/* CMP - compare with accumulator	*/
		/* opcode - 10111 | ___	
				    reg 
			*/
		/* compared by subtracting contents of register from accumulator, and the condition bits are changed accordingly	*/
		case 0xB8: sprintf(output, "CMP B"); break;
		case 0xB9: sprintf(output, "CMP C"); break;
		case 0xBA: sprintf(output, "CMP D"); break;
		case 0xBB: sprintf(output, "CMP E"); break;
		case 0xBC: sprintf(output, "CMP H"); break;
		case 0xBD: sprintf(output, "CMP L"); break;
		case 0xBE: sprintf(output, "CMP M"); break;
		case 0xBF: sprintf(output, "CMP A"); break;
		


		/* specials	*/
		case 0xEB: sprintf(output, "XCHG"); break;
		case 0x27: sprintf(output, "DAA"); break;
		case 0x2F: sprintf(output, "CMA"); break;
		case 0x37: sprintf(output, "STC"); break;
		case 0x3F: sprintf(output, "CMC"); break; //complements the carry bit
		

		/* ITS FUCKING DONE MATE!!!!	*/


	
	}
	return bytes;
}
