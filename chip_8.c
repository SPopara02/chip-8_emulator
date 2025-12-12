#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "chip_8.h"

unsigned char memory[4096]; //memory space
unsigned char vram[32][64]; //redesigned vram
bool key_latched = false;
unsigned char key_pressed = 0xFF;
unsigned short stack[16]; //stack
unsigned char reg_V[16]; //data registers V0 to VF
unsigned char reg_sound; //sound register
unsigned char reg_delay; //delay register
unsigned short reg_I; //address register I
unsigned char *pc=NULL;
unsigned short *sp=NULL;
unsigned short memadr=0x0000; //used for evaluating memory addresses
unsigned char byte=0x00; //used for evaluating hex bytes and nibbles
unsigned char keypad[16];

unsigned char fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0,  // 0
    0x20, 0x60, 0x20, 0x20, 0x70,  // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0,  // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0,  // 3
    0x90, 0x90, 0xF0, 0x10, 0x10,  // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0,  // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0,  // 6
    0xF0, 0x10, 0x20, 0x40, 0x40,  // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0,  // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0,  // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90,  // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0,  // B
    0xF0, 0x80, 0x80, 0x80, 0xF0,  // C
    0xE0, 0x90, 0x90, 0x90, 0xE0,  // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0,  // E
    0xF0, 0x80, 0xF0, 0x80, 0x80   // F
};

bool cLoadRom(const char *path){
    bool success=false;
    FILE *fp=fopen(path,"rb");
    if(!fp){
        printf("Error: No such file in directory!\n");
        success = false;
        fclose(fp);
        return success;
    }
    unsigned char rc;
    int i=0x200;
    while(!feof(fp)){
        rc=fgetc(fp);
        *(memory+i)=rc;
        i++;
    }
    *(memory+i-1)=0x00;
    success= true;
    fclose(fp);

    return success;
}

bool cInit(){

    bool success=false;
    for(int i=0;i<16;i++){
        *(reg_V+i)=0x00;
    }
    reg_I=0x0000;
    reg_sound=0x00;
    reg_delay=0x00;
    sp=stack-1;
    pc=(memory+0x200);
    for(int i=0; i<16; i++) keypad[i] = false;
    srand((unsigned int)time(NULL));
    memcpy(memory, fontset, sizeof(fontset));
    success=true;

    return success;
}


int cHandleTimers(){
    if(reg_delay > 0) reg_delay--;
    if(reg_sound > 0) reg_sound--;

    return reg_sound;
}

void opDXYN(unsigned char x, unsigned char y, unsigned char n){
    //wrap the starting coords
    x = reg_V[x]%64;
    y = reg_V[y]%32;
    reg_V[0xF] = 0;
    for(unsigned char i = 0; i < n; i++){
        if(y+i > 31) break; //clip on y if oob
        unsigned char line = *(memory + reg_I + i); //taken from memory
        for(unsigned char j = 0; j < 8; j++){
            if(x+j > 63) break; //clip on x if oob
            unsigned char pixel = ((line << j) & 0x80);
            reg_V[0xF] |= (pixel & vram[y + i][x + j])>>7;
            vram[y + i][x + j] ^= pixel;
        }
    }
}

