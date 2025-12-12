#include <stdio.h>
#include <stdbool.h>
#include <windows.h>
#include <unistd.h>
#include <stdlib.h>
#include <SDL.h>
#include "peripherals.h"
#include "chip_8.h"

int main(int argc,char* args[])
{
    if (argc < 3){
        printf("Nedovoljno argumenata!\nFormat: %s <inst_po_frejmu> <putanja>\n", args[0]);
        return 0;
    }

    char *krajstr = NULL;
    long ipf = strtol(args[1], &krajstr, 10);
    if(*krajstr != '\0' || ipf <= 0){
        printf("Nevalidan broj instrukcija po frejmu: '%s'\n", args[1]);
        return 0;
    }
    const char* putanja = args[2];
    if(!cLoadRom(putanja)){
        printf("Error loading ROM!\n");
        return 0;
    }
    else printf("ROM loaded!\n");

    if(!cInit()){
        printf("Chip initialization failed!\n");
        return 0;
    }
    else printf("Chip initialized!\n");

    if(!gInit()){
        printf("Graphics initializaton failed!\n");
        return 0;
    }
    else printf("Graphics initialized!\n");

    SDL_Event e;
    bool quit=false;

    while(!quit){
        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) quit=true;
            if(e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) kEventHandler(e, keypad);
        }
        for(int i = 0; i < ipf; i++){
            cEmulateCycle();
        }
        gDrawFrame(vram);
        if(cHandleTimers() > 1) Beep(440, 30);
        usleep(16670);
    }
    gShutdown();

    return 1;
}
