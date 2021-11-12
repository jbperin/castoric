
.zero

_spritePtrReadTexture .dsb 2

.text

#ifndef USE_C_SPRITE

_unrollRightColumnASM
.(
loopLine
    ;; renCurrentColor     = spritePtrReadTexture[precalTexPixelOffset [spriteTextureLinIdx]];
    ldy _spriteTextureLinIdx
    lda _precalTexPixelOffset,Y
    tay
    lda (_spritePtrReadTexture),Y
    sta _renCurrentColor
    ;; if (renCurrentColor != EMPTY_ALPHA) {
    cmp #EMPTY_ALPHA
    beq skipTexel
        ;; jsr _colorRightTexel

    ldx         _renCurrentColor

    ;; *theAdr = ((*theAdr)&0xF8) | tabRightRed[renCurrentColor];
    ;; theAdr += NEXT_SCANLINE_INCREMENT;

    ldy         #0
    lda         (_theAdr),y
    and         #$F8
    ora         _tabRightRed,x
    sta         (_theAdr),y

;;     clc     
;;     lda         _theAdr
;;     adc         #40
;;     sta         _theAdr
;; .(  
;;     bcc skip:    inc _theAdr+1: skip .)


    ;; *theAdr = ((*theAdr)&0xF8) | tabRightGreen[renCurrentColor];
    ;; theAdr += NEXT_SCANLINE_INCREMENT;

    ldy         #40
    lda         (_theAdr),y
    and         #$F8
    ora         _tabRightGreen,x
    sta         (_theAdr),y

    ;; *theAdr = ((*theAdr)&0xF8) | tabRightBlue[renCurrentColor];
    ;; theAdr += NEXT_SCANLINE_INCREMENT;

    ldy         #80
    lda         (_theAdr),y
    and         #$F8
    ora         _tabRightBlue,x
    sta         (_theAdr),y

;;     clc     
;;     lda         _theAdr 
;;     adc         #120
;;     sta         _theAdr
;; .(: bcc skip:    inc _theAdr+1: skip .)





;;        jmp endIf
    ;; else
skipTexel
        clc   
        lda         _theAdr
        adc         #120
        sta         _theAdr
        .(:bcc skip:    inc _theAdr+1: skip:.):
endIf
    inc _spriteTextureLinIdx:

    dec _spriteNbLoopLine:
    bne loopLine

.)
    rts



_unrollLeftColumnASM
.(
loopLine
    ;; renCurrentColor     = spritePtrReadTexture[precalTexPixelOffset [spriteTextureLinIdx]];
    ldy _spriteTextureLinIdx
    lda _precalTexPixelOffset,Y
    tay
    lda (_spritePtrReadTexture),Y
    sta _renCurrentColor
    ;; if (renCurrentColor != EMPTY_ALPHA) {
    cmp #EMPTY_ALPHA
    beq skipTexel
        ;; jsr _colorLeftTexel
    ldx         _renCurrentColor

    ;; *theAdr = tabLeftRed[renCurrentColor]|((*theAdr)&0x07);
    ;; theAdr += NEXT_SCANLINE_INCREMENT;
    ldy         #0
    lda         (_theAdr),y
    and         #$07 
    ora         _tabLeftRed,x
    sta         (_theAdr),y

    ;; *theAdr = tabLeftGreen[renCurrentColor]|((*theAdr)&0x07);
    ;; theAdr += NEXT_SCANLINE_INCREMENT;

    ldy         #40
    lda         (_theAdr),y
    and         #$07 
    ora         _tabLeftGreen,x
    sta         (_theAdr),y

    ;; *theAdr = tabLeftBlue[renCurrentColor]|((*theAdr)&0x07);
    ;; theAdr += NEXT_SCANLINE_INCREMENT;

    ldy         #80
    lda         (_theAdr),y
    and         #$07
    ora         _tabLeftBlue,x
    sta         (_theAdr),y

;;     clc     
;;     lda         _theAdr
;;     adc         #120
;;     sta         _theAdr
;; .(:bcc skip:    inc _theAdr+1: skip .)




;;        jmp endIf
    ;; else
skipTexel
        clc   
        lda         _theAdr
        adc         #120
        sta         _theAdr
        .(:bcc skip:    inc _theAdr+1: skip:.):
endIf
    inc _spriteTextureLinIdx:

    dec _spriteNbLoopLine:
    bne loopLine

.)
    rts    



_spriteDrawColumn   
.(
    ;; FIXME : ugly hack because raylogdist is array of 16 bits
    lda _spriteViewportColIdx
    asl
    sta tmp5

    ;; if (objLogDistance[engCurrentObjectIdx] < raylogdist[spriteViewportColIdx])
    ldy _engCurrentObjectIdx
    lda _objLogDistance,Y
    ldy tmp5
    cmp _raylogdist,Y
    bcs spriteDrawColumn_Done

        ;; spriteTextureLinIdx     = spriteSavTextureLinIdx;
        ;; spriteNbLoopLine              = spriteSavNbLoopLine;
        lda _spriteSavTextureLinIdx
        sta _spriteTextureLinIdx
        lda _spriteSavNbLoopLine
        sta _spriteNbLoopLine

        ;; theAdr                  = (unsigned char *)((int)baseAdr + spriteScreenOffset ); // multi120[spriteViewportLinIdx]); // 
        clc
        lda _spriteScreenOffset
        adc _baseAdr
        sta _theAdr
        lda _spriteScreenOffset+1
        adc _baseAdr+1
        sta _theAdr+1

        ;; spriteTexColumn               = precalTexPixelOffset [spriteTextureColIdx];
        ;; spritePtrReadTexture    = spriteTexture + (unsigned int)((multi32_high[spriteTexColumn] << 8) | multi32_low[spriteTexColumn]);

        ldy _spriteTextureColIdx
        lda _precalTexPixelOffset,Y
        sta _spriteTexColumn
        ldy _spriteTexColumn
        lda _multi32_low,Y
        clc
        adc _spriteTexture
        sta _spritePtrReadTexture
        lda _multi32_high,Y
        adc _spriteTexture+1
        sta _spritePtrReadTexture+1

        ;; if ((spriteViewportColIdx&0x01) != 0){
        ;;     unrollLeftColumnASM();
        ;; } else {
        ;;     unrollRightColumnASM();
        ;; }
.(
            lda _spriteViewportColIdx
            and #$01
            beq drawRight
                jsr _unrollLeftColumnASM
            jmp endIf
drawRight
                jsr _unrollRightColumnASM
endIf
.)

spriteDrawColumn_Done
.)
    rts

#endif ;;USE_C_SPRITE
