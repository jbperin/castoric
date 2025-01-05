// Raycasting system for Oric machine
// Author : Jean-Baptiste PERIN
// Date : 2021


#define ANGLE_MAX       (signed char)0xC0
#define IS_FRONT(i)     (((lAngle[(i)] & ANGLE_MAX) == 0) || ((lAngle[(i)] & ANGLE_MAX) == ANGLE_MAX))
#define abs(x)          (((x)<0)?-(x):(x))
// #define max(x,y)          (((x)<(y))?(y):(x))

// Ray hit distance computation
signed char      RayAlpha  = 0;
signed char      RayLeftAlpha;
signed char      InterpAngleLeft;
unsigned char    RaySliceIdx;
unsigned char    InterpIdxLeft;
unsigned char    RayNbSlice;
unsigned int     RayDistance;
unsigned int     RayWallLog;

// Texture offset computation
int rayTmp2;
int rayTmp1;
int rayDeltaX, rayDeltaY;
signed char rayCurrentAngle;



unsigned char    rayzbuffer[NUMBER_OF_SLICE]; // FIXME .. should be int
unsigned int     raylogdist[NUMBER_OF_SLICE];
unsigned char    raywall[NUMBER_OF_SLICE];
#ifdef USE_C_TOTO
unsigned char*   tab_denom;
#endif
unsigned char    RayCurrentWall;
signed char      lWallsCosBeta[NB_MAX_WALL];
unsigned char    RayIdXPoint1, RayIdXPoint2;

// [ref scene_load]
signed char      lPointsX[NB_MAX_POINT];
signed char      lPointsY[NB_MAX_POINT];
unsigned char    lWallsPt1[NB_MAX_WALL];
unsigned char    lWallsPt2[NB_MAX_WALL];

unsigned char           rayNbPoints;
unsigned char           rayNbWalls;


signed char      lAngle[NB_MAX_POINT];
unsigned char    isVisible[NB_MAX_POINT];
unsigned char    isFront[NB_MAX_POINT];
unsigned char    TableVerticalPos[NUMBER_OF_SLICE];
unsigned char    tabTexCol[NUMBER_OF_SLICE];


// [ref camera_situation]
signed char             rayCamPosX = 0;
signed char             rayCamPosY = 0;
signed char             rayCamRotZ = 0;

#ifndef USE_ASM_LOGTRIGO
// [ref lookup_generate]

// Precalculated logarithm of trigonometric function sine and cosine
// generated by python script 
// castoric> python tools\precalcLogTrigo.py > proto\c\tablogtrigo.h
#include "tablogtrigo.h"
#else
extern signed char tabLog2Sin[];
extern signed char tabLog2Cos[];
extern unsigned char tab_1oversin[];
extern unsigned char tab_1overcos[];
#endif // USE_ASM_LOGTRIGO

// Precalculated table of angle to cast ray at and reverse table 
// generated by python script 
// castoric> python tools\precalcRayAngle.py > proto\c\tabrayangle.h
#include "tabrayangle.h"


// Compensation table to prevent fisheye effect
// generated by python script 
// castoric> python tools\precalcUnfish.py > proto\c\unfish.h
#include "tabunfish.h"


// Conversion table between distance and height on screen
// generated by python script 
// castoric> python tools\precalcDistance2Height.py > proto\c\tabunlogd2hh.h
#include "tabunlogd2hh.h"

#ifndef RAYTABLE_IN_OVERLAY
#include "tabMulti.h"
#endif

#include "math.c"



#ifdef USE_C_RAYCAST

void rayInitCasting(){
    unsigned char ii;
    for (ii=0; ii< NUMBER_OF_SLICE; ii++) {
        rayzbuffer[ii]      = 255;
        raywall[ii]         = 255;
    }
}

#endif // USE_C_RAYCAST

