import os
from PIL import Image
import codegen
import argparse

def fullImage2Sprites (filepathname,texname):
    im = Image.open(filepathname) #
    im.resize((32,32), Image.ANTIALIAS).save(f"img/{texname}.bmp", "BMP")
    im = Image.open(f"img/{texname}.bmp")
    rgb_im = im.convert('RGB')

    imw, imh = im.width, im.height

    bufimgtranslat = []

    for ii in range (imw):
        for jj in range (imh):    
            r, g, b = rgb_im.getpixel((ii, jj))
            texel_value = (r//85)*16 + (g//85)*4 + (b//85)
            if (texel_value == 17) : texel_value = 0
            elif (texel_value == 0) : texel_value = 1
            bufimgtranslat.append(texel_value)

    cCode = codegen.buffer2cCode(f"texture_{texname}", "unsigned char", bufimgtranslat)
    # with open (f"proto/c/texture_{texname}.h", "w") as fic:
    #     fic.write (cCode)
    return cCode


def image2Sprites (filepathname, ligne, colonne, texname):
    im = Image.open(filepathname) #
    im.crop((colonne*64+colonne, ligne*64+ligne, (colonne+1)*64+colonne, (ligne+1)*64+ligne)).resize((32,32), Image.ANTIALIAS).save(f"img/{texname}.bmp", "BMP")
    im = Image.open(f"img/{texname}.bmp")
    rgb_im = im.convert('RGB')

    imw, imh = im.width, im.height

    bufimgtranslat = []

    for ii in range (imw):
        for jj in range (imh):    
            r, g, b = rgb_im.getpixel((ii, jj))
            texel_value = (r//85)*16 + (g//85)*4 + (b//85)
            if (texel_value == 17) : texel_value = 0
            elif (texel_value == 0) : texel_value = 1
            bufimgtranslat.append(texel_value)

    cCode = codegen.buffer2cCode(f"texture_{texname}", "unsigned char", bufimgtranslat)
    with open (f"proto/c/texture_{texname}.h", "w") as fic:
        fic.write (cCode)
    return cCode


def main():
    print(image2Sprites ("img/spritessoldier.bmp", 1, 0, "soldier_front_00"))
    print(image2Sprites ("img/spritessoldier.bmp", 1, 2, "soldier_left_00"))
    print(image2Sprites ("img/spritessoldier.bmp", 1, 4, "soldier_back_00"))
    print(image2Sprites ("img/spritessoldier.bmp", 1, 6, "soldier_right_00"))

    print(image2Sprites ("img/spritessoldier.bmp", 3, 0, "soldier_front_01"))
    print(image2Sprites ("img/spritessoldier.bmp", 3, 2, "soldier_left_01"))
    print(image2Sprites ("img/spritessoldier.bmp", 3, 4, "soldier_back_01"))
    print(image2Sprites ("img/spritessoldier.bmp", 3, 6, "soldier_right_01"))
    # print (image2TextureBuf ('img/christmas.bmp'))
    print(fullImage2Sprites ("img/soldier_left_00_rew.PNG","soldier_left_00"))
    # print(image2Sprites ("img/spritessoldier.bmp", 5, 4, "soldier_dead_00"))
    # print(image2Sprites ("img/spritessoldier.bmp", 0, 0, "soldier_stand_00"))


if __name__ == "__main__":
    # execute only if run as a script
    main()    
