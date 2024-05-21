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


	printf("%d", c == ROM);
	if (c != ROM) {
		while (continue_test) {
			if (cpu.pc == 0)
				break;
			int current_cycles;
			log_instruction_state(cpu);
			emulate_8080(&cpu, &current_cycles);
		}
	}

	terminate_logger();
	free(cpu.memory);
	return 0;
		
	/*
	//only needed if the rom chosen is space invaders.
	int display_height = 768;
	int display_width = 672;
	ALLEGRO_DISPLAY* isplay = create_window();

	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	int clocks_per_second = 2000000;
	int interrupts_per_second = 120;
	double clocks_per_interrupt = (double)clocks_per_second / interrupts_per_second;

	double last_time = al_get_time();

	bool vblank = false;
	init_sounds();

	int frames = 0;

	double start = last_time;
	long cycles_consumed = 0;
	

	while (setup) {
		//Get the current time each iteration

		if (cpu.pc == 0) break;
		
		if(cycles_consumed < clocks_per_interrupt){


			int current_cycles = 0;
			if (cpu.memory[cpu.pc] == 0xDB) {
				//handle IN
				machine_key_press();
				uint8_t port = cpu.memory[cpu.pc + 1];
				machine_in(&cpu, port);//make it update the accumulator itself.
				//printf("%s %d\n", "IN", port);
				current_cycles = 10;
				cpu.pc += 2;
			}
			else if (cpu.memory[cpu.pc] == 0xD3) {
				//handle OUT
				uint8_t port = cpu.memory[cpu.pc + 1];
				machine_out(&cpu, port);
				//printf("%s %d\n", "OUT", port);
				current_cycles = 10;
				cpu.pc += 2;
			}
			else {
			#ifdef CPU_DIAG_PRINT
				//if (instructions_ran > 3749) {
					disassemble_8080(cpu.memory, cpu.pc, instruction_str_ptr);
					printf("%s\n", instruction_str_ptr);
				//}
			#endif
				emulate_8080(&cpu, &current_cycles);
		

			#ifdef CPU_DIAG_PRINT
				//if (instructions_ran > 3749) 
					printf("%ld cycles passed\n", cycles_consumed + current_cycles);
			#endif
			}
			#ifndef CPU_DIAG
				cycles_consumed += current_cycles;
				total_cycles += current_cycles;
			#endif
		} //emulate 1 instruction, n cycles.
		/*
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
	deinit_sounds();
	*/

}
