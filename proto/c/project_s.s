
#define FOCAL 100
#define SW VIEWPORT_WIDTH*3
#define SH VIEWPORT_HEIGHT*3

#include "tabAngle2Col.h"

; signed char deltaX, deltaY, deltaZ;
_DeltaX .dsb 1
_DeltaY .dsb 1
_DeltaZ .dsb 1

; signed char theta, alpha ;
_theta .dsb 1
_alpha .dsb 1

; unsigned char log_distance;
_log_distance .dsb 1

; unsigned char logDistancePlanCam;
_logDistancePlanCam .dsb 1

; int tmpXScreen, tmpYScreen;
_tmpXScreen .dsb 2
_tmpYScreen .dsb 2


; signed char PtX;
; signed char PtY;
; signed char PtZ;
_PointX .dsb 1
_PointY .dsb 1
_PointZ .dsb 1


; signed char *AngleH;
_AngleH .dsb 1

; signed int *xScreen;
_xScreen .dsb 2
; signed int *yScreen;
_yScreen .dsb 2



absDeltaX .dsb 1
absDeltaY .dsb 1

; unsigned int LogNumerator;
; unsigned int ExpResult;
_LogNumerator .dsb 2
_ExpResult .dsb 2

_lexp:
.(
    lda #0
    ;;sta _ExpResult
    sta _ExpResult+1
    lda _LogNumerator+1
    beq LogNumeratorUnder256
    lda _LogNumerator
    cmp #$21
    bmi LogNumeratorUnder289
    cmp #$33
    bmi LogNumeratorUnder307
    cmp #$40
    bmi LogNumeratorUnder320
    lda #$FF
    sta _ExpResult
    lda #$7F
    sta _ExpResult+1
    jmp exponentiateRatio_done
LogNumeratorUnder320
    sec
    sbc #$33;
    tay
    lda _tab_exp_extended3, y
    clc
    adc #$f4
    sta _ExpResult
    lda #$02
    adc #0
    sta _ExpResult+1
    jmp exponentiateRatio_done
LogNumeratorUnder307
    sec
    sbc #$21
    tay
    lda _tab_exp_extended2, y
    sta _ExpResult
    lda #2
    sta _ExpResult+1
    jmp exponentiateRatio_done
LogNumeratorUnder289
    sec
    sbc #$00
    tay
    lda _tab_exp_extended, y
    sta _ExpResult
    lda #1
    sta _ExpResult+1
    jmp exponentiateRatio_done
LogNumeratorUnder256
    ldy _LogNumerator
    lda _tab_exp, y
    sta _ExpResult
exponentiateRatio_done
.)
    rts

; unsigned int LONGEXP(unsigned int x){
;     LogNumerator = x;
;     asm (
;     return (ExpResult);
; }


tmp_log .dsb 2

_sproject:
.(
    pha:txa:pha:tya:pha 

    ; DeltaX = PtX - glCamPosX ;
	sec
	lda _PointX
	sbc _rayCamPosX
	sta _DeltaX
    sta _tx

    ; DeltaY = PtY - glCamPosY ;
	sec
	lda _PointY
	sbc _rayCamPosY
	sta _DeltaY
    sta _ty

    ; DeltaZ = PtZ - rayCamPosZ ;
	sec
	lda _PointZ
	sbc #3 ;  TODO .. should be _rayCamPosZ
	sta _DeltaZ

    ; theta = ATAN2 (DeltaY, DeltaX);
	jsr _atan2_8
	lda _res
	sta _theta

    ; alpha = theta - rayCamRotZ;
    sec
    sbc _rayCamRotZ
    sta _alpha

    ;*AngleH = alpha;
    sta _AngleH

    ; xScreen = tabAngle2Col [alpha]
    lda _alpha
    tay 
    lda _tabAngle2Col_Low, Y
    sta _xScreen
    lda _tabAngle2Col_High, Y
    sta _xScreen + 1
   

    lda _DeltaX
    bmi deltaX_negativ
    jmp absDeltaX_done
deltaX_negativ
    eor #$ff: sec: adc #$00
absDeltaX_done
    sta absDeltaX

    lda _DeltaY
    bmi deltaY_negativ
    jmp absDeltaY_done
deltaY_negativ
    eor #$ff: sec: adc #$00
absDeltaY_done
    sta absDeltaY

    lda absDeltaX
    cmp absDeltaY
    bcc absDeltaX_less_than_absDeltaY
    ; IF abs(DeltaX) > abs(DeltaY)
        ; log_distance = log2_tab[abs(DeltaX)] + tab_1overcos[(unsigned char)theta];
        ldy absDeltaX
        lda _log2_tab, y
        ldy _theta
        clc
        adc _tab_1overcos, y
        sta _log_distance

    jmp end_if_absDeltaX_less_than_absDeltaY
absDeltaX_less_than_absDeltaY    
    ; ELIF abs(DeltaX) < abs(DeltaY)
        ; log_distance = log2_tab[abs(DeltaY)] + tab_1oversin[(unsigned char)theta];
        ldy absDeltaY
        lda _log2_tab, y
        ldy _theta
        clc
        adc _tab_1oversin, y
        sta _log_distance
end_if_absDeltaX_less_than_absDeltaY

    ; if (log_distance < tab_1overcos[(unsigned char)alpha]) {
    ldy _alpha
    lda _log_distance
    cmp _tab_1overcos, y
    bcs LogDistanceGreaterOrEquals1overCos
    ;     logDistancePlanCam = tab_1overcos[(unsigned char)alpha]  - log_distance ;
        ldy _alpha 
        lda _tab_1overcos, y
        sec 
        sbc _log_distance
        sta _logDistancePlanCam
    jmp endifLogDistance
LogDistanceGreaterOrEquals1overCos    
    ; } else {
    ;     logDistancePlanCam = log_distance - tab_1overcos[(unsigned char)alpha];
        lda _log_distance
        sec
        ldy _alpha
        sbc _tab_1overcos, y
        sta _logDistancePlanCam
    ; }
endifLogDistance

    ; log2_tab[abs(DeltaZ)] + log2_tab[FOCAL] - logDistancePlanCam
    lda #0
    sta tmp_log+1
    ldy #FOCAL
    lda _log2_tab, y
    sta tmp_log

    lda _DeltaZ
    .(:bpl skip: eor #$FF: clc: adc #1: skip:.)
    tay
    lda _log2_tab, y
    clc
    adc tmp_log
    sta tmp_log
    lda tmp_log + 1 ; TODO to optimize . can at least be replace by lda #0
    adc #0
    sta tmp_log + 1

    sec
    lda tmp_log
    sbc _logDistancePlanCam
    sta _LogNumerator
    lda tmp_log+1
    sbc #0 ; _LogNumerator+1
    sta _LogNumerator+1

    jsr _lexp

    lda _DeltaZ
    bpl deltaZIsPositiv
    clc
    lda #<(SH/2)
    adc _ExpResult
    sta _yScreen
    lda #>(SH/2)
    adc _ExpResult + 1
    sta _yScreen+1

    jmp sproject_done
deltaZIsPositiv    
    sec 
    lda #<(SH/2)
    sbc _ExpResult
    sta _yScreen
    lda #>(SH/2)
    sbc _ExpResult + 1
    sta _yScreen+1
    
sproject_done
    pla:tay:pla:tax:pla
.)
    rts

