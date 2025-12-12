#ifndef CHIP_8_H
#define CHIP_8_H
#include <stdbool.h>

extern unsigned char vram[32][64];
extern unsigned char keypad[16];
bool cLoadRom(const char *path);
bool cInit();
void cEmulateCycle();
int cHandleTimers();

#endif