#ifdef USE_C_TOTO
void toto() {

    RayAlpha        = InterpAngleLeft; //tabRayAngles[InterpIdxLeft]-glCamRotZ;
    RaySliceIdx     = InterpIdxLeft;
    if (RayNbSlice == 0) return;

#ifdef DEBUG
    printf ("Wall %d from %d to %d, %d slice from (c=%d a=%d) to (c=%d, a=%d) \n", RayCurrentWall, RayIdXPoint1, RayIdXPoint2,
    RayNbSlice, RaySliceIdx,RayAlpha, RaySliceIdx+RayNbSlice, RayAlpha+tabRayAngles[RaySliceIdx+RayNbSlice]
    );
#endif
    while(1){
        RayDistance     = RayWallLog + (unsigned int)tab_denom[(unsigned char)RayAlpha];

#ifdef DEBUG
        printf ("RaySliceIdx = %d, RayAlpha = %d, RayDistance=%d\n", RaySliceIdx, RayAlpha, RayDistance);
#endif
        if (rayzbuffer[RaySliceIdx] > RayDistance){
            rayzbuffer[RaySliceIdx]     = RayDistance;
            raywall[RaySliceIdx]        = RayCurrentWall;
        } 
        if ((--RayNbSlice) == 0) break;
        RaySliceIdx     += 1;
        RayAlpha        = rayCamRotZ + tabRayAngles[RaySliceIdx];

    }
}
#else
extern void toto();
#endif

#ifdef USE_C_PREDRAW
void preDraw(){

    if (lWallsCosBeta[RayCurrentWall] == 0){    // Wall is O,y aligned   
        RayWallLog      = log2_tab[(unsigned char)(abs(lPointsX[RayIdXPoint1]-rayCamPosX))];
        tab_denom       = tab_1overcos;
    } else {                                    // Wall is O,x aligned
        RayWallLog      = log2_tab[(unsigned char)(abs(lPointsY[RayIdXPoint1]-rayCamPosY))];
        tab_denom       = tab_1oversin;
    }
}
#else
extern void preDraw();
#endif // USE_C_PREDRAW

#define ANGLE_TO_COL(x) tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(x)]]



#ifdef USE_C_RAYCAST


void drawFullVisibleWall(){

    preDraw();

    if (lAngle[RayIdXPoint1] > lAngle[RayIdXPoint2]){

        InterpIdxLeft       = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint1)]] ; // ANGLE_TO_COL(RayIdXPoint1);
        InterpAngleLeft     = lAngle[RayIdXPoint1]+ rayCamRotZ;
        RayNbSlice          = (tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint2)]] - InterpIdxLeft)+1;
    } else {

        InterpIdxLeft       = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint2)]];
        InterpAngleLeft     = lAngle[RayIdXPoint2]+ rayCamRotZ;
        RayNbSlice          = (tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint1)]] - InterpIdxLeft)+1;
    }

    toto();
}


void drawFullCrossingWall(){

    preDraw();

    InterpAngleLeft     = RayLeftAlpha;
    RayNbSlice          = NUMBER_OF_SLICE;
    InterpIdxLeft       = 0;

    toto();
}

void drawLeftCuttingWall1Visible(){
    
    preDraw();

    InterpAngleLeft         = RayLeftAlpha;
    RayNbSlice              = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint1)]];
    InterpIdxLeft           = 0; 

    toto();

}
void drawLeftCuttingWall2Visible(){
    preDraw();

    InterpAngleLeft         = RayLeftAlpha;
    RayNbSlice              = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint2)]];
    InterpIdxLeft           = 0; 
    toto();

}


void drawRightCuttingWall2Visible(){
    preDraw();
    InterpAngleLeft         = lAngle[RayIdXPoint2]+rayCamRotZ;
    InterpIdxLeft           = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint2)]];
    RayNbSlice              = (NUMBER_OF_SLICE - InterpIdxLeft)+1;
    toto();

    // asm(
    //     "jsr _preDraw;"

    //     "ldy _RayIdXPoint2;"
    //     "lda _lAngle, Y;"
    //     "tay;" //save lAngle[RayIdXPoint2] in Y
    //     "clc;"
    //     "adc _rayCamRotZ;"
    //     "sta _InterpAngleLeft;"

    //     "tya;" //retrieve lAngle[RayIdXPoint2] from Y
    //     "eor #$FF;"
    //     "sec;"
    //     "adc #HALF_FOV_FIX_ANGLE;"
    //     "tay;"
    //     "lda _tabAngle2Col, Y;"
    //     "sta _InterpIdxLeft;"

    //     "eor #$FF;"
    //     "sec;"
    //     "adc #NUMBER_OF_SLICE;"
    //     "sta _RayNbSlice;"

    //     "jsr _toto;"
    // );

}



