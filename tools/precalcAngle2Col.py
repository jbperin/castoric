# 
# Author : Jean-Baptiste PERIN
# Date : 2024

import struct 
import math
import codegen
import config

screen_width = config.VIEWPORT_WIDTH*3
hfov = math.radians(config.HFOV_IN_DEGREES)           # Champ de vision en radians

def genAngle2Col():
    # TODO: renvoyer une table de valeurs 16 bits pour line
    tabAngle2Col = []
    tabAngle2Col_High = []
    tabAngle2Col_Low = []
    for angle in range (256):
        alpha= int.from_bytes(bytes([angle]), byteorder='big', signed=True)
        v= round((screen_width/2)*(1-(math.tan(alpha*config.FIX_TO_RAD)/math.tan (hfov/2))))+3
        if v > 32767: # screen_width:
            v = 32767 # screen_width+1
        elif v < -32768:
            v = -32768 # 
        tabAngle2Col.append(v)
    for angle in range (64,128):
        tabAngle2Col[angle] = -32768
    for angle in range (128,192):
        tabAngle2Col[angle] = 32767
    for v in tabAngle2Col:
        tabAngle2Col_High.append(((v)&0xFF00)>>8)
        tabAngle2Col_Low.append((v)&0x00FF)
    return tabAngle2Col, tabAngle2Col_High, tabAngle2Col_Low

def main():
    [tabAngle2Col, tabAngle2Col_High, tabAngle2Col_Low] = genAngle2Col()

    sttabAngle2Col_High = codegen.buffer2asmCode("tabAngle2Col_High", "unsigned char", tabAngle2Col_High)
    sttabAngle2Col_Low = codegen.buffer2asmCode("tabAngle2Col_Low", "unsigned char", tabAngle2Col_Low)
    print (sttabAngle2Col_High)
    print (sttabAngle2Col_Low)


if __name__ == "__main__":
    main()
