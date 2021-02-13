

#define NB_MAX_WALL     32
#define NB_MAX_POINT    64

#define ANGLE_MAX       (signed char)0xC0
#define IS_FRONT(i)     (((lAngle[(i)] & ANGLE_MAX) == 0) || ((lAngle[(i)] & ANGLE_MAX) == ANGLE_MAX))
#define abs(x)          (((x)<0)?-(x):(x))
// #define max(x,y)          (((x)<(y))?(y):(x))


signed char      RayAlpha  = 0;
signed char      RayLeftAlpha;
signed char      InterpAngleLeft;
unsigned char    RaySliceIdx;
unsigned char    InterpIdxLeft;
unsigned char    RayNbSlice;
unsigned int     RayDistance;
unsigned int     RayWallLog;
unsigned char    rayzbuffer[NUMBER_OF_SLICE]; // FIXME .. should be int
unsigned int     raylogdist[NUMBER_OF_SLICE];
#ifdef USE_C_TOTO
unsigned char*   tab_denom;
#endif
unsigned char    raywall[NUMBER_OF_SLICE];
unsigned char    RayCurrentWall;
signed char      lWallsCosBeta[NB_MAX_WALL];
unsigned char    RayIdXPoint1, RayIdXPoint2;
signed char      lPointsX[NB_MAX_POINT];
signed char      lPointsY[NB_MAX_POINT];
unsigned char    lWallsPt1[NB_MAX_WALL];
unsigned char    lWallsPt2[NB_MAX_WALL];
signed char      lAngle[NB_MAX_POINT];
unsigned char    isVisible[NB_MAX_POINT];
unsigned char    isFront[NB_MAX_POINT];
unsigned char    TableVerticalPos[NUMBER_OF_SLICE];
unsigned char    tabTexCol[NUMBER_OF_SLICE];

unsigned char           rayNbPoints;
unsigned char           rayNbWalls;

signed char             rayCamPosX = 0;
signed char             rayCamPosY = 0;
signed char             rayCamRotZ = 0;



// Precalculated logarithm of trigonometric function sine and cosine
// generated by python script 
// castoric> python tools\precalcLogTrigo.py > proto\c\tablogtrigo.h
#include "tablogtrigo.h"

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

#include "tabMulti.h"

#include "math.c"

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

#define ANGLE_TO_COL(x) tabAngle2Col[tabRayAngles[0]-lAngle[(x)]]

void drawFullCrossingWall(){

    preDraw();

    InterpAngleLeft     = RayLeftAlpha;
    RayNbSlice          = NUMBER_OF_SLICE;
    InterpIdxLeft       = 0;

    toto();
}

void drawFullVisibleWall(){

    preDraw();

    if (lAngle[RayIdXPoint1] > lAngle[RayIdXPoint2]){

        InterpIdxLeft       = ANGLE_TO_COL(RayIdXPoint1);
        InterpAngleLeft     = lAngle[RayIdXPoint1]+ rayCamRotZ;
        RayNbSlice          = (ANGLE_TO_COL(RayIdXPoint2) - InterpIdxLeft)+1;
    } else {

        InterpIdxLeft       = ANGLE_TO_COL(RayIdXPoint2);
        InterpAngleLeft     = lAngle[RayIdXPoint2]+ rayCamRotZ;
        RayNbSlice          = (ANGLE_TO_COL(RayIdXPoint1) - InterpIdxLeft)+1;
    }

    toto();
}


void drawLeftCuttingWall1Visible(){
    
    preDraw();

    InterpAngleLeft         = RayLeftAlpha;
    RayNbSlice              = ANGLE_TO_COL(RayIdXPoint1);
    InterpIdxLeft           = 0; 

    toto();
}
void drawLeftCuttingWall2Visible(){
    preDraw();

    InterpAngleLeft         = RayLeftAlpha;
    RayNbSlice              = ANGLE_TO_COL(RayIdXPoint2);
    InterpIdxLeft           = 0; 
    toto();
}

void drawRightCuttingWall1Visible(){
    preDraw();
    InterpIdxLeft           = ANGLE_TO_COL(RayIdXPoint1);
    InterpAngleLeft         = lAngle[RayIdXPoint1]+rayCamRotZ;
    RayNbSlice              = (NUMBER_OF_SLICE - InterpIdxLeft)+1;
 
    toto();
}
void drawRightCuttingWall2Visible(){
    preDraw();
    InterpAngleLeft         = lAngle[RayIdXPoint2]+rayCamRotZ;
    InterpIdxLeft           = ANGLE_TO_COL(RayIdXPoint2);
    RayNbSlice              = (NUMBER_OF_SLICE - InterpIdxLeft)+1;
    toto();
}

