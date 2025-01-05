#include <stdint.h>


#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#include "display.h"
#include "logger.h"
#include "sound.h"

// 256 x 224 / 8 bytes = 7168 bytes. In hex - 1C11 bytes. from 0x0000 to 0x1C10. 
// Since the VRAM starts from 0x2400, we simply offset.
#define VRAM_START 0x2400
#define VRAM_END 0x3FFF
 
static ALLEGRO_DISPLAY* display;	
static ALLEGRO_EVENT_QUEUE* queue;

//so im going to be drawing from the bottom up, left to right
bool init_gui(int display_width, int display_height) {
	bool inits[5] = { 0 };
	inits[0] = al_init();
	if (!inits[0]) {
		debug_print("ALLEGRO COULD NOT BE INITIATED", ERROR);
		return false;
	}
	inits[1] = al_install_keyboard();
	if (!inits[1]) {
		debug_print("ALLEGRO KEYBOARD COULD NOT BE INITIATED", ERROR);
		return false;
	}
	inits[2] = al_install_audio();
	if (!inits[2]) {
		debug_print("ALLEGRO AUDIO COULD NOT BE INITIATED", ERROR);
		return false;
	}
	inits[3] = al_init_primitives_addon();
	if (!inits[3]) {
		debug_print("ALLEGRO PRIMITIVES COULD NOT BE INITIATED", ERROR);
		return false;
	}
	inits[4] = init_sounds();
	if (!inits[4]) {
		debug_print("SOUND FILES COULD NOT BE INITIATED", ERROR);
		return false;
	}



	debug_print("All Modules Installed!", DEBUG);

	//display
	display = al_create_display(display_width, display_height);
	if (!display) {
		debug_print("COULD NOT INITIATE DISPLAY, CLOSING", ERROR);
		destroy_gui();
		return false;
	}

	al_set_window_title(display, "SPACE INVADERS - 8080");

	//event queue
	queue = al_create_event_queue();
	if (!queue) {
		debug_print("COULD NOT INITIATE EVENT QUEUE, CLOSING", ERROR);
		destroy_gui();
		return false;
	}
	al_register_event_source(queue, al_get_keyboard_event_source());
	al_register_event_source(queue, al_get_display_event_source(display));

	debug_print("INITIATED GUI COMPONENTS!", DEBUG);

	return true;
}

void render_mem(uint8_t* mem) {
	ALLEGRO_COLOR white = al_map_rgb(255, 255, 255);
	al_clear_to_color(al_map_rgb(0, 0, 0));
	int i = 767, j = 0;
	for (uint16_t x = VRAM_START; x <= VRAM_END; x++) {
		uint8_t bits = mem[x];
		for (int y = 0; y < 8; y++) {		
			int bit = bits & 1;
			if (bit)
				al_draw_filled_rectangle(j, i, j + 2, i - 2, white);
			i -= 3;
			bits >>= 1;
		}
		if (i <= 0) {
			j += 3;
			i = 767;
		}
	}
	al_flip_display();
}

double get_time_seconds() {
	return al_get_time();
}

//resource cleanup - the only place i wish i was in C++
void destroy_gui() {
	//https://github.com/liballeg/allegro5/blob/065a727f92dc2cb62b603d2e7451697f19d247e7/src/display.c#L144
	al_destroy_display(display);
	al_destroy_event_queue(queue);
	al_shutdown_primitives_addon();
	deinit_sounds();
	al_uninstall_system();
}

