// Author : Jean-Baptiste PERIN
// Date : 2021
//
// Build with OSDK: osdk_build.bat && osdk_execute.bat
//

#undef DEBUG

#include "lib.h"

#include "config.h"

#include "raycast.c"
#include "scene.c"

#include "game.c"

#include "player.c"

#include "texel.c"

#include "drawWalls.c"

#ifdef USE_SPRITE
#include "sprite.c"
#include "texture_key.h"
#include "texture_soldier_back_00.h"
#include "texture_soldier_front_00.h"
#include "texture_soldier_left_00.h"
#include "texture_soldier_right_00.h"
#endif

#define CHANGE_INK_TO_BLACK	            0
#define CHANGE_INK_TO_RED	            1		
#define CHANGE_INK_TO_GREEN	            2		
#define CHANGE_INK_TO_BLUE	            4	 

#include "dichobuf.c"
#include "dist.c"
#include "engine.c"

void prepareRGB(){
    int ii;

    // parcours de lignes de 3 en 3
    for (ii=0; ii < (TEXT_SCREEN_HEIGHT - NB_LESS_LINES_4_COLOR)*8;  ii+=3){
        poke (HIRES_SCREEN_ADDRESS+((ii)*NEXT_SCANLINE_INCREMENT),CHANGE_INK_TO_RED);
        poke (HIRES_SCREEN_ADDRESS+((ii+1)*NEXT_SCANLINE_INCREMENT),CHANGE_INK_TO_GREEN);
        poke (HIRES_SCREEN_ADDRESS+((ii+2)*NEXT_SCANLINE_INCREMENT),CHANGE_INK_TO_BLUE);
    }
}


// [ref camera_situation]
void initCamera(){
    rayCamPosX               = 0;
    rayCamPosY               = 0; 
    rayCamRotZ               = 64;
    RayLeftAlpha            = rayCamRotZ + HALF_FOV_FIX_ANGLE;
}


#ifdef DEBUG
#include "debug.c"
#endif

#include "viewport.c"


void gameLoop() {
    int ii;
    engInitObjects();
    engAddObject(OBJ_KEY, 3, 3, 0);
    engAddObject(OBJ_SOLDIER, 1, 2, soldier_data);

    while (running) {

        doke(630,0);

        if ((doorState==1) && (scene_00[2+4*2] != 6)) {
            scene_00[2+4*2] ++;
            scene_00[2+5*2] ++;
            if (scene_00[2+4*2] == 6) doorState = 0;
            initScene (scene_00, texture_00);
            refreshNeeded = 1;
        } else {
            player ();
        }

        dichoInit();
        engPulse();



        if (refreshNeeded) {
            rayInitCasting();

            rayProcessPoints();
            rayProcessWalls();

            // clearViewport();
            drawWalls();
#ifdef USE_SPRITE
            // logdist(signed char posX, signed char posY) 
            if ((rayCamPosX == 3) && (rayCamPosY == 3)){
                if (hasKey == 0) {
                    zap();
                }
                hasKey = 1;
            }
            // if (! hasKey) drawSprite (3, 3, texture_aKey);
#endif
            refreshNeeded = 0;
            // printf("\n(X=%d Y=%d) [a=%d] [t=%d]\n\n", rayCamPosX, rayCamPosY, rayCamRotZ, 65535-deek(630));
            // if (hasKey) printf ("Key");
        }      
        ii= dichoNbVal;
        while (ii >0) {
            ii -= 1;
            // printf("%d\t", tabDichoIdxs[ii]);
            drawSprite (objPosX[tabDichoIdxs[ii]], objPosY[tabDichoIdxs[ii]], objTexture[tabDichoIdxs[ii]]);
        }

    }
}

