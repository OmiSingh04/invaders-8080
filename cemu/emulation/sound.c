#include <stdio.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "sound.h"



ALLEGRO_SAMPLE* sounds[TOTAL_SOUNDS];


void load_sounds() {
	char path[256];

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
}

void play_sound(int sound) {
	if (sound >= TOTAL_SOUNDS)
		return;
	al_play_sample(sounds[sound], 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
}

void deinit_sounds() {
	for (int i = 0; i < TOTAL_SOUNDS; i++)
		al_destroy_sample(sounds[i]);
}
