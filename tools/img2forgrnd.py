import os
from PIL import Image, ImageOps
import codegen
import argparse
import math
import config
## [ref texture_file2buffer]
def image2TextureBuf(filepathname, replace_colors=None, fill_option=None, fill_color=None, output_name=None):
    base = os.path.basename(filepathname)
    namerad = os.path.splitext(base)[0] if output_name is None else output_name

    im = Image.open(filepathname)

    # Convertir en RGBA pour gérer la transparence et les remplacements de couleur
    rgba_im = im.convert('RGBA')

    # Appliquer l'option de remplissage si nécessaire
    if fill_option and fill_color:
        rgba_im = make_square(rgba_im, fill_option, fill_color)


    # Redimensionner en 32x32 si l'image n'est pas déjà de cette taille
    if rgba_im.size != (config.VIEWPORT_WIDTH,  config.VIEWPORT_HEIGHT):
        rgba_im = rgba_im.resize((config.VIEWPORT_WIDTH,  config.VIEWPORT_HEIGHT), Image.LANCZOS)

    imw, imh = rgba_im.width, rgba_im.height
    bufimgtranslat = []

    # Couleur de remplacement par défaut pour la transparence
    default_replacement_color = (152, 0, 136)

    for ii in range(imw):
        for jj in range(imh):    
            r, g, b, a = rgba_im.getpixel((ii, jj))
            
            # Vérifier si le pixel est transparent
            if a == 0:
                r, g, b = default_replacement_color
            else:
                # Vérifier si le pixel est proche d'une des couleurs à remplacer
                for target_color, tolerance in replace_colors:
                    if is_within_tolerance((r, g, b), target_color, tolerance):
                        r, g, b = default_replacement_color
                        break  # Arrête la vérification dès qu'une couleur correspond
            
            texel_value = (r // 64) * 16 + (g // 64) * 4 + (b // 64)
            bufimgtranslat.append(texel_value)

    # Utiliser output_name pour remplacer namerad si spécifié
    cCode = codegen.buffer2cCode("texture_" + namerad, "unsigned char", bufimgtranslat)

    return cCode

def is_within_tolerance(color, target_color, tolerance):
    """Calcul la distance euclidienne entre `color` et `target_color` et compare à `tolerance`."""
    distance = math.sqrt(sum((c - t) ** 2 for c, t in zip(color, target_color)))
    return distance <= tolerance

def make_square(im, option, fill_color):
    """Ajuste l'image pour qu'elle devienne un carré selon l'option spécifiée."""
    imw, imh = im.width, im.height
    if imw == imh:
        return im  # Déjà carré

    fill_color = (fill_color[0], fill_color[1], fill_color[2], 255)

    if imw < imh:
        # Image plus haute que large
        delta = imh - imw
        if option == "rfill":
            return ImageOps.expand(im, (0, 0, delta, 0), fill_color)
        elif option == "lfill":
            return ImageOps.expand(im, (delta, 0, 0, 0), fill_color)
        elif option == "cfill":
            return ImageOps.expand(im, (delta // 2, 0, delta - delta // 2, 0), fill_color)
    elif imw > imh:
        # Image plus large que haute
        delta = imw - imh
        if option == "ufill":
            return ImageOps.expand(im, (0, delta, 0, 0), fill_color)
        elif option == "dfill":
            return ImageOps.expand(im, (0, 0, 0, delta), fill_color)
        elif option == "cfill":
            return ImageOps.expand(im, (0, delta // 2, 0, delta - delta // 2), fill_color)

    return im  # Retourne l'image inchangée si aucune condition n'est remplie

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("imagefile", help="file path and name of the texture (png or bmp)")
    parser.add_argument(
        "--replace_color", nargs=4, action='append', type=int,
        metavar=('R', 'G', 'B', 'TOL'),
        help="RGB color and tolerance for color replacement. Can be used multiple times for different colors."
    )
    parser.add_argument(
        "--name", type=str, help="Optional name for the output buffer array."
    )

    # Options de remplissage mutuellement exclusives
    fill_group = parser.add_mutually_exclusive_group()
    fill_group.add_argument("--cfill", nargs=3, type=int, metavar=('R', 'G', 'B'),
                            help="Fill color to pad image on both sides to make it square")
    fill_group.add_argument("--rfill", nargs=3, type=int, metavar=('R', 'G', 'B'),
                            help="Fill color to pad image on the right to make it square")
    fill_group.add_argument("--lfill", nargs=3, type=int, metavar=('R', 'G', 'B'),
                            help="Fill color to pad image on the left to make it square")
    fill_group.add_argument("--ufill", nargs=3, type=int, metavar=('R', 'G', 'B'),
                            help="Fill color to pad image on the top to make it square")
    fill_group.add_argument("--dfill", nargs=3, type=int, metavar=('R', 'G', 'B'),
                            help="Fill color to pad image on the bottom to make it square")

    args = parser.parse_args()

    # Transforme les couleurs et tolérances en liste de tuples
    replace_colors = [(tuple(color[:3]), color[3]) for color in args.replace_color] if args.replace_color else []

    # Déterminer l'option de remplissage et la couleur de remplissage
    fill_option, fill_color = None, None
    for option in ['cfill', 'rfill', 'lfill', 'ufill', 'dfill']:
        color = getattr(args, option)
        if color:
            fill_option = option
            fill_color = color
            break

    print("// CAUTION !! GENERATED FILE. DO NOT MODIFY BY HAND")
    print("// texture buffer generated from file : " + args.imagefile)
    print("// by script : " + os.path.basename(__file__))
    print("// [ref texture_file2buffer]")
    print(image2TextureBuf(args.imagefile, replace_colors=replace_colors, fill_option=fill_option, fill_color=fill_color, output_name=args.name))

if __name__ == "__main__":
    # execute only if run as a script
    main()
