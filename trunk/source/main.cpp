// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 3.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 3.0 for more details.

// Copyright (C) 2012-2013      filfat, xerpi, JoostinOnline

#include <gccore.h>
#include <wiiuse/wpad.h>
#include <wiikeyboard/keyboard.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fat.h>
#include <sdcard/wiisd_io.h>
#include <png.h>
#include <pngu.h>
//#include <GEMS_WifiWii.h>

#include "grass_png.h"
#include "dirt_png.h"
#include "stone_png.h"

//Classes:
#include "debug.h"
#include "mainAPI.h"
#include "init.h"
#include "World.hpp"
#include "Camera.hpp"
#include "Player.hpp"
#include "World.hpp"
#include "Image.hpp"
#include "map.h"
#include "debug.h"
#include "main.h"
#include "utils.h"
#include "devicemounter.h"

//Font:
#include <ft2build.h>
#include FT_FREETYPE_H
#include "rursus_compact_mono_ttf.h"
//Color:
#define	BLACK	"\x1b[30;1m"
#define	RED	"\x1b[31;1m"
#define	GREEN	"\x1b[32;1m"
#define	YELLOW	"\x1b[33;1m"
#define BLUE    "\x1b[34;1m"
#define MAGENTA "\x1b[35;1m"
#define CYAN    "\x1b[36;1m"
#define	WHITE	"\x1b[37;1m"
#define CHANGE_COLOR(X)	(printf((X)))

extern "C" {
        extern void __exception_setreload(int t);
}


//const unsigned int LOCAL_PLAYERS = 4; //i dont think the wii can handle 4 players but who knows ;)
//const unsigned int ONLINE_PLAYERS = 8; //due ram; maybe can increes that later.
//const unsigned int PORT = 8593; //PORT

int fatDevice = FAT_DEVICE_NONE;

        //--------------------------------------------------
        //Wiilight
        //--------------------------------------------------
        lwp_t light_thread = 0;
        void *light_loop (void *arg);
        //vu32 *light_reg = (u32*) HW_GPIO;
        bool light_on = false;
        u8 light_level = 0;
        struct timespec light_timeon = { 0 };
        struct timespec light_timeoff = { 0 };

        //----------------------------------------------------------
        //Function's:
        //----------------------------------------------------------

        void WIILIGHT_TurnOn(){
        *(u32*)0xCD0000C0 |= 0x20;
}

        void WIILIGHT_TurnOff(){
        *(u32*)0xCD0000C0 &= ~0x20;
}

void WIILIGHT_SetLevel(int level){
        light_level = MIN(MAX(level, 0), 100);
        // Calculate the new on/off times for this light intensity
        u32 level_on;
        u32 level_off;
        level_on = (light_level * 2.55) * 40000;
        level_off = 10200000 - level_on;
        light_timeon.tv_nsec = level_on;
        light_timeoff.tv_nsec = level_off;
}

World world;
void UpdateCamera();
void MoveCamera();
int Menu(int texture);

