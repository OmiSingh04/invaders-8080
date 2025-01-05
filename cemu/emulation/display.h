/*
* The resolution is 256x224 at 60 Hz
* 256 x 224 / 8 bytes = 7168 bytes. In hex - 1C11 bytes. from 0x0000 to 0x1C10. Since the VRAM starts from 0x2400, we simply offset.
*/

#ifndef DISPLAY_H_
#define DISPLAY_H

bool init_gui(int, int);
void render_mem(uint8_t*);
void destroy_gui();
double get_time_seconds();	//did i need to wrap it?
#endif
