#include<stdio.h>
#include<stdint.h>
//meow
int disassembler_8080(unsigned char* buffer, uint16_t pc){

	unsigned char* opcode = buffer + pc;
	
	//*opcode -= 48;//ascii character to an actual number, so we offset by 48, the buffer is in a text file.
	int bytes = 1;//assume it is a 1 byte instruction, no args.

	printf("%04X ", pc + 0x100);
	switch(*opcode){


		/*control instructions */
		case 0x00: printf("NOP"); break;
		case 0x08: printf("NOP"); break;
		

		/* interrupt flip-flop instructions */

		/* opcode - 1111 | _ | 011
				   x ->	1 for EI
					0 for DI

			*/

		case 0xFB: printf("EI"); break;//enable interrupt - enables cpu to respond to interrupts
		case 0xF3: printf("DI"); break;//disable interrupt




		/* Input-Output instructions (2 byte instructions)	*/
	
		/* opcode - 	1101 | _ | 011	|	exp (8 bit)
				       x ->	1 - IN
						2 - OUT

			*/


		/*	IN - a byte is read from input device exp and replaces the contents of the accumulator*/
		/*	OUT - the accumulator value is sent to the output device exp	*/


		case 0xDB: printf("IN $%02X", opcode[1]); bytes = 2; break;
		case 0xD3: printf("OUT $%02X", opcode[1]); bytes = 2; break;





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
		case 0x01: printf("LXI	B, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x11: printf("LXI	D, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x21: printf("LXI	H, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;
		case 0x31: printf("LXI SP, $%02x%02X", opcode[2], opcode[1]); bytes = 3; break;



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
		case 0x06: printf("MVI 	B, $%02x", opcode[1]); bytes = 2; break;
		case 0x0E: printf("MVI	C, $%02X", opcode[1]); bytes = 2; break;
		case 0x16: printf("MVI	D, $%02X", opcode[1]); bytes = 2; break;
		case 0x1E: printf("MVI	E, $%02X", opcode[1]); bytes = 2; break;
		case 0x26: printf("MVI	H, $%02X", opcode[1]); bytes = 2; break;
		case 0x2E: printf("MVI 	L, $%02X", opcode[1]); bytes = 2; break;
		case 0x36: printf("MVI 	M, $%02X", opcode[1]); bytes = 2; break;
		case 0x3E: printf("MVI  A, $%02X", opcode[1]); bytes = 2; break;
	

			
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
		case 0x40: printf("MOV B, B"); break; //useless i guess...
		case 0x41: printf("MOV B, C"); break;
		case 0x42: printf("MOV B, D"); break;
		case 0x43: printf("MOV B, E"); break;
		case 0x44: printf("MOV B, H"); break;
		case 0x45: printf("MOV B, L"); break;
		case 0x46: printf("MOV B, M"); break;
		case 0x47: printf("MOV B, A"); break;


		/*	MOV C <- src	*/
		case 0x48: printf("MOV C, B"); break;
		case 0x49: printf("MOV C, C"); break;
		case 0x4A: printf("MOV C, D"); break;
		case 0x4B: printf("MOV C, E"); break;
		case 0x4C: printf("MOV C, H"); break;
		case 0x4D: printf("MOV C, L"); break;
		case 0x4E: printf("MOV C, M"); break;
		case 0x4F: printf("MOV C, A"); break;

		/* 	MOV D <- src */
		case 0x50: printf("MOV D, B"); break;
		case 0x51: printf("MOV D, C"); break;
		case 0x52: printf("MOV D, D"); break;
		case 0x53: printf("MOV D, E"); break;
		case 0x54: printf("MOV D, H"); break;
		case 0x55: printf("MOV D, L"); break;
		case 0x56: printf("MOV D, M"); break;
		case 0x57: printf("MOV D, A"); break;



		/*	MOV E <- src	*/
		case 0x58: printf("MOV E, B"); break;
		case 0x59: printf("MOV E, C"); break;
		case 0x5A: printf("MOV E, D"); break;
		case 0x5B: printf("MOV E, E"); break;
		case 0x5C: printf("MOV E, H"); break;
		case 0x5D: printf("MOV E, L"); break;
		case 0x5E: printf("MOV E, M"); break;
		case 0x5F: printf("MOV E, A"); break;

	
		/*	MOV H <- src 	*/
		case 0x60: printf("MOV H, B"); break;
		case 0x61: printf("MOV H, C"); break;
		case 0x62: printf("MOV H, D"); break;
		case 0x63: printf("MOV H, E"); break;
		case 0x64: printf("MOV H, H"); break;
		case 0x65: printf("MOV H, L"); break;
		case 0x66: printf("MOV H, M"); break;
		case 0x67: printf("MOV H, A"); break;

		/*	MOV L <- src	*/
		case 0x68: printf("MOV L, B"); break;
		case 0x69: printf("MOV L, C"); break;
		case 0x6A: printf("MOV L, D"); break;
		case 0x6B: printf("MOV L, E"); break;
		case 0x6C: printf("MOV L, H"); break;
		case 0x6D: printf("MOV L, L"); break;
		case 0x6E: printf("MOV L, M"); break;
		case 0x6F: printf("MOV L, A"); break;

		/*	MOV M <- src	*/
		case 0x70: printf("MOV M, B"); break;
		case 0x71: printf("MOV M, C"); break;
		case 0x72: printf("MOV M, D"); break;
		case 0x73: printf("MOV M, E"); break;
		case 0x74: printf("MOV M, H"); break;
		case 0x75: printf("MOV M, L"); break;
		case 0x76: printf("HLT"); break; //src and dest cant be M and M
		case 0x77: printf("MOV M, A"); break;


		/*	MOV A <- src	*/
		case 0x78: printf("MOV A, B"); break;
		case 0x79: printf("MOV A, C"); break;
		case 0x7A: printf("MOV A, D"); break;
		case 0x7B: printf("MOV A, E"); break;
		case 0x7C: printf("MOV A, H"); break;
		case 0x7D: printf("MOV A, L"); break;
		case 0x7E: printf("MOV A, M"); break;
		case 0x7F: printf("MOV A, A"); break; //god i make so many typoes
		




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
		
		case 0x04: printf("INR,	B"); break;
		case 0x0C: printf("INR,	C"); break;
		case 0x14: printf("INR,	D"); break;
		case 0x1C: printf("INR,	E"); break;
		case 0x24: printf("INR,	H"); break;
		case 0x2C: printf("INR, L"); break;
		case 0x34: printf("INR, M"); break; 	
		case 0x3C: printf("INR, A"); break;
	

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

		case 0x03: printf("INX,	B"); break;
		case 0x13: printf("INX,	D"); break;
		case 0x23: printf("INX,	H"); break;
		case 0x33: printf("INX,	SP"); break;


		
	
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
				
		case 0x05: printf("DCR,	B"); break;
		case 0x0D: printf("DCR,	C"); break;
		case 0x15: printf("DCR,	D"); break;
		case 0x1D: printf("DCR,	E"); break;
		case 0x25: printf("DCR,	H"); break;
		case 0x2D: printf("DCR,	L"); break;
		case 0x35: printf("DCR, M"); break;
		case 0x3D: printf("DCR,	A"); break;


		/* DCR instructions (1 byte instructions)	 */

		/* decrements the value in the register pair by 1 */

		/* opcode - 00 | __ | 1011
				 rp ->	00 - BC
					01 - DE
					10 - HL
					11 - SP
			*/

		
		case 0x0B: printf("DCX	B"); break;
		case 0x1B: printf("DCX	D"); break;
		case 0x2B: printf("DCX	H"); break;
		case 0x3B: printf("DCX	SP"); break;
	
		
		/* DAD instructions */

		/* adds the 16 bit number specified by the register pair in the HL pair with 2's complement arithmetic. */

		/* opcode - 
			00 | __ | 1001
			     rp -> 	00 - BC
					01 - DE
					10 - HL
					11 - SP
			*/



		case 0x09: printf("DAD	B"); break;
		case 0x19: printf("DAD	D"); break;
		case 0x29: printf("DAD 	H"); break;
		case 0x39: printf("DAD 	SP"); break;

		
		


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

		case 0x07: printf("RLC"); break;
		case 0x0F: printf("RRC"); break;
		case 0x17: printf("RAL"); break;
		case 0x1F: printf("RAR"); break;


		/* LOAD and STORE */
		

		/*	LDAX (1 byte instruction)	*/

		//load the contents of the memory location pointed to, by the BC register pair or the DE register pair into the accumulator.


		/* opcode - 000 | _ | 1010
				 rp -> 	0 - BC pair
					1 - DE pair

			*/


		case 0x0A: printf("LDAX	B"); break;
		case 0x1A: printf("LDAX	D"); break;



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
	
	
		case 0x2A: printf("LHLD	$%02X%02X", opcode[2], opcode[1]); bytes = 3; break;



		/*	SHLD - (3 byte instruction)	*/
		
		/* 	opcode - 00100010 | low_addr | high_addr	*/

		/*	the content of the L register is stored in the memory address formed by concatenating HIgh_addr and low_addr
			the content of the H register is stored in the next byte of memory	*/
		case 0x22: printf("SHLD	$%02X%02X", opcode[2], opcode[1]); bytes = 3; break;

	

		/* LDA - (3 byte instruction) */

		/* opcode - 00111010 | low_add | high_add */

		/*The byte at the memory address formed by concatenating the hi_add and low_add replaces the contents of the accumulator*/


		case 0x3A: printf("LDA, $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;


		/*	STA instruction - (3 byte instruction)	*/

		/* 	opcode -  00110010 | low_addr | hi_addr	*/
		
		/* the content of the accumulator is stored at the address formed by concatenating high_addr and low_addr	*/
		
		case 0x32: printf("STA $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;






		/* STAX instruction (1 byte instruction)	*/

		/* opcode - 000 | _ | 0010	
				  rp -> 0 - BC
					1 - DE
			*/


		case 0x02: printf("STAX B"); break;
		case 0x12: printf("STAX D"); break;




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
		case 0xE9: printf("PCHL"); break; 

		case 0xC3: printf("JMP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump unconditionally
		case 0xC2: printf("JNZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if zero bit is 0
		case 0xCA: printf("JZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;// jump is zero bit is 1
		case 0xD2: printf("JNC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if not carry bit
		case 0xDA: printf("JC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if carry bit
		case 0xE2: printf("JPO $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if parity bit is 0
		case 0xEA: printf("JPE $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if parity bit is 1
		case 0xF2: printf("JP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if sign bit is 0
		case 0xFA: printf("JM $%02X%02X", opcode[2], opcode[1]); bytes = 3; break;//jump if sign bit is 1



		
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


		case 0xCD: printf("CALL $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //simple CALL
		case 0xC4: printf("CNZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if zero bit is 0
		case 0xCC: printf("CZ $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if zero bit is 1	
		case 0xD4: printf("CNC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if carry bit is 0
		case 0xDC: printf("CC $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if carry bit is 1	
		case 0xE4: printf("CPO $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if parity bit is 0
		case 0xEC: printf("CPE $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if parity bit is 1	
		case 0xF4: printf("CP $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if sign bit is 0	
		case 0xFC: printf("CM $%02X%02X", opcode[2], opcode[1]); bytes = 3; break; //call if sign bit is 1	


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

		case 0xC9: printf("RET"); break;
		case 0xD8: printf("RC"); break; //ret if carry bit is 1
		case 0xD0: printf("RNC"); break;//ret if carry bit is 0
		case 0xC8: printf("RZ"); break;//ret if zero bit is 1
		case 0xC0: printf("RNZ"); break;//ret if zero bit is 0
		case 0xF8: printf("RM"); break;//ret is sign bit is 1
		case 0xF0: printf("RP"); break;//ret if sign bit is 0
		case 0xE8: printf("RPE"); break;//ret if parity bit is 1
		case 0xE0: printf("RPO"); break;//ret if parity bit is 0


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

		case 0xC6: printf("ADI $%02X", opcode[1]); bytes = 2; break;
		case 0xCE: printf("ACI $%02X", opcode[1]); bytes = 2; break;
		case 0xD6: printf("SUI $%02X", opcode[1]); bytes = 2; break;
		case 0xDE: printf("SBI $%02X", opcode[1]); bytes = 2; break;
		case 0xE6: printf("ANI $%02X", opcode[1]); bytes = 2; break;
		case 0xEE: printf("XRI $%02X", opcode[1]); bytes = 2; break;
		case 0xF6: printf("ORI $%02X", opcode[1]); bytes = 2; break;
		case 0xFE: printf("CPI $%02X", opcode[1]); bytes = 2; break;



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
		
		case 0xC7: printf("RST 0"); break;
		case 0xCF: printf("RST 1"); break;
		case 0xD7: printf("RST 2"); break;
		case 0xDF: printf("RST 3"); break;
		case 0xE7: printf("RST 4"); break;
		case 0xEF: printf("RST 5"); break;
		case 0xF7: printf("RST 6"); break;
		case 0xFF: printf("RST 7"); break;


		
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

		case 0xC5: printf("PUSH B"); break;
		case 0xD5: printf("PUSH D"); break;
		case 0xE5: printf("PUSH H"); break;
		case 0xF5: printf("PUSH PSW"); break;



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


		case 0xC1: printf("POP B"); break;
		case 0xD1: printf("POP D"); break;
		case 0xE1: printf("POP H"); break;
		case 0xF1: printf("POP PSW"); break;


		

		/* XTHL */
		/*11100011*/
		/*	The contents of the L register are exchanged with the contents of the memory byte whose address is held in the SP.	
			the contents of the H register are excahgned with the contents of the memry bute whose address is held in the SP+1*/
		
		/* SPHL */
		//11111001 
		/* The 16 bits of data in HL are replaces by the contents of the SP. SP gets the HL value.	*/

		case 0xE3: printf("XTHL"); break;
		case 0xF9: printf("SPHL"); break;


		
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

		case 0x80: printf("ADD B"); break;
		case 0x81: printf("ADD C"); break;
		case 0x82: printf("ADD D"); break;
		case 0x83: printf("ADD E"); break;
		case 0x84: printf("ADD H"); break;
		case 0x85: printf("ADD L"); break;
		case 0x86: printf("ADD M"); break;
		case 0x87: printf("ADD A"); break;


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



		case 0x88: printf("ADC B"); break;
		case 0x89: printf("ADC C"); break;
		case 0x8A: printf("ADC D"); break;
		case 0x8B: printf("ADC E"); break;
		case 0x8C: printf("ADC H"); break;
		case 0x8D: printf("ADC L"); break;
		case 0x8E: printf("ADC M"); break;
		case 0x8F: printf("ADC A"); break;


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


		case 0x90: printf("SUB B"); break;
		case 0x91: printf("SUB C"); break;
		case 0x92: printf("SUB D"); break;
		case 0x93: printf("SUB E"); break;
		case 0x94: printf("SUB H"); break;
		case 0x95: printf("SUB L"); break;
		case 0x96: printf("SUB M"); break;
		case 0x97: printf("SUB A"); break;

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




		case 0x98: printf("SBB B"); break;
		case 0x99: printf("SBB C"); break;
		case 0x9A: printf("SBB D"); break;
		case 0x9B: printf("SBB E"); break;
		case 0x9C: printf("SBB H"); break;
		case 0x9D: printf("SBB L"); break;
		case 0x9E: printf("SBB M"); break;
		case 0x9F: printf("SBB A"); break;



		/* Logical and with accumulator (result in accummulator itself)	*/

		/* opcode - 10011 | ___	
				    reg 
			*/
		case 0xA0: printf("ANA B"); break;
		case 0xA1: printf("ANA C"); break;
		case 0xA2: printf("ANA D"); break;
		case 0xA3: printf("ANA E"); break;
		case 0xA4: printf("ANA H"); break;
		case 0xA5: printf("ANA L"); break;
		case 0xA6: printf("ANA M"); break;
		case 0xA7: printf("ANA A"); break;
	

		/* Logical XOR with accumulator	*/
		/* opcode - 10101 | ___	
				    reg 
			*/
		case 0xA8: printf("XRA B"); break;
		case 0xA9: printf("XRA C"); break;
		case 0xAA: printf("XRA D"); break;
		case 0xAB: printf("XRA E"); break;
		case 0xAC: printf("XRA H"); break;
		case 0xAD: printf("XRA L"); break;
		case 0xAE: printf("XRA M"); break;
		case 0xAF: printf("XRA A"); break;
		

		/* Logical OR with accumulator	*/
		/* opcode - 10110 | ___	
				    reg 
			*/
		case 0xB0: printf("ORA B"); break;
		case 0xB1: printf("ORA C"); break;
		case 0xB2: printf("ORA D"); break;
		case 0xB3: printf("ORA E"); break;
		case 0xB4: printf("ORA H"); break;
		case 0xB5: printf("ORA L"); break;
		case 0xB6: printf("ORA M"); break;
		case 0xB7: printf("ORA A"); break;
		


		/* CMP - compare with accumulator	*/
		/* opcode - 10111 | ___	
				    reg 
			*/
		/* compared by subtracting contents of register from accumulator, and the condition bits are changed accordingly	*/
		case 0xB8: printf("CMP B"); break;
		case 0xB9: printf("CMP C"); break;
		case 0xBA: printf("CMP D"); break;
		case 0xBB: printf("CMP E"); break;
		case 0xBC: printf("CMP H"); break;
		case 0xBD: printf("CMP L"); break;
		case 0xBE: printf("CMP M"); break;
		case 0xBF: printf("CMP A"); break;
		


		/* specials	*/
		case 0xEB: printf("XCHG"); break;
		case 0x27: printf("DAA"); break;
		case 0x2F: printf("CMA"); break;
		case 0x37: printf("STC"); break;
		case 0x3F: printf("CMC"); break; //complements the carry bit
		

		/* ITS FUCKING DONE MATE!!!!	*/


	
	}
	printf("\n");
	return bytes;
}