void drawRightCuttingWall1Visible(){
    preDraw();
    InterpIdxLeft           = tabAngle2Col[HALF_FOV_FIX_ANGLE-lAngle[(RayIdXPoint1)]];
    InterpAngleLeft         = lAngle[RayIdXPoint1]+rayCamRotZ;
    RayNbSlice              = (NUMBER_OF_SLICE - InterpIdxLeft)+1;
 
    toto();

    // asm (
    //     "jsr _preDraw;"

    //     "ldy _RayIdXPoint1;"
    //     "lda _lAngle, Y;"
    //     "tay;" //save lAngle[RayIdXPoint1] in Y
    //     "clc;"
    //     "adc _rayCamRotZ;"
    //     "sta _InterpAngleLeft;"

    //     "tya;" //retrieve lAngle[RayIdXPoint1] from Y
    //     "eor #$FF;"
    //     "sec;"
    //     "adc #HALF_FOV_FIX_ANGLE;"
    //     "tay;"
    //     "lda _tabAngle2Col, Y;"
    //     "sta _InterpIdxLeft;"

    //     "eor #$FF;"
    //     "sec;"
    //     "adc #NUMBER_OF_SLICE;"
    //     "sta _RayNbSlice;"
    //     "jsr _toto;"
    // );

}
#endif // USE_C_RAYCAST


