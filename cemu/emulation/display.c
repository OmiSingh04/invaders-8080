#include <stdint.h>

#include "logger.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

// 256 x 224 / 8 bytes = 7168 bytes. In hex - 1C11 bytes. from 0x0000 to 0x1C10. 
// Since the VRAM starts from 0x2400, we simply offset.
#define VRAM_START 0x2400
#define VRAM_END 0x3FFF

static bool init_allegro_module(){
	if(	!( al_init() 
		&& al_install_keyboard() 
		&& al_install_audio() 
		&& al_init_acodec_adon() 
		&& al_init_primitives_addon())
	){
		debug_
		return false;
	}

	puts("All Modules Installed!\n");
	return true;
}


//so im going to be drawing from the bottom up, left to right
ALLEGRO_DISPLAY* create_window(int display_width, int display_height) {
	ALLEGRO_DISPLAY* display = al_create_display(display_width, display_height);
	al_set_window_title(display, "SPACE INVADERS - 8080");
	return display;
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
