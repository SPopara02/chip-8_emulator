#ifndef PERIPHERALS_H
#define PERIPHERALS_H
#include <stdbool.h>

bool gInit();
void gShutdown();
void gDrawFrame(unsigned char vram[32][64]);
void gClearScreen();
void kEventHandler(SDL_Event e, unsigned char* keypad);

#endif