void main(){
    int ii, jj;
    // printf ("DEBUT\n");

    // [res camera_situation]]
    initCamera();
    
    // [ref scene_load]
    initScene (scene_00, texture_00);
    hasKey = 0;
    doorState == 2;
#ifdef DEBUG

    rayInitCasting();
    rayProcessPoints();
    rayProcessWalls();

    detailPoints(); 
    get();
    textZBuffer ();
    get();
    textCol ();
    get();
#endif

    hires();
    paper(4);
    // ink(7);
    prepareRGB();
    *((unsigned char *)0x26A)= 10;
    *((unsigned char *)49000)= 0x14;
    *((unsigned char *)49040)= 0x14;
    *((unsigned char *)49080)= 0x14;
    *((unsigned char *)49001)= 7;
    *((unsigned char *)49041)= 7;
    *((unsigned char *)49081)= 7;
    *((unsigned char *)49043)= 10;
    *((unsigned char *)49083)= 10;
    *((unsigned char *)49004)= 124;
    *((unsigned char *)49044)= 124;
    *((unsigned char *)49084)= 124;
    *((unsigned char *)49007)= 80;
    *((unsigned char *)49008)= 69;
    *((unsigned char *)49009)= 65;
    *((unsigned char *)49010)= 67;
    *((unsigned char *)49011)= 69;
    *((unsigned char *)49012)= 20;
    *((unsigned char *)49048)= 49;
    *((unsigned char *)49088)= 49;
    *((unsigned char *)49049)= 48;
    *((unsigned char *)49089)= 48;
    *((unsigned char *)49050)= 48;
    *((unsigned char *)49090)= 48;
    *((unsigned char *)49051)= 37;
    *((unsigned char *)49091)= 37;
    *((unsigned char *)49014)= 124;
    *((unsigned char *)49054)= 124;
    *((unsigned char *)49094)= 124;
    *((unsigned char *)49015)= 9;
    *((unsigned char *)49055)= 9;
    *((unsigned char *)49095)= 9;
    *((unsigned char *)49057)= 65;
    *((unsigned char *)49058)= 50;
    *((unsigned char *)49097)= 40;
    *((unsigned char *)49098)= 36;
    *((unsigned char *)49099)= 32;
    *((unsigned char *)49020)= 8;
    *((unsigned char *)49060)= 10;
    *((unsigned char *)49100)= 10;
    *((unsigned char *)49021)= 124;
    *((unsigned char *)49061)= 124;
    *((unsigned char *)49101)= 124;
    *((unsigned char *)49023)= 76;
    *((unsigned char *)49024)= 79;
    *((unsigned char *)49025)= 86;
    *((unsigned char *)49026)= 69;
    *((unsigned char *)49065)= 49;
    *((unsigned char *)49105)= 49;
    *((unsigned char *)49066)= 50;
    *((unsigned char *)49106)= 50;
    *((unsigned char *)49028)= 124;
    *((unsigned char *)49068)= 124;
    *((unsigned char *)49108)= 124;
    *((unsigned char *)49030)= 70;
    *((unsigned char *)49031)= 76;
    *((unsigned char *)49032)= 79;
    *((unsigned char *)49033)= 87;
    *((unsigned char *)49034)= 69;
    *((unsigned char *)49035)= 82;
    *((unsigned char *)49072)= 51;
    *((unsigned char *)49112)= 51;
    *((unsigned char *)49036)= 124;
    *((unsigned char *)49076)= 124;
    *((unsigned char *)49116)= 124;

#ifdef DEBUG
    drawWalls();
    // drawSprite (6, 6, texture_pillar);
#endif
    running = 1;
    refreshNeeded           = 1;
#define BORDER_COLOR 10
    for (ii=VIEWPORT_START_COLUMN-1; ii<= VIEWPORT_WIDTH+VIEWPORT_START_COLUMN; ii++){
        drawTexelOnScreen (VIEWPORT_START_LINE-2, ii, BORDER_COLOR);
        drawTexelOnScreen (VIEWPORT_START_LINE-1, ii, BORDER_COLOR);
        drawTexelOnScreen (VIEWPORT_START_LINE+VIEWPORT_HEIGHT, ii, BORDER_COLOR);
        drawTexelOnScreen (VIEWPORT_START_LINE+VIEWPORT_HEIGHT+1, ii, BORDER_COLOR);
    }
    for (ii=VIEWPORT_START_LINE; ii< VIEWPORT_HEIGHT+VIEWPORT_START_LINE; ii++){
        drawTexelOnScreen (ii, VIEWPORT_START_COLUMN-1, BORDER_COLOR);
        drawTexelOnScreen (ii, VIEWPORT_START_COLUMN, BORDER_COLOR);
        drawTexelOnScreen (ii, VIEWPORT_START_COLUMN+VIEWPORT_WIDTH, BORDER_COLOR);
        drawTexelOnScreen (ii, VIEWPORT_START_COLUMN+VIEWPORT_WIDTH-1, BORDER_COLOR);
    }
    // poke (0xBBA3, CHANGE_INK_TO_BLACK);
    // sprintf ((void *)(0xBB80+24*40), "LIFE AMMO");//"\033DLIFE      AMMO \033G");
    // sprintf ((void *)(0xBB80+25*40), "LIFE AMMO");//"\033DLIFE      AMMO \033G");
    // sprintf ((void *)(0xBB80+26*40), "\012\004  100 10");
    // sprintf ((void *)(0xBB80+27*40), "\012\004  100 10");
    

    gameLoop();
	
}