void cEmulateCycle(){
    //Fetch
    unsigned short opcode=(*pc<<8)|*(pc+1);
    //Decode + Execute
    switch((opcode&0xF000)>>12){
    case 0x0:
        switch(opcode&0x0FFF){
        case 0x0E0:
            for(int i=0; i< 32; i++){
                for(int j=0; j<64; j++){
                    vram[i][j] = 0x0;
                }
            }
            pc += 2;
            break;

        case 0x0EE:
            pc=memory+(*sp);
            sp--;
            break;

        default: printf("Invalid opcode: %x\n",opcode);
        }
        break;

    case 0x1:
        memadr=opcode&0x0FFF;
        pc=memory+memadr;
        break;

    case 0x2:
        memadr=opcode&0x0FFF;
        sp++;
        *sp=pc+2-memory;
        pc=memory+memadr;
        break;

    case 0x3:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            byte=(opcode&0x00FF);
            if(reg_V[x]==byte){
                pc+=4;
            }
            else{
                pc+=2;
            }
        }
        break;

    case 0x4:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            byte=(opcode&0x00FF);
            if(reg_V[x]!=byte){
                pc+=4;
            }
            else{
                pc+=2;
            }
        }
        break;

    case 0x5:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            unsigned char y=(opcode&0x00F0)>>4;
            if(reg_V[x]==reg_V[y]){
                pc+=4;
            }
            else{
                pc+=2;
            }
        }
        break;

    case 0x6:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            byte=(opcode&0x00FF);
            reg_V[x]=byte;
            pc+=2;
        }
        break;

    case 0x7:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            byte=(opcode&0x00FF);
            reg_V[x]+=byte;
            pc+=2;
        }
        break;

    case 0x8:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            unsigned char y=(opcode&0x00F0)>>4;
            switch(opcode&0x000F){
            case 0x0:
                reg_V[x]=reg_V[y];
                break;
            case 0x1:
                reg_V[x]=reg_V[x]|reg_V[y];
                break;
            case 0x2:
                reg_V[x]=reg_V[x]&reg_V[y];
                break;
            case 0x3:
                reg_V[x]=reg_V[x]^reg_V[y];
                break;
            case 0x4:
                {
                    unsigned char tmp=reg_V[x];
                    reg_V[x]+=reg_V[y];
                    reg_V[0xF]=(reg_V[x]<tmp||reg_V[x]<reg_V[y])?0x1:0x0;
                }
                break;
            case 0x5:
                {
                    unsigned char tmp=reg_V[x];
                    reg_V[x]-=reg_V[y];
                    reg_V[0xF]=tmp<reg_V[y]?0x0:0x1;
                }
                break;
            case 0x6:
                {
                    unsigned char tmp=reg_V[y];
                    reg_V[x]=reg_V[y]>>1;
                    reg_V[0xF]=tmp&0x1;
                }
                break;
            case 0x7:
                {
                    unsigned char tmp=reg_V[x];
                    reg_V[x]=reg_V[y]-reg_V[x];
                    reg_V[0xF]=tmp>reg_V[y]?0x0:0x1;
                }

                break;
            case 0xE:
                {
                    unsigned char tmp=reg_V[y];
                    reg_V[x]=reg_V[y]<<1;
                    reg_V[0xF]=(tmp&0x80)>>7;
                }
                break;
            }
            pc+=2;
        }
        break;

    case 0x9:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            unsigned char y=(opcode&0x00F0)>>4;
            if(reg_V[x]!=reg_V[y]){
                pc+=4;
            }
            else{
                pc+=2;
            }
        }
        break;

    case 0xA:
        memadr=(opcode&0x0FFF);
        reg_I=memadr;
        pc+=2;
        break;

    case 0xB:
        memadr=(opcode&0x0FFF);
        pc=memory+memadr+reg_V[0x0];
        break;

    case 0xC:
        {
            unsigned char x = (opcode&0x0F00) >> 8;
            byte = (opcode&0x00FF);
            reg_V[x] = (rand() % 256) & byte;
            pc += 2;
        }
        break;

    case 0xD:
            {
                unsigned char x=(opcode&0x0F00)>>8;
                unsigned char y=(opcode&0x00F0)>>4;
                byte=(opcode&0x000F);
                opDXYN(x,y,byte);
                pc+=2;
            }
            break;

    case 0xE:
        {
            unsigned char x=(opcode&0x0F00)>>8;
            switch(opcode&0x00FF){
            case 0x9E:
                if(keypad[reg_V[x]]) pc+=4;
                else pc+=2;
                break;
            case 0xA1:
                if(!keypad[reg_V[x]]) pc+=4;
                else pc+=2;
                break;
            default:
                printf("Invalid opcode!");

            }
        }
        break;

    case 0xF:
        {
                unsigned char x=(opcode&0x0F00)>>8;
                switch(opcode&0x00FF){
                case 0x07:
                    reg_V[x]=reg_delay;
                    pc += 2;
                    break;
                case 0x0A:
                    {
                        //poll if latch is open
                        if(!key_latched){
                            for(int i = 0; i < 16; i++){  //cycle the keypad and look for a pressed key
                                if(keypad[i]){  //if found, assign its index to keypress var
                                    key_pressed = i;
                                    key_latched = true;
                                    break;
                                }
                            }
                        }
                        else{
                            if(!keypad[key_pressed]){ //on release, assign to register, reset keypress to default and increment pc, else stall
                                reg_V[x] = key_pressed;
                                key_pressed = 0xFF;
                                key_latched = false;
                                pc += 2;
                            }
                        }
                    }
                    break;
                case 0x15:
                    reg_delay=reg_V[x];
                    pc += 2;
                    break;
                case 0x18:
                    reg_sound=reg_V[x];
                    pc += 2;
                    break;
                case 0x1E:
                    reg_I+=reg_V[x];
                    pc += 2;
                    break;
                case 0x29:
                    reg_I = reg_V[x] * 5;
                    pc +=2;
                    break;
                case 0x33:
                    *(memory+reg_I)=(reg_V[x]/100)%10;
                    *(memory+reg_I+1)=(reg_V[x]/10)%10;
                    *(memory+reg_I+2)=reg_V[x]%10;
                    pc += 2;
                    break;
                case 0x55:
                    for(int i=0;i<=x;i++){
                        *(memory+reg_I+i)=reg_V[i];
                    }
                    reg_I+=x+1;
                    pc += 2;
                    break;
                case 0x65:
                    for(int i=0;i<=x;i++){
                        reg_V[i]=*(memory+reg_I+i);
                    }
                    reg_I+=x+1;
                    pc += 2;
                    break;
                default:
                    printf("Invalid opcode: %x\n",opcode);
                }
        }
        break;

    default: printf("Invalid opcode: %x\n",opcode);
    }
}

