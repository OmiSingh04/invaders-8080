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

	state_8080 cpu = {0, 0, 0, 0, 0, 0, 0, false, {0x00}, 0, 0, 0, 0, NULL};

	if (!init_state(&cpu)){
		exit(1);
	}

	FILE* exm = fopen(EXM_PATH, "rb");//provided as args from CMake
	FILE* exer = fopen(EXER_PATH, "rb");
	FILE* pre = fopen(PRE_PATH, "rb");
	FILE* cputest = fopen(DIAG_PATH, "rb");
	int exm_offset = 0x100;
	FILE* rom = fopen(ROM_PATH, "rb");
	int rom_offset = 0;

	bool x = read_file(exm, cpu.memory, exm_offset);
	if (!x) {
		free(cpu.memory);
		fclose(exm);
		exit(1);
	}
	

	#ifdef CPU_DIAG 
		cpu.pc = 0x100;
		cpu.memory[0] = 0xD3;
		//cpu.memory[0x0005] = 0xD3;
		//cpu.memory[0x0006] = 0x01;
		//cpu.memory[0x0007] = 0xC9;
	#endif

	debug_print("CPU set up!\n", DEBUG); 
	char* instruction_str_ptr = malloc(20);

	int display_height = 768;
	int display_width = 672;
		
	/*
	bool setup = init_allegro_module();
	ALLEGRO_DISPLAY* display = create_window();

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
	long total_cycles = 0;
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
	free(instruction_str_ptr);
	free(cpu.memory);
	return 0;

}
/*
0000 - 1FFF 8K ROM
2000 - 23FF 1K RAM
2400 - 3FFF 7K Video RAM
4000 - ~    RAM mirror

8k ROM - 
invaders.h 0000-07FF
invaders.g 0800-0FFF
invaders.f 1000-17FF
invaders.e 1800-1FFF


Video - 256 x 224. - height x width!
Each pixel is on or off. (Hence requires one bit)
256 x 224 / 8 = 7168 ~ 7k bytes

Max. CPU clock rate	2 MHz to 3.125 MHz
*/


/*
* 
* So the idea we will go for is - we have 2MHz clock rate. meaning 2 Million cycles can occur in 1 second
* We have the video output going at 60 Hz
* To sync our instructions to the video output, we run 2 million clock cycles/s / 60 frames/s = which is 33333 cycles / frame.
* So we process 33333 cycles for the next frame, then wait until 1/60 of a second, since this cpu is much faster. :)
* 
* I have also not implemented full/half vblank yet.
* 
* You have 2 different interrupts generated at 60 Hz, one when the beam is near the middle of the screen, and one at the end of the screen
* To simulate this, we generate could... generate 120 Hz in the loop, and each would alternatively mean the two different RSTs.
* 
*/