// #define LORES_SCREEN_ADDRESS            ((unsigned int)0xBB80)
// #define STANDARD_CHARSET_ADDRESS        ((unsigned int)0xB400)
// #define ALTERNATE_CHARSET_ADDRESS       ((unsigned int)0xB800)
// #define STANDARD_HIRES_CHARSET_ADDRESS  ((unsigned int)0x9800)

// #define CHANGE_INK_TO_BLACK	            0		
// #define CHANGE_INK_TO_YELLOW	        3		
// #define CHANGE_INK_TO_MAGENTA           5			
// #define CHANGE_INK_TO_CYAN	            6		
// #define CHANGE_INK_TO_WHITE	            7	


// #define USE_STANDARD_CHARSET    	                    8		
// #define USE_ALTERNATE_CHARSET	                        9		
// #define USE_DOUBLE_SIZE_STANDARD_CHARSET	            10		
// #define USE_DOUBLE_SIZE_ALTERNATE_CHARSET	            11		
// #define USE_DOUBLE_SIZE_BLINKING_STANDARD_CHARSET       12		
// #define USE_DOUBLE_SIZE_BLINKING_ALTERNATE_CHARSET      13		
// #define USE_BLINKING_STANDARD_CHARSET	                14		
// #define USE_BLINKING_ALTERNATE_CHARSET	                15		
// // Change Paper (background) color	
// #define CHANGE_PAPER_TO_BLACK			                16		
// #define CHANGE_PAPER_TO_RED	                            17		
// #define CHANGE_PAPER_TO_GREEN	                        18		
// #define CHANGE_PAPER_TO_YELLOW	                        19		
// #define CHANGE_PAPER_TO_BLUE	                        20		
// #define CHANGE_PAPER_TO_MAGENTA	                        21		
// #define CHANGE_PAPER_TO_CYAN	                        22		
// #define CHANGE_PAPER_TO_WHITE	                        23		
// // Video control attributes	
// #define SWITCH_TO_TEXT_MODE_60HZ		                24		
// #define SWITCH_TO_TEXT_MODE_50HZ		                26		
// #define SWITCH_TO_HIRES_MODE_60HZ	                    28		
// #define SWITCH_TO_HIRES_MODE_50HZ                       30		

// /*
//  * VISIBILITY LIMITS
//  */
// #define ANGLE_MAX 0xC0
// #define ANGLE_VIEW 0xE0


// unsigned int multi40[] = {
//         0, 40, 80, 120, 160, 200, 240, 280, 320, 360, 400, 440, 480, 520, 560, 600
//         , 640, 680, 720, 760, 800, 840, 880, 920, 960, 1000, 1040, 1080, 1120, 1160, 1200, 1240
//         // , 1280, 1320, 1360, 1400, 1440, 1480, 1520, 1560, 1600, 1640, 1680, 1720, 1760, 1800, 1840, 1880
//         // , 1920, 1960, 2000, 2040, 2080, 2120, 2160, 2200, 2240, 2280, 2320, 2360, 2400, 2440, 2480, 2520
//         // , 2560, 2600, 2640, 2680, 2720, 2760, 2800, 2840, 2880, 2920, 2960, 3000, 3040, 3080, 3120, 3160
//         // , 3200, 3240, 3280, 3320, 3360, 3400, 3440, 3480, 3520, 3560, 3600, 3640, 3680, 3720, 3760, 3800
//         // , 3840, 3880, 3920, 3960, 4000, 4040, 4080, 4120, 4160, 4200, 4240, 4280, 4320, 4360, 4400, 4440
//         // , 4480, 4520, 4560, 4600, 4640, 4680, 4720, 4760, 4800, 4840, 4880, 4920, 4960, 5000, 5040, 5080
//         // , 5120, 5160, 5200, 5240, 5280, 5320, 5360, 5400, 5440, 5480, 5520, 5560, 5600, 5640, 5680, 5720
//         // , 5760, 5800, 5840, 5880, 5920, 5960, 6000, 6040, 6080, 6120, 6160, 6200, 6240, 6280, 6320, 6360
//         // , 6400, 6440, 6480, 6520, 6560, 6600, 6640, 6680, 6720, 6760, 6800, 6840, 6880, 6920, 6960, 7000
//         // , 7040, 7080, 7120, 7160, 7200, 7240, 7280, 7320, 7360, 7400, 7440, 7480, 7520, 7560, 7600, 7640
//         // , 7680, 7720, 7760, 7800, 7840, 7880, 7920, 7960
//         };

