#include "config.h"

#ifndef USE_C_COMPUTELOGDIST

_disDeltaX .dsb 1
_disDeltaY .dsb 1
_disLog2Delta .dsb 1 
_disLogDistance .dsb 1


;; void computeLogDistance (){
_computeLogDistance
.(
;;     signed char     deltaX(_disDeltaX), deltaY(_disDeltaY);
;;     unsigned char   log2Delta(_disLog2Delta);
;;     unsigned int    log2dist(_disLog2Delta+1);
;; 
;;     deltaX          = objPosX[engCurrentObjectIdx]-rayCamPosX;
    ldy _engCurrentObjectIdx
    lda _objPosX, y
    sec
    sbc _rayCamPosX
    sta _disDeltaX
;;     deltaY          = objPosY[engCurrentObjectIdx]-rayCamPosY;
    lda _objPosY, y
    sec
    sbc _rayCamPosY
    sta _disDeltaY
;; 
;;     if ((deltaX == 0) && (deltaY == 0)){
    lda _disDeltaX
    ora _disDeltaY
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
    lda _disDeltaY: sta _ty
    lda _disDeltaX: sta _tx
    jsr         _atan2_8
    lda _res : 
    ldy _engCurrentObjectIdx
    sta _objAlpha, y

;;     objAngle[engCurrentObjectIdx]           = objAlpha[engCurrentObjectIdx]-rayCamRotZ;
    sec: sbc _rayCamRotZ: sta _objAngle,Y
;; 
;;     if (abs(deltaX) > abs(deltaY)) {
    lda _disDeltaX
    .(:bpl already_positiv:eor #$FF: sec: adc #0: sta _disDeltaX :already_positiv:.)
    lda _disDeltaY
    .(:bpl already_positiv:eor #$FF: sec: adc #0: sta _disDeltaY :already_positiv:.)
    cmp _disDeltaX
    bcs else_02
;;         log2Delta = log2_tab[(unsigned char)(abs(deltaX))];
            ldy _disDeltaX
            lda _log2_tab, Y
            sta _disLog2Delta
;;         objLogDistance[engCurrentObjectIdx] = log2Delta + (unsigned int)tab_1overcos[(unsigned char)objAlpha[engCurrentObjectIdx]];
            ldy _engCurrentObjectIdx
            lda _objAlpha,Y
            tay
            lda _tab_1overcos, Y
            clc 
            adc _disLog2Delta
            ;; ldy _engCurrentObjectIdx
            ;; sta _objLogDistance,Y
            sta _disLogDistance
        jmp endif_02
;;     } else {
else_02
;;         log2Delta = log2_tab[(unsigned char)(abs(deltaY))];
            ldy _disDeltaY
            lda _log2_tab, Y
            sta _disLog2Delta
;;         objLogDistance[engCurrentObjectIdx] = log2Delta + (unsigned int)tab_1oversin[(unsigned char)objAlpha[engCurrentObjectIdx]];
            ldy _engCurrentObjectIdx
            lda _objAlpha,Y
            tay
            lda _tab_1oversin, Y
            clc 
            adc _disLog2Delta
            ;;ldy _engCurrentObjectIdx
            ;;sta _objLogDistance,Y
            sta _disLogDistance

endif_02
;;     }

    ; if (log_distance < tab_1overcos[(unsigned char)objAngle[engCurrentObjectIdx]]) {
    ldy _engCurrentObjectIdx
    lda _objAngle, y
    tay 
    lda _tab_1overcos, y
    sta tmp7
    lda _disLogDistance
    cmp  tmp7
    bcs else_03
    ;     objLogDistance[engCurrentObjectIdx] =  tab_1overcos[(unsigned char)objAngle[engCurrentObjectIdx]]  - log_distance ;
        lda tmp7
        sec
        sbc _disLogDistance
        ldy _engCurrentObjectIdx 
        sta _objLogDistance, y

    jmp endif_03
    ; } else {
else_03
    ;     objLogDistance[engCurrentObjectIdx] =  log_distance - tab_1overcos[(unsigned char)objAngle[engCurrentObjectIdx]];
        lda _disLogDistance
        sec
        sbc tmp7
        ldy _engCurrentObjectIdx 
        sta _objLogDistance, y

endif_03
    ; }



computeLogDistanceDone
;; }
.)
    rts
#endif ;; USE_C_COMPUTELOGDIST