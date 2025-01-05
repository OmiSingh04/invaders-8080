#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <allegro5/allegro5.h>
#include "input.h"


bool key_state[9];
ALLEGRO_KEYBOARD_STATE keyboard_state;

//to handle the inputs that would be given to the space invaders game. it had a large case with a display, and buttons.
//I think i need to reconsider how i handle input here/

uint8_t machine_key_press() {
	al_get_keyboard_state(&keyboard_state);

	key_state[P1_LEFT] = al_key_down(&keyboard_state, ALLEGRO_KEY_A);
	key_state[P1_RIGHT] = al_key_down(&keyboard_state, ALLEGRO_KEY_D);
	key_state[P1_FIRE] = al_key_down(&keyboard_state, ALLEGRO_KEY_SPACE);
	key_state[P1_START] = al_key_down(&keyboard_state, ALLEGRO_KEY_1);

	key_state[P2_LEFT] = al_key_down(&keyboard_state, ALLEGRO_KEY_LEFT);
	key_state[P2_RIGHT] = al_key_down(&keyboard_state, ALLEGRO_KEY_RIGHT);
	key_state[P2_FIRE] = al_key_down(&keyboard_state, ALLEGRO_KEY_UP);
	key_state[P2_START] = al_key_down(&keyboard_state, ALLEGRO_KEY_2);

	key_state[COIN_INSERT] = al_key_down(&keyboard_state, ALLEGRO_KEY_LSHIFT);
	return 0;//why am i returning this??
}