// unsigned int multi27[] = {
//         0, 27, 54, 81, 108, 135, 162, 189, 216, 243, 270, 297, 324, 351, 378, 405
//         , 432, 459, 486, 513, 540, 567, 594, 621, 648, 675, 702, 729, 756, 783, 810, 837
//         , 864, 891, 918, 945, 972, 999, 1026, 1053};


// unsigned char idxWall[] = {
//         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//         , 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
//         , 0, 0, 0, 0, 0, 0, 0, 0};

// line in [0..65] column in [0..79]
// void colorSquare(unsigned char line, unsigned char column, unsigned char theColor){

//     unsigned char r, g, b;
//     unsigned char *adr;

//     PROFILE_ENTER(ROUTINE_COLORSQUARE);
//     // retrieve the color components from the color value
//     r = (theColor>>4)& 0x03;
//     g = (theColor>>2)& 0x03;
//     b = (theColor)& 0x03;

//     // compute the start adress of the screen square to color
//     //adr = (unsigned char *)(HIRES_SCREEN_ADDRESS + (line*3)*NEXT_SCANLINE_INCREMENT + (column>>1));
//     adr = (unsigned char *)(HIRES_SCREEN_ADDRESS + multi40[(line<<1) + line] + (column>>1));

//     if ((column&0x01) == 0){
//         *adr |= encodeHColor[r];
//         adr += NEXT_SCANLINE_INCREMENT;
//         *adr |= encodeHColor[g];
//         adr += NEXT_SCANLINE_INCREMENT;
//         *adr |= encodeHColor[b];
//     } else {
//         *adr |= encodeLColor[r];
//         adr += NEXT_SCANLINE_INCREMENT;
//         *adr |= encodeLColor[g];
//         adr += NEXT_SCANLINE_INCREMENT;
//         *adr |= encodeLColor[b];
//     }

//     PROFILE_LEAVE(ROUTINE_COLORSQUARE);
// }

// void drawImage01(){
//     int ii, jj;

//     signed char idxScreenLine, idxScreenCol;
//     unsigned char height, texcolumn;

//     PROFILE_ENTER(ROUTINE_DRAW01);
//     for (ii = 0; ii < 40; ii++) {
//         height          = tabHeight[ii];
//         texcolumn       = tabTexCol[ii];
//         idxScreenCol    = 10+ii;

//         ddaStartValue       = 0;
//         ddaNbStep           = 2*height;
//         ddaNbVal            = TEXTURE_SIZE;

//         ddaInit();
//         idxScreenLine       = 32 - height;
//         // printf ("%d %d\n", ddaCurrentValue, idxScreenLine);
//         if ((idxScreenLine >=0) && (idxScreenLine < 64)) {
//             colorSquare(idxScreenLine, idxScreenCol, bufimg[multi40[ddaCurrentValue] + texcolumn]);
//         }
//         while (ddaCurrentValue < ddaEndValue) {
//             (*ddaStepFunction)(); 
//             // printf ("%d\n", ddaCurrentValue, idxScreenLine);
//             if ((idxScreenLine >=0) && (idxScreenLine < 64)) {
//                 colorSquare(idxScreenLine, idxScreenCol, bufimg[multi40[ddaCurrentValue] + texcolumn]);
//             }
//             idxScreenLine   += 1;
//         }
//     }
//     PROFILE_LEAVE(ROUTINE_DRAW01);
// }


