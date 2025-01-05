#ifndef SOUND_H_
#define SOUND_H_

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

#define TOTAL_SOUNDS 8

enum sound_map {
	SHOOT = 0, 
	FLEET_MOVEMENT_1,
	FLEET_MOVEMENT_2,
	FLEET_MOVEMENT_3,
	FLEET_MOVEMENT_4,
	DIE,
	KILL, 
	UFO
};

extern ALLEGRO_SAMPLE* sound[];

bool init_sounds();
void play_sound(int sound);
void deinit_sounds();
#endif