int main(int argc, char **argv)
{       
        // In the event of a code dump, the app will exit after 10 seconds (unless you press POWER)
        __exception_setreload(10);
        Initialize();
        fatMountSimple("sd", &__io_wiisd);
        #ifdef USBGECKO
        Debug("fatMountSimple() Done");
        #endif
        
        //Disk light turn on and init
        //WIILIGHT_Init();
        WIILIGHT_SetLevel(255);
        WIILIGHT_TurnOn();
        
        //MAP mainMAP;
        
        fatDevice = FAT_DEVICE_SD;
        
        //#ifdef USBGECKO
        //Debug("The User Has a %s", fatDevice); for some reason it does not work.
        //#endif
        
        //API:
        //API mainAPI;
        //#ifdef USBGECKO
        //Debug("mainAPI() Done");
        //#endif
        //mainAPI.initAPI();
        //#ifdef USBGECKO
        //Debug("initAPI() Done");
        //#endif
        bool debugText = false;
        int texture = 1;
        bool mainGame = true;
        
        Image grass((uint8_t *)grass_png);
        Image dirt((uint8_t *)dirt_png);
        Image stone((uint8_t *)stone_png);
        #ifdef USBGECKO
        Debug("Image() Done");
        #endif
        
        //ADD OTHER INIT'S HERE
        
        #ifdef USBGECKO
        Debug("All Inits is Done");
        #endif
		
		//Menu
		s8 selected = 1; // Make sure that "selected" is defined as 1-6
		goto mainMenu;
	mainMenu:selected = 1;
	while(true) {
		//VIDEO_ClearFrameBuffer(rmode,xfb[fb],COLOR_BLACK);
		printf("\x1b[2;0H"); // This resets the position of the console
		CHANGE_COLOR(GREEN);
		printf("WiiCraft %s\n", "6.3");
		CHANGE_COLOR(WHITE);
        printf("========[Menu]========\n");
		MENU("Singelplayer                 ", 1); // MENU(description, option number)
		MENU("Swap Texture's               ", 2);
		MENU("Exit", 3);
		do {pressed = DetectInput(DI_BUTTONS_DOWN);} while(!pressed);
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {
			selected++;
			if (selected>3) selected = 1;
			continue;
		}
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) {
			selected--;
			if (selected<1) selected = 3;
			continue;
		}
		if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
			if(selected == 1){
				goto mainGame;
			}
			if(selected == 2){	
				selected = 1;
				while(true) {
					printf("\x1b[2;0H"); // This resets the position of the console
					CHANGE_COLOR(GREEN);
					printf("WiiCraft %s\n", "6.3");
					CHANGE_COLOR(WHITE);
					printf("========[Menu]========\n");
					MENU("Set Texture To Stone", 1); // MENU(description, option number)
					MENU("Set Texture To Grass", 2);
					MENU("Back", 3);
					do {pressed = DetectInput(DI_BUTTONS_DOWN);} while(!pressed);
					if (WPAD_ButtonsDown(0) & WPAD_BUTTON_DOWN) {
						selected++;
						if (selected>3) selected = 1;
						continue;
					}
					if (WPAD_ButtonsDown(0) & WPAD_BUTTON_UP) {
						selected--;
						if (selected<1) selected = 3;
						continue;
					}
					if (WPAD_ButtonsDown(0) & WPAD_BUTTON_A) {
						if(selected == 1) {
							texture = 1;
							continue;
						}
						if(selected == 2) {
							texture = 2;
							continue;
						}
						if(selected == 3) {
							goto mainMenu;
						}
					}
				}
			}
			if(selected == 3){
				goto EXIT;
			}
		}
	}
		
	
        
  /*----------------------------------------<Main Game Loop>-----------------------------------------*/
        mainGame:while(mainGame == true){
                UpdatePad();
                printf("\x1b[%d;%dH", 2, 0);
                printf("Press + For Pause Menu");
                if(debugText == true){
                        //GX_SetViewport(0, 0, rmode->fbWidth, rmode->efbHeight, 0, 1);
                        VIDEO_ClearFrameBuffer(rmode,xfb[fb],COLOR_BLACK);
                        printf("\x1b[%d;%dH", 2, 0);
                        printf("FPS: %f\n", fps);
                        printf("PosX: %f   PosY: %f  PosZ: %f\n", world.player->position.x, world.player->position.y, world.player->position.z);
                        printf("Pitch: %f   Yaw: %f\n", world.player->pitch, world.player->yaw);
                        printf("Size: %i\n", world.chunkHandler->chunkList.size());
                        printf("ChunkX: %i  ChunkY: %i  ChunkZ: %i\n", world.player->chunk_x, world.player->chunk_y, world.player->chunk_z);
                        printf("BlockX: %i  BlockY: %i  BlockZ: %i\n", world.player->block_x, world.player->block_y, world.player->block_z);    
                        printf("Player Status: %s  Velocity.y: %f\n", world.player->status == ON_AIR ? "AIR" : "GROUND",world.player->velocity.y);
                }
                switch(texture){
                        case 0:
                                break;
                        case 1:
                                stone.setGX(GX_TEXMAP0);
                                break;
                        case 2:
                                grass.setGX(GX_TEXMAP0);
                                break;
                        case 3:
                                dirt.setGX(GX_TEXMAP0);
                                break;
                        default:
                                break;
                }
                DrawCubeTex(0,0,-5);

                world.update();
                world.drawChunks();

                MoveCamera();
                UpdateCamera();
                SwapBuffer();
                FPS(&fps);
                if (pressed & WPAD_BUTTON_PLUS ){
                        /*printf("========[Pause]========\n");
                        printf("[*]A: Resume\n");
                        printf("[*]Home: Quit to Menu\n");
                        while(1){
                                if(pressed & WPAD_BUTTON_A){
                                        break;
                                }
                                else if(pressed & WPAD_BUTTON_HOME){*/
                                        printf("\x1b[2;0H");
                                        VIDEO_ClearFrameBuffer(rmode,xfb[fb],COLOR_BLACK);
                                        //--------
                                        #ifdef USBGECKO
                                        Debug("--<Main Loop Exited>--");
                                        #endif
                                        goto mainMenu;
                                /*}
                        }
                        continue;*/
                }
                if ((pressed & WPAD_BUTTON_UP) && (pressed & WPAD_BUTTON_A) && (pressed & WPAD_BUTTON_B)) debugText = !debugText;
                VIDEO_WaitVSync();
        }

        EXIT:printf("\x1b[2;0H");
        VIDEO_ClearFrameBuffer(rmode,xfb[fb],COLOR_BLACK);
        SwapBuffer();
        Deinitialize();
        exit(0);
		
        #ifdef USBGECKO
        Debug("ERROR: PASSED LAST LINE OF CODE!");
        #endif
        exit(-1);
}
/*----------------------------------------<End Of Main Game Loop>-----------------------------------------*/





