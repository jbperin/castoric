#include "config.h"

#ifndef USE_C_COMPUTELOGDIST
;; void computeLogDistance (){
_computeLogDistance
.(
;;     signed char     deltaX(tmp5), deltaY(tmp5+1);
;;     unsigned char   log2Delta(tmp6);
;;     unsigned int    log2dist(tmp6+1);
;; 
;;     deltaX          = objPosX[engCurrentObjectIdx]-rayCamPosX;
    ldy _engCurrentObjectIdx
    lda _objPosX, y
    sec
    sbc _rayCamPosX
    sta tmp5
;;     deltaY          = objPosY[engCurrentObjectIdx]-rayCamPosY;
    lda _objPosY, y
    sec
    sbc _rayCamPosY
    sta tmp5+1
;; 
;;     if ((deltaX == 0) && (deltaY == 0)){
    lda tmp5
    ora tmp5+1
    bne  endif_01
;;         objLogDistance[engCurrentObjectIdx] = 0;
;;         objAngle[engCurrentObjectIdx] = 0;
        lda #0
        sta _objLogDistance,y
        sta _objAngle,y
        jmp computeLogDistanceDone
;;         return ;
endif_01
;;     }
;;     
;;     objAlpha[engCurrentObjectIdx]  = ATAN2(deltaY, deltaX);
    lda tmp5+1: sta _ty
    lda tmp5: sta _tx
    jsr         _atan2_8
    lda _res : 
    ldy _engCurrentObjectIdx
    sta _objAlpha, y

;;     objAngle[engCurrentObjectIdx]           = objAlpha[engCurrentObjectIdx]-rayCamRotZ;
    sec: sbc _rayCamRotZ: sta _objAngle,Y
;; 
;;     if (abs(deltaX) > abs(deltaY)) {
    lda tmp5
    .(:bpl already_positiv:eor #$FF: sec: adc #1: sta tmp5 :already_positiv:.)
    lda tmp5+1
    .(:bpl already_positiv:eor #$FF: sec: adc #1: sta tmp5+1 :already_positiv:.)
    cmp tmp5
    bcs else_02
;;         log2Delta = log2_tab[(unsigned char)(abs(deltaX))];
            ldy tmp5
            lda _log2_tab, Y
            sta tmp6
;;         objLogDistance[engCurrentObjectIdx] = log2Delta + (unsigned int)tab_1overcos[(unsigned char)objAlpha[engCurrentObjectIdx]];
            ldy _engCurrentObjectIdx
            lda _objAlpha,Y
            tay
            lda _tab_1overcos, Y
            clc 
            adc tmp6
            ldy _engCurrentObjectIdx
            sta _objLogDistance,Y
        jmp endif_02
;;     } else {
else_02
;;         log2Delta = log2_tab[(unsigned char)(abs(deltaY))];
            ldy tmp5+1
            lda _log2_tab, Y
            sta tmp6
;;         objLogDistance[engCurrentObjectIdx] = log2Delta + (unsigned int)tab_1oversin[(unsigned char)objAlpha[engCurrentObjectIdx]];
            ldy _engCurrentObjectIdx
            lda _objAlpha,Y
            tay
            lda _tab_1oversin, Y
            clc 
            adc tmp6
            ldy _engCurrentObjectIdx
            sta _objLogDistance,Y

endif_02
;;     }

computeLogDistanceDone
;; }
.)
    rts
#endif ;; USE_C_COMPUTELOGDIST


#ifndef USE_C_SPRITEVISIBILITY
_elaborateSpriteVisibility:
.(
    ;; visi_SpriteVisibility = 0;
    lda #0: sta _visi_SpriteVisibility

    ;; if (abs(objAngle[engCurrentObjectIdx]) < HALF_FOV_FIX_ANGLE){
    ldy _engCurrentObjectIdx
    lda _objAngle,Y
    .(:bpl is_positiv:eor #$FF: sec: adc #1: is_positiv:.)
    cmp #HALF_FOV_FIX_ANGLE
    bcs notVisibleType1
        ;; visi_SpriteVisibility = 1;
        lda #1: sta _visi_SpriteVisibility
        ;; return
        jmp elaborateSpriteVisibility_done

notVisibleType1:

    ;; visi_beta            = objAlpha[engCurrentObjectIdx] - 64; 
    ldy _engCurrentObjectIdx
    lda _objAlpha, y
    sec
    sbc #64
    sta _visi_beta

    ;; visi_incX            = tab3Cos[(unsigned char)visi_beta];
    ;; visi_incY            = tab3Sin[(unsigned char)visi_beta];
    ldy _visi_beta; ;; TODO: replace by tay
    lda _tab3Cos, y
    sta _visi_incX
    lda _tab3Sin, y
    sta _visi_incY

    ;; visi_angleRight          = ATAN2(objY+visi_incY - rayCamPosY, objX+visi_incX - rayCamPosX)-rayCamRotZ;
    lda _visi_incY: clc: adc _objY: sec: sbc _rayCamPosY: sta _ty
    lda _visi_incX: clc: adc _objX: sec: sbc _rayCamPosX: sta _tx
    jsr _atan2_8
    lda _res: sec: sbc _rayCamRotZ
    sta _visi_angleRight

    ;; if (abs(visi_angleRight) < HALF_FOV_FIX_ANGLE) {

    lda _visi_angleRight ;; TODO remove 
    .(:bpl is_positiv:eor #$FF: sec: adc #1: is_positiv:.)
    cmp #HALF_FOV_FIX_ANGLE
    bcs notVisibleType2
        ;; objAngleRight = visi_angleRight;
        ;; visi_SpriteVisibility = 2;
        lda #2: sta _visi_SpriteVisibility
        lda _visi_angleRight: sta _objAngleRight
        ;; return 
        jmp elaborateSpriteVisibility_done

notVisibleType2:

    ;; visi_angleLeft           = ATAN2(objY-visi_incY - rayCamPosY, objX-visi_incX - rayCamPosX)-rayCamRotZ;
    lda _objY: sec: sbc _visi_incY: sec: sbc _rayCamPosY: sta _ty
    lda _objX: sec: sbc _visi_incX: sec: sbc _rayCamPosX: sta _tx
    jsr _atan2_8
    lda _res: sec: sbc _rayCamRotZ
    sta _visi_angleLeft

    ;;if (abs(visi_angleLeft) < HALF_FOV_FIX_ANGLE){
    lda _visi_angleLeft ;; TODO remove 
    .(:bpl is_positiv:eor #$FF: sec: adc #1: is_positiv:.)
    cmp #HALF_FOV_FIX_ANGLE
    bcs notVisibleType3
        ;; objAngleLeft = visi_angleLeft;
        ;; visi_SpriteVisibility = 3;
        lda #3: sta _visi_SpriteVisibility
        lda _visi_angleLeft: sta _objAngleLeft
        ;;return;
        jmp elaborateSpriteVisibility_done
notVisibleType3:

elaborateSpriteVisibility_done:
.)
    rts
#endif ;; USE_C_SPRITEVISIBILITY
