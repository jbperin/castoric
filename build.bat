

REM [ref lookup_generate]
python tools\precalDistance2Height.py > proto\c\tabunlogd2hh.h
python tools\precalcTabMulti.py > proto\c\tabMulti.h
python tools\precalcUnfish.py > proto\c\tabunfish.h
python tools\precalcLogTrigo.py > proto\c\tablogtrigo.h
python tools\precalcRayAngle.py > proto\c\tabrayangle.h

python tools\precalcTextureIndex.py > proto\c\tabIdxRdTexture.h
python tools\precalcIdxOfIdxRd.py > proto\c\tabAdrTabIdxRd.h

REM [ref config_viewport]
python tools\clearViewport.py > proto\c\viewport_s.s
python tools\clearColumn.py > proto\c\column_s.s

REM [ref texel_codec]
python tools\precalcTexel.py > proto\c\tabTexelColor.h

REM [ref config_generate]
python tools\generateConstants.py > proto\c\constants.h

REM [ref texture_file2buffer]
python tools\texture2buf.py img\bluestone.png > proto\c\texture_bluestone.h
python tools\texture2buf.py img\logo.png > proto\c\texture_logo.h
python tools\texture2buf.py img\redbrick.png > proto\c\texture_redbrick.h
python tools\texture2buf.py img\christmas.bmp > proto\c\texture_christmas.h
python tools\texture2buf.py img\pillar.png > proto\c\texture_pillar.h

python tools\texture2buf.py img\ceologo.png > proto\c\texture_ceologo.h
python tools\texture2buf.py img\oriclogo.png > proto\c\texture_oriclogo.h
python tools\texture2buf.py img\greystone.png > proto\c\texture_greystone.h
python tools\texture2buf.py img\purplestone.png > proto\c\texture_purplestone.h