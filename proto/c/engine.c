// Inpired by Rax https://github.com/raxrax/oricAsciiTank

#define OBJECTS_MAX 18

// objects
#define OBJ_KEY 1
#define OBJ_SOLDIER 2

unsigned char   objType     [OBJECTS_MAX];
unsigned char   objActive   [OBJECTS_MAX];
signed char     objPosX     [OBJECTS_MAX];
signed char     objPosY     [OBJECTS_MAX];
char *          objData     [OBJECTS_MAX];
unsigned char * objTexture  [OBJECTS_MAX];

static unsigned char engNbObject;
static unsigned char engCurrentObjectIdx;

void keyUpdate();
char soldier_data [] = {32};
void soldierUpdate();


void engObjectPulse()
{
    switch (objType[engCurrentObjectIdx])
    {
        case OBJ_KEY:
            keyUpdate();
            break;
        case OBJ_SOLDIER:
            soldierUpdate();
            break;
    }
}

void engPulse() {
    engNbObject = 0;
    for (engCurrentObjectIdx = 0; engCurrentObjectIdx < OBJECTS_MAX; engCurrentObjectIdx++) {
        if (objActive[engCurrentObjectIdx]) {
            engNbObject++;
            engObjectPulse();
        }
    }
}



void keyUpdate()
{
    unsigned char ldist;
	unsigned char ex = objPosX[engCurrentObjectIdx];
    unsigned char ey = objPosY[engCurrentObjectIdx];
    // printf ("key pulse\n");
    ldist = computeLogDist (ex, ey);
    dichoInsert (engCurrentObjectIdx, ldist);

    // objTexture[engCurrentObjectIdx] = texture_aKey;
    objPosX[engCurrentObjectIdx] = ex;
    objPosY[engCurrentObjectIdx] = ey;
}

void soldierUpdate()
{
    unsigned char ldist;
    unsigned char displaystate;
    signed char direction;
    signed char sex, sey;
	signed char ex = objPosX[engCurrentObjectIdx];
    signed char ey = objPosY[engCurrentObjectIdx];
    sex = ex;
    sey = ey;
    direction = *(objData[engCurrentObjectIdx]);
    if (-112 >= direction) {
        ex--;
    } else if ((-112 < direction) && (-80 >= direction)) {
        ex--; ey--;
    } else if ((-80 < direction) && (-48 >= direction)) {
        ey--;
    } else if ((-48 < direction) && (-16 >= direction)) {
        ex++; ey--;
    } else if ((-16 < direction) && (16 >= direction)) {
        ex++;
    } else if ((16 < direction) && (48 >= direction)) {
        ex++; ey++;
    } else if ((48 < direction) && (80 >= direction)) {
        ey++;
    } else if ((80 < direction) && (112 >= direction)) {
        ex--; ey++;
    } else {
        ex--;
    }
    if (isInWall(ex, ey)) {
         direction += 16;
         *(objData[engCurrentObjectIdx]) = direction;
         ex = sex;
         ey = sey;
    }
    ldist = computeLogDist (ex, ey);
    dichoInsert (engCurrentObjectIdx, ldist);

    // computeRelativeOrientation (*(objData[engCurrentObjectIdx]));
    displaystate = computeRelativeOrientation (direction, rayCamRotZ);
    switch (displaystate) {
        case 0:
            objTexture[engCurrentObjectIdx] = texture_soldier_back_00; // ptrTextureSoldierBack;
            break;
        case 1:
            objTexture[engCurrentObjectIdx] = texture_soldier_left_00; // ptrTextureSoldierRight;
            break;
        case 2:
            objTexture[engCurrentObjectIdx] = texture_soldier_front_00; // ptrTextureSoldierFront;
            break;
        case 3:
            objTexture[engCurrentObjectIdx] = texture_soldier_right_00; // ptrTextureSoldierLeft;
            break;
    }
    objPosX[engCurrentObjectIdx] = ex;
    objPosY[engCurrentObjectIdx] = ey;
    refreshNeeded = 1;
}

void engInitObjects()
{
    unsigned char i;
    for (i = 0; i < OBJECTS_MAX; i++) objActive[i] = 0;

}

void engAddObject(char type, signed char x, signed char y, char *data)
{
    unsigned char i;
    for (i = 0; i < OBJECTS_MAX; i++)
    {
        if (objActive[i] == 0)
        {
            objActive[i] = 1;
            objType[i] = type;
            objPosX[i] = x;
            objPosY[i] = y;
            objData[i] = data;
            return;
        }
    }
}

void engDeleteObject(unsigned char objectNumber) {
    objActive[objectNumber] = 0;
}

void engDeleteAllObjects(unsigned char objectType) {
    unsigned char i;
    for (i = 0; i < OBJECTS_MAX; i++)
        if (1 == objActive[i] && objectType == objType[i])
            objActive[i] = 0;
}


// void main () {
//     int ii;
//     printf ("coucou\n");
//     engInitObjects();
//     engAddObject(OBJ_KEY, 3, 3, 0);
//     engAddObject(OBJ_SOLDIER, 3, -3, soldier_data);
//     for (ii=0 ; ii < 4; ii++){
//         engPulse();
//     }
// }