#ifdef USE_C_ZBUFFWALLS
void zbuffWalls() {
    
    RayCurrentWall = rayNbWalls;
    do {
        RayCurrentWall --;


        RayIdXPoint1        = lWallsPt1[RayCurrentWall];
        RayIdXPoint2        = lWallsPt2[RayCurrentWall];

        if (isVisible[RayIdXPoint1]) {
            if (isVisible[RayIdXPoint2]) {
                drawFullVisibleWall();
            } else {
                // Point 1 is visible and Point 2 is not in FOV
                if (lAngle[RayIdXPoint1] > 0){
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawLeftCuttingWall1Visible();
                    } else {
#ifdef USE_C_RAYCAST
                        if (abs(lAngle[RayIdXPoint1] - lAngle[RayIdXPoint2]) > 127){
                            drawLeftCuttingWall1Visible();
                        } else {
                            drawRightCuttingWall1Visible();
                        }
#else 
                        asm (
                            "ldy _RayIdXPoint2;"
                            "lda _lAngle,Y;"
                            "sta reg7;"
                            "ldy _RayIdXPoint1;"
                            "lda _lAngle,Y;"
                            "sec;"
                            "sbc reg7;"
                            "bvc VeryWideAngle_01;"
                            "jsr _drawLeftCuttingWall1Visible;"
                            "jmp endAngleSpecific_01;"
                        "VeryWideAngle_01;"
                            "jsr _drawRightCuttingWall1Visible;"
                        "endAngleSpecific_01;"

                        );
#endif 
                    }

                } else if (lAngle[RayIdXPoint1] < 0) {
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawRightCuttingWall1Visible();
                    } else {
#ifdef USE_C_RAYCAST
                        if (abs(lAngle[RayIdXPoint1] - lAngle[RayIdXPoint2]) > 127){
                            drawRightCuttingWall1Visible();
                        } else {
                            drawLeftCuttingWall1Visible();
                        }
#else
                        asm (
                            "ldy _RayIdXPoint2;"
                            "lda _lAngle,Y;"
                            "sta reg7;"
                            "ldy _RayIdXPoint1;"
                            "lda _lAngle,Y;"
                            "sec;"
                            "sbc reg7;"
                            "bvc VeryWideAngle_02;"
                            "jsr _drawRightCuttingWall1Visible;"
                            "jmp endAngleSpecific_02;"
                        "VeryWideAngle_02;"
                            "jsr _drawLeftCuttingWall1Visible;"
                        "endAngleSpecific_02;"

                        );
#endif
                    }

                } else { // (lAngle[idxPt1] < 0)
#ifdef USE_C_RAYCAST
                    if (lAngle[RayIdXPoint2] < 0) {
                        drawRightCuttingWall1Visible();
                    } else {
                        drawLeftCuttingWall1Visible();
                    }
#else
                    asm (
                        "ldy _RayIdXPoint2;"
                        "lda _lAngle,Y;"
                        "bpl PositivPoint2Angle;"
                            "jsr _drawRightCuttingWall1Visible;"
                        "jmp endPositivPoint2Angle;"
                        "PositivPoint2Angle;"
                            "jsr _drawLeftCuttingWall1Visible;"
                        "endPositivPoint2Angle;"

                    );
#endif
                }
            }
        } else {
            if (isVisible[RayIdXPoint2]) {

                // Point 2 is visible and Point 1 is not in FOV
                if (lAngle[RayIdXPoint2] > 0){
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawLeftCuttingWall2Visible();
                    } else {
#ifdef USE_C_RAYCAST
                        if (abs(lAngle[RayIdXPoint2] - lAngle[RayIdXPoint1]) > 127){
                            drawLeftCuttingWall2Visible();
                        } else {
                            drawRightCuttingWall2Visible();
                        }
#else
                        asm (
                            "ldy _RayIdXPoint1;"
                            "lda _lAngle,Y;"
                            "sta reg7;"
                            "ldy _RayIdXPoint2;"
                            "lda _lAngle,Y;"
                            "sec;"
                            "sbc reg7;"
                            "bvc VeryWideAngle_03;"
                            "jsr _drawLeftCuttingWall2Visible;"
                            "jmp endAngleSpecific_03;"
                        "VeryWideAngle_03;"
                            "jsr _drawRightCuttingWall2Visible;"
                        "endAngleSpecific_03;"

                        );
#endif
                    }
                } else if (lAngle[RayIdXPoint2] < 0) {
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawRightCuttingWall2Visible();
                    } else {
#ifdef USE_C_RAYCAST

                        if (abs(lAngle[RayIdXPoint2] - lAngle[RayIdXPoint1]) > 127){
                            drawRightCuttingWall2Visible();
                        } else {
                            drawLeftCuttingWall2Visible();
                        }
#else
                        asm (
                            "ldy _RayIdXPoint1;"
                            "lda _lAngle,Y;"
                            "sta reg7;"
                            "ldy _RayIdXPoint2;"
                            "lda _lAngle,Y;"
                            "sec;"
                            "sbc reg7;"
                            "bvc VeryWideAngle_04;"
                            "jsr _drawRightCuttingWall2Visible;"
                            "jmp endAngleSpecific_04;"
                        "VeryWideAngle_04;"
                            "jsr _drawLeftCuttingWall2Visible;"
                        "endAngleSpecific_04;"

                        );
#endif
                    }
                } else { // (lAngle[idxPt1] == 0
#ifdef USE_C_RAYCAST
                    if (lAngle[RayIdXPoint1] < 0) {
                        drawRightCuttingWall2Visible();
                    } else {
                        drawLeftCuttingWall2Visible();
                    }
#else 
                    asm (
                        "ldy _RayIdXPoint1;"
                        "lda _lAngle,Y;"
                        "bpl PositivPoint2Angle_02;"
                            "jsr _drawRightCuttingWall2Visible;"
                        "jmp endPositivPoint2Angle_02;"
                        "PositivPoint2Angle_02;"
                            "jsr _drawLeftCuttingWall2Visible;"
                        "endPositivPoint2Angle_02;"

                    );
#endif
                }
            } else {
                if (   (isFront[RayIdXPoint1] 
                    || isFront[RayIdXPoint2] )
                    && ((lAngle[RayIdXPoint1] & 0x80) != (lAngle[RayIdXPoint2] & 0x80)) 
                    && (abs(lAngle[RayIdXPoint2]) < 127 - abs(lAngle[RayIdXPoint1])) 
                ) {
                    drawFullCrossingWall();
                }
            }
        }
    } while (RayCurrentWall != 0);
}

