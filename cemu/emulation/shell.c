#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stdint.h>
#include<string.h>

#include "file_util.h"
#include "disassembler.h"
#include "state.h"
#include "input.h"
#include "display.h"
#include "sound.h"
#include "logger.h"

bool x = false;

int main(int argc, char* argv[]){

	init_logger();

	state_8080 cpu = {0, 0, 0, 0, 0, 0, 0, false, {0x00}, 0, 0, 0, 0, NULL, 0, 0};

	if (!init_state(&cpu)){
		exit(1);
	}

	//Select program
	puts("Enter the value for the test/rom you want to load");
	puts("1. SPACE INVADERS\n2. CPUTEST\n3. 8080PRE\n4. 8080EXER\n5. 8080EXM");

	char c = getchar();
	if (c == EOF) {
		deinit_state(&cpu);
		exit(1);
	}

	int offset = load_file(&cpu, c);//returns the offset - if the system runs on CP/M - the program loads from after zero page - 256
	if (offset == -1) {
		debug_print("Could not read file!", ERROR);
		deinit_state(&cpu);
		terminate_logger();
		exit(0);
	}

	cpu.pc = offset;
	long total_cycles = 0;

	debug_print("CPU set up!\n", DEBUG); 

	//log discretes, only 100000 at a time to not generate gigabytes of logs.

	if (c != ROM) {
		while (continue_test) {
			if (cpu.pc == 0)
				break;
			
			if (cpu.instructions_ran == 30574014L)
				x = true;
			
			if (cpu.instructions_ran > 33971127L)
				break;
			
			if (x)
				log_instruction_state_2(cpu);
			int current_cycles;
			emulate_8080(&cpu, &current_cycles);
		}
	}
	else {
		//run game
		int display_height = 768;
		int display_width = 672;
		if (!init_gui(display_width, display_height))
			goto deinit;
			

		int clocks_per_second = 2000000;
		int interrupts_per_second = 120;
		double clocks_per_interrupt = (double)clocks_per_second / interrupts_per_second;

		double last_time = get_time_seconds();

		bool vblank = false;

		int frames = 0;

		double start = last_time;
		long cycles_consumed = 0;

		int breaks = 0;
		

		while (true) {
			//Get the current time each iteration

			if (cpu.pc == 0) {
				breaks++;
				if (breaks == 2) break;
			}

			
			if (cycles_consumed < clocks_per_interrupt) {
				int current_cycles = 0;
				if (cpu.memory[cpu.pc] == 0xDB) {
					machine_key_press();
					uint8_t port = cpu.memory[cpu.pc + 1];
					machine_in(&cpu, port);
					current_cycles = 10;
					cpu.pc += 2;
				}
				else if (cpu.memory[cpu.pc] == 0xD3) {
					uint8_t port = cpu.memory[cpu.pc + 1];
					machine_out(&cpu, port);
					current_cycles = 10;
					cpu.pc += 2;
				}
				else {
					emulate_8080(&cpu, &current_cycles);
					cycles_consumed += current_cycles;
					total_cycles += current_cycles;
				}
			}
			else {
				double now = al_get_time();
				if (now - last_time > (1.0 / interrupts_per_second)) {
					if (!vblank) {
						generate_vblank(&cpu, 0xCF);
						vblank = true;
					}
					else {
						render_mem(cpu.memory);
						generate_vblank(&cpu, 0xD7);
						vblank = false;
					}
					last_time = now;
					cycles_consumed -= clocks_per_interrupt;
				}
			}
			if(al_key_down(&keyboard_state, ALLEGRO_KEY_ESCAPE))
					break;
		}

		deinit: destroy_gui();

	}

	terminate_logger();
	free(cpu.memory);
	return 0;

}


//33971128
//10191338
//45073081 / 3 = 
/*
0			3397112.0 - 1 - done
3397113.0	6794225.0 - 2 - done
6794226.0	10191337.0 - 3 - done
10191338.0	13588450.0 - 4 - done
13588451.0	16985563.0 - 5 - done
16985564.0	20382675.0 - 6 - done
20382676.0	23779788.0 - 7 - done
23779789.0	27176901.0 - 8 - done
27176902.0	30574013.0 - 9 - done
30574014.0	33971127.0 - 10 - ongoing
*/
		
