unsigned char computeLogDist (signed char posX, signed char posY){

    signed char     deltaX, deltaY;
    signed char     alpha;
    unsigned char   log2Delta;
    unsigned int    log2dist;
    signed char     column;

    deltaX          = posX-rayCamPosX;
    deltaY          = posY-rayCamPosY;

    if ((deltaX == 0) && (deltaY == 0)) return 0;
    
    alpha           = ATAN2(deltaY, deltaX);
    column = tabAngle2Col[HALF_FOV_FIX_ANGLE-alpha+rayCamRotZ];
    if (abs(deltaX) > abs(deltaY)) {
        log2Delta = log2_tab[(unsigned char)(abs(deltaX))];
        log2dist = log2Delta + (unsigned int)tab_1overcos[(unsigned char)alpha];
    } else {
        log2Delta = log2_tab[(unsigned char)(abs(deltaY))];
        log2dist = log2Delta + (unsigned int)tab_1oversin[(unsigned char)alpha];
    }
    return (log2dist);
}
// compute orientation relatively to the camera
// param dir : 
unsigned char computeRelativeOrientation (signed char dirP, signed char dirC) {
    unsigned char res;
    signed char reldiff;
    unsigned char absdiff;
    // printf ("dirP = %d, dirC =%d \n",dirP, dirC);
    reldiff  = dirP - dirC + 32;
    absdiff = (unsigned char)(reldiff);
    // printf ("rel ori %d %d\n",dirP - dirC, res);
    // printf ("reldiff = %d, absdiff =  %d\n",reldiff, absdiff);
    res = absdiff >> 6;
    return res;
}