void MoveCamera()
{
        /*if(expansion_type == WPAD_EXP_NUNCHUK)
        {
                if(js->mag >= 0.2f)
                {
                        float n_angle = DegToRad(NunchukAngle(js->ang));
                        world.player->move( cos(n_angle)*js->mag / 5.0f, world.player->camera->getRightVector());
                        world.player->move(-sin(n_angle)*js->mag / 5.0f, world.player->camera->getForwardVector());     
                }
        }*/
        Chunk *cp;
        Block *bp;
        
                /*cp = chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
                cp = cp->rightNeighbour;
                bp = cp->blockList[3][0][0];
                bp->transparent = true;
                cp->needsUpdate = true;*/
        
        if(pressed & WPAD_BUTTON_A)
        {
                cp = world.chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
                bp = cp->blockList[world.player->block_z][world.player->block_y][world.player->block_x];
                bp->transparent = true;
                cp->needsUpdate = true;
                if(world.player->block_x == 0)
                        world.chunkHandler->setChunkToUpdate(cp->leftNeighbour);
                if(world.player->block_x == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->rightNeighbour);
                if(world.player->block_y == 0)
                        world.chunkHandler->setChunkToUpdate(cp->downNeighbour);
                if(world.player->block_y == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->upNeighbour);
                if(world.player->block_z == 0)
                        world.chunkHandler->setChunkToUpdate(cp->backNeighbour);
                if(world.player->block_z == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->frontNeighbour);
        }


        if(pressed & WPAD_BUTTON_B)
        {
                cp = world.chunkHandler->chunkList[getWorldIndex(world.player->chunk_x, world.player->chunk_y, world.player->chunk_z)];
                bp = cp->blockList[world.player->block_z][world.player->block_y][world.player->block_x];
                bp->transparent = false;
                cp->needsUpdate = true;
                if(world.player->block_x == 0)
                        world.chunkHandler->setChunkToUpdate(cp->leftNeighbour);
                if(world.player->block_x == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->rightNeighbour);
                if(world.player->block_y == 0)
                        world.chunkHandler->setChunkToUpdate(cp->downNeighbour);
                if(world.player->block_y == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->upNeighbour);
                if(world.player->block_z == 0)
                        world.chunkHandler->setChunkToUpdate(cp->backNeighbour);
                if(world.player->block_z == (CHUNK_SIZE-1))
                        world.chunkHandler->setChunkToUpdate(cp->frontNeighbour);
        }
        
        
        /*if(pressed & WPAD_NUNCHUK_BUTTON_C)
                world.player->position.y += 0.25f;
        if(pressed & WPAD_NUNCHUK_BUTTON_Z)
                world.player->position.y -= 0.25f;
                
        if(pressed & WPAD_BUTTON_1)
                world.player->position.y += 0.25f;
        if(pressed & WPAD_BUTTON_2)
                world.player->position.y -= 0.25f;
                
        if(pressed & WPAD_BUTTON_UP)
                world.player->pitch += 0.02f;
        if(pressed & WPAD_BUTTON_DOWN)
                world.player->pitch -= 0.02f;
                
        if(pressed & WPAD_BUTTON_RIGHT)
                world.player->yaw -= 0.02f;
        if(pressed & WPAD_BUTTON_LEFT)
                world.player->yaw += 0.02f;*/
                
}


void UpdateCamera()
{
        guMtxConcat(world.getCameraView(), model, modelview);
        GX_LoadPosMtxImm(modelview, GX_PNMTX0);
}


int Menu(int texture){return false;
}