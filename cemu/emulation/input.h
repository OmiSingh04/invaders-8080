#ifndef INPUT_H_
#define INPUT_H

#include <allegro5/allegro.h>

enum key_map {
	P1_LEFT = 0, 
	P1_RIGHT, 
	P1_FIRE,
	P1_START,

	P2_LEFT,
	P2_RIGHT,
	P2_FIRE,
	P2_START,

	COIN_INSERT,
};

extern ALLEGRO_KEYBOARD_STATE keyboard_state;
extern bool key_state[];
//updates the boolean array of the required buttons
uint8_t machine_key_press();
#endif
