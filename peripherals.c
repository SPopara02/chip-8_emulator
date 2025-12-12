#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include "peripherals.h"

const int screen_width=640;
const int screen_height=320;
SDL_Window *window=NULL;
SDL_Renderer *gRenderer=NULL;

bool gInit(){
    bool success=true;
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf("SDL failed to initialize! %s \n",SDL_GetError());
        success=false;
    }
    else{
        window=SDL_CreateWindow("CHIP-8",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,screen_width,screen_height,SDL_WINDOW_SHOWN);
        if(window==NULL){
            printf("Failed to create window! %s \n",SDL_GetError());
            success=false;
        }
        else{
            gRenderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if(gRenderer==NULL){
                printf("Failed to create renderer! %s \n",SDL_GetError());
                success=false;
            }
            else{
                SDL_SetRenderDrawColor(gRenderer,0x0,0x0,0x0,0xFF);
                SDL_RenderClear(gRenderer);
            }
        }
    }
    return success;
}

void gShutdown(){
    SDL_DestroyWindow(window);
    window=NULL;
    SDL_DestroyRenderer(gRenderer);
    gRenderer=NULL;
}

void gDrawFrame(unsigned char vram[32][64]){
    gClearScreen();
    for(int i=0;i<32;i++){
        unsigned char tile=0x0;
        for(int j=0;j<64;j++){
            tile=vram[i][j];
            unsigned char x_off=0;
            if(tile==0x80){
                SDL_Rect pixel={10*j+x_off,10*i,10,10};
                SDL_SetRenderDrawColor(gRenderer,0xFF,0xFF,0xFF,0xFF);
                SDL_RenderFillRect(gRenderer,&pixel);
            }
            x_off+=10;
        }
    }
    SDL_RenderPresent(gRenderer);
}

void gClearScreen(){
    SDL_SetRenderDrawColor(gRenderer,0x0,0x0,0x0,0x255);
    SDL_RenderClear(gRenderer);
}

void kEventHandler(SDL_Event e,unsigned char* keypad){
    if(e.type == SDL_KEYDOWN){
        switch(e.key.keysym.sym){
        case SDLK_1:
            *(keypad + 0x1) = true;
            break;
        case SDLK_2:
            *(keypad + 0x2) = true;
            break;
        case SDLK_3:
            *(keypad + 0x3) = true;
            break;
        case SDLK_4:
            *(keypad + 0xC) = true;
            break;
        case SDLK_q:
            *(keypad + 0x4) = true;
            break;
        case SDLK_w:
            *(keypad + 0x5) = true;
            break;
        case SDLK_e:
            *(keypad + 0x6) = true;
            break;
        case SDLK_r:
            *(keypad + 0xD) = true;
            break;
        case SDLK_a:
            *(keypad + 0x7) = true;
            break;
        case SDLK_s:
            *(keypad + 0x8) = true;
            break;
        case SDLK_d:
            *(keypad + 0x9) = true;
            break;
        case SDLK_f:
            *(keypad + 0xE) = true;
            break;
        case SDLK_z:
            *(keypad + 0xA) = true;
            break;
        case SDLK_x:
            *(keypad + 0x0) = true;
            break;
        case SDLK_c:
            *(keypad + 0xB) = true;
            break;
        case SDLK_v:
            *(keypad + 0xF) = true;
            break;
        }
    }
    else{
        switch(e.key.keysym.sym){
        case SDLK_1:
            *(keypad + 0x1) = false;
            break;
        case SDLK_2:
            *(keypad + 0x2) = false;
            break;
        case SDLK_3:
            *(keypad + 0x3) = false;
            break;
        case SDLK_4:
            *(keypad + 0xC) = false;
            break;
        case SDLK_q:
            *(keypad + 0x4) = false;
            break;
        case SDLK_w:
            *(keypad + 0x5) = false;
            break;
        case SDLK_e:
            *(keypad + 0x6) = false;
            break;
        case SDLK_r:
            *(keypad + 0xD) = false;
            break;
        case SDLK_a:
            *(keypad + 0x7) = false;
            break;
        case SDLK_s:
            *(keypad + 0x8) = false;
            break;
        case SDLK_d:
            *(keypad + 0x9) = false;
            break;
        case SDLK_f:
            *(keypad + 0xE) = false;
            break;
        case SDLK_z:
            *(keypad + 0xA) = false;
            break;
        case SDLK_x:
            *(keypad + 0x0) = false;
            break;
        case SDLK_c:
            *(keypad + 0xB) = false;
            break;
        case SDLK_v:
            *(keypad + 0xF) = false;
            break;
        }
    }
}
