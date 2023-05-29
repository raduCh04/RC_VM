bits 16
mov word [1000], 1
mov word [1002], 2
mov word [1004], 3
mov word [1006], 4
mov bx, 1000
mov word [bx + 4], 10
mov bx, [1000]
mov cx, [1002]
mov dx, [1004]
mov bp, [1006]
; REG_A     : 0x0000 (0)
; REG_C     : 0x0000 (0)
; REG_D     : 0x0000 (0)
; REG_B     : 0x0000 (0)
; REG_SP    : 0x0000 (0)
; REG_BP    : 0x0000 (0)
; REG_SI    : 0x0000 (0)
; REG_DI    : 0x0000 (0)
; REG_IP    : 0x0030 (48)
; REG_FLAGS : 0x0000 (0)
; 0x0000: C706 E803 0100 C706 EA03 0200 C706 EC03 0300 C706 EE03 0400 BBE8 03C7 4704 0A00 