void rayProcessWalls() {
    int v0, v2;
    int v1;
    int deltaX, deltaY;
    signed char angle;
    

    for (RayCurrentWall = 0; RayCurrentWall < rayNbWalls; RayCurrentWall ++){
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
                        if (abs(lAngle[RayIdXPoint1] - lAngle[RayIdXPoint2]) > 127){
                            drawLeftCuttingWall1Visible();
                        } else {
                            drawRightCuttingWall1Visible();
                        }
                    }

                } else if (lAngle[RayIdXPoint1] < 0) {
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawRightCuttingWall1Visible();
                    } else {
                        if (abs(lAngle[RayIdXPoint1] - lAngle[RayIdXPoint2]) > 127){
                            drawRightCuttingWall1Visible();
                        } else {
                            drawLeftCuttingWall1Visible();
                        }
                    }

                } else { // (lAngle[idxPt1] < 0)
                    if (lAngle[RayIdXPoint2] < 0) {
                        drawRightCuttingWall1Visible();
                    } else {
                        drawLeftCuttingWall1Visible();
                    }
                }
            }
        } else {
            if (isVisible[RayIdXPoint2]) {

                // Point 2 is visible and Point 1 is not in FOV
                if (lAngle[RayIdXPoint2] > 0){
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawLeftCuttingWall2Visible();
                    } else {
                        if (abs(lAngle[RayIdXPoint2] - lAngle[RayIdXPoint1]) > 127){
                            drawLeftCuttingWall2Visible();
                        } else {
                            drawRightCuttingWall2Visible();
                        }
                    }
                } else if (lAngle[RayIdXPoint2] < 0) {
                    if ((lAngle[RayIdXPoint1] & 0x80) == (lAngle[RayIdXPoint2] & 0x80)) {
                        drawRightCuttingWall2Visible();
                    } else {
                        if (abs(lAngle[RayIdXPoint2] - lAngle[RayIdXPoint1]) > 127){
                            drawRightCuttingWall2Visible();
                        } else {
                            drawLeftCuttingWall2Visible();
                        }
                    }
                } else { // (lAngle[idxPt1] == 0
                    if (lAngle[RayIdXPoint1] < 0) {
                        drawRightCuttingWall2Visible();
                    } else {
                        drawLeftCuttingWall2Visible();
                    }
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
    }
    /* 
     * Change output from logarithmic scale to linear scale 
     */
    for (RaySliceIdx=0; RaySliceIdx<NUMBER_OF_SLICE; RaySliceIdx++){
        RayCurrentWall = raywall[RaySliceIdx];
        if (RayCurrentWall != 255) {
            raylogdist[RaySliceIdx] = rayzbuffer[RaySliceIdx];

            if (unfish[RaySliceIdx] < rayzbuffer[RaySliceIdx])
                rayzbuffer[RaySliceIdx] -= unfish[RaySliceIdx];


            TableVerticalPos[RaySliceIdx] =dist2hh(rayzbuffer[RaySliceIdx]);
            rayzbuffer[RaySliceIdx] = longexp(rayzbuffer[RaySliceIdx]);        
        } else {
            
        }
    }

    // Compute texture column informations
    for (RaySliceIdx=0; RaySliceIdx<NUMBER_OF_SLICE; RaySliceIdx++){
        RayCurrentWall = raywall[RaySliceIdx];
        if (RayCurrentWall != 255) {
            angle       = rayCamRotZ + tabRayAngles[RaySliceIdx];
            if (lWallsCosBeta[RayCurrentWall] == 0){    // Wall is O,y aligned   
                deltaY      = lPointsY[lWallsPt1[RayCurrentWall]]-rayCamPosY;
                 
                if (angle == 0){
                    v0 = 0;
                    v1 = 0;
                    v2 = 0;
                } else if (angle > 0) {
                    v0 = log2sin(angle); // round(32*math.log2(math.sin(angle*FIX2RAD)*COEFF))
                    v1 = raylogdist[RaySliceIdx] + v0;
                    if (v1<0) v1=0;
                    v2 = longexp(v1); // (2**(v1/32))

                } else if (angle < 0) {
                    v0 = log2sin(angle); // round(32*math.log2(-math.sin(angle*FIX2RAD)*COEFF))
                    v1 = raylogdist[RaySliceIdx] + v0;
                    if (v1<0) v1=0;
                    v2 = -longexp(v1); // -(2**(v1/32)) # 
                }
                if (deltaY < 0) {
                    tabTexCol [RaySliceIdx]        = abs(v2+multiCoeff[abs(deltaY)]);
                } else {
                    tabTexCol [RaySliceIdx]        = abs(v2-multiCoeff[abs(deltaY)]);
                }
            } else {                       // Wall is O,x aligned 
                deltaX      = lPointsX[lWallsPt1[RayCurrentWall]]-rayCamPosX;
                if (tabRayAngles[RaySliceIdx] == 0){
                    v0 = 0;
                    v1 = 0;
                    v2 = 0;
                } else if (abs (angle) < 64) {
                    v0 = log2cos(angle); // round(32*math.log2(math.sin(angle*FIX2RAD)*COEFF))
                    v1 = raylogdist[RaySliceIdx] + v0;
                    if (v1<0) v1=0;
                    v2 = longexp(v1); // (2**(v1/32))

                } else if (abs (angle) >= 64) {
                    v0 = log2cos(angle); // round(32*math.log2(-math.sin(angle*FIX2RAD)*COEFF))
                    v1 = raylogdist[RaySliceIdx] + v0;
                    if (v1<0) v1=0;
                    v2 = -longexp(v1); // -(2**(v1/32)) # 
                }
                if (deltaX < 0){
                    tabTexCol [RaySliceIdx]        = abs(v2+multiCoeff[abs(deltaX)]);
                } else {
                    tabTexCol [RaySliceIdx]        = abs(v2-multiCoeff[deltaX]);
                }
            }
        } else {
            tabTexCol [RaySliceIdx]        = 0;
        }

    }

}

void rayProcessPoints() {

    for (RayIdXPoint1 = 0; RayIdXPoint1 < rayNbPoints; RayIdXPoint1 ++) {

        lAngle[RayIdXPoint1]        =  ATAN2(lPointsY[RayIdXPoint1]-rayCamPosY, lPointsX[RayIdXPoint1]-rayCamPosX) - rayCamRotZ;

        isVisible[RayIdXPoint1]     = (abs(lAngle[RayIdXPoint1])<tabRayAngles[0])?1:0;
        isFront[RayIdXPoint1]       = IS_FRONT(RayIdXPoint1)?1:0;
    }
}