#endif // USE_C_ZBUFFWALLS


#ifdef USE_C_DISTOFFSETSLICES
void    distOffsetSlices() {

    
    RaySliceIdx = NUMBER_OF_SLICE;
    do {
         
        RaySliceIdx--;


        RayCurrentWall                  = raywall[RaySliceIdx];
        tabTexCol [RaySliceIdx]         = 0;


        if (RayCurrentWall != 255) {

               

            raylogdist[RaySliceIdx] = rayzbuffer[RaySliceIdx];
            if (unfish[RaySliceIdx] < rayzbuffer[RaySliceIdx])
                rayzbuffer[RaySliceIdx] -= unfish[RaySliceIdx];



            TableVerticalPos[RaySliceIdx] =dist2hh(rayzbuffer[RaySliceIdx]);

            /* 
            * Change output from logarithmic scale to linear scale 
            */
            // rayzbuffer[RaySliceIdx] = longexp(rayzbuffer[RaySliceIdx]);        

            /*
             * Compute texture column informations
             */
 
            rayCurrentAngle       = rayCamRotZ + tabRayAngles[RaySliceIdx];

            if (lWallsCosBeta[RayCurrentWall] == 0){    // Wall is O,y aligned   
 
             
                if (rayCurrentAngle == 0){
                    rayTmp2 = 0;
                } else {
                    rayTmp1 = raylogdist[RaySliceIdx] + tabLog2Sin[(unsigned char)rayCurrentAngle];
                    if (rayTmp1<0) rayTmp1=0;
                    rayTmp2 = longexp(rayTmp1);
                    if (rayCurrentAngle <= 0) rayTmp2 = -rayTmp2;
                }

                rayDeltaY      = lPointsY[lWallsPt1[RayCurrentWall]]-rayCamPosY;
                if (rayDeltaY < 0) {
                    rayTmp2      +=  multiCoeff[-rayDeltaY];
                } else {
                    rayTmp2      -=  multiCoeff[rayDeltaY];
                }


            } else {                       // Wall is O,x aligned 
         

                if (tabRayAngles[RaySliceIdx] == 0){
                    rayTmp2 = 0;
                } else {
                    rayTmp1 = raylogdist[RaySliceIdx] + tabLog2Cos[(unsigned char)rayCurrentAngle]; // v0; //tabLog2Cos[rayCurrentAngle];
                    if (rayTmp1<0) rayTmp1=0;
                    rayTmp2 = longexp(rayTmp1);
                    if (abs (rayCurrentAngle) >= 64) {
                        rayTmp2 = -rayTmp2; // -(2**(v1/32)) # 
                    }
                }




                rayDeltaX      = lPointsX[lWallsPt1[RayCurrentWall]]-rayCamPosX;
                if (rayDeltaX < 0){
                    rayTmp2      += multiCoeff[-rayDeltaX];
                } else {
                    rayTmp2      -= multiCoeff[rayDeltaX];
                }

            }
            if (rayTmp2 < 0) rayTmp2 = -rayTmp2;
            tabTexCol [RaySliceIdx]        = rayTmp2;
        }
    } while (RaySliceIdx !=0);

}
#endif // USE_C_DISTOFFSETSLICES

void rayProcessWalls() {

    
    zbuffWalls();

    distOffsetSlices();

}

#ifdef USE_C_PROCESS_POINT
void rayProcessPoints() {

    RayIdXPoint1 = rayNbPoints;

    do {
        RayIdXPoint1                -= 1;
        lAngle[RayIdXPoint1]        =  ATAN2(lPointsY[RayIdXPoint1]-rayCamPosY, lPointsX[RayIdXPoint1]-rayCamPosX) - rayCamRotZ;

        isVisible[RayIdXPoint1]     = (abs(lAngle[RayIdXPoint1])<HALF_FOV_FIX_ANGLE)?1:0;
        isFront[RayIdXPoint1]       = IS_FRONT(RayIdXPoint1)?1:0;
    } while ( RayIdXPoint1 != 0 );
}
#endif // USE_C_PROCESS_POINT