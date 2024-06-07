#include <stdio.h>
#include <stdbool.h>
#include "sound.h"
#include "logger.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_audio.h>


ALLEGRO_SAMPLE* sounds[TOTAL_SOUNDS];


bool init_sounds() {
	if (!al_install_audio()) {
		debug_print("Could not install audio addon!", ERROR);
		return false;
	}

	if (!al_init_acodec_addon()) {
		debug_print("Could not install acodec addon!", ERROR);
		return false;
	}

	char path[256] = { 0 };

	sprintf(path, "%s%s", SOUND_FILES, "/kill.wav");
	sounds[KILL] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/shoot.wav"); 
	sounds[SHOOT] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/fleet_1.wav"); 
	sounds[FLEET_MOVEMENT_1] = al_load_sample(path);
	
	sprintf(path, "%s%s", SOUND_FILES, "/fleet_2.wav"); 
	sounds[FLEET_MOVEMENT_2] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/fleet_3.wav"); 
	sounds[FLEET_MOVEMENT_3] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/fleet_4.wav"); 
	sounds[FLEET_MOVEMENT_4] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/die.wav"); 
	sounds[DIE] = al_load_sample(path);

	sprintf(path, "%s%s", SOUND_FILES, "/die.wav"); 
	sounds[UFO] = al_load_sample(path);


	//check if all the loads were valid.
	for (int i = 0; i < TOTAL_SOUNDS; i++)
		if (!sounds[i]) return false;

	return true;
}

void play_sound(int sound) {
	if (sound >= TOTAL_SOUNDS || sound < 0)
		return;
	al_play_sample(sounds[sound], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void deinit_sounds() {
	for (int i = 0; i < TOTAL_SOUNDS; i++)
		al_destroy_sample(sounds[i]);
}

