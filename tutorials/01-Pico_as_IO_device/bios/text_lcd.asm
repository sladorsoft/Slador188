        CPU 186
        BITS 16

%include "system_def.inc"

        SECTION .text

        GLOBAL text_lcd_init
        GLOBAL text_lcd_clear_display
        GLOBAL text_lcd_return_home
        GLOBAL text_lcd_entry_mode_set
        GLOBAL text_lcd_display_ctrl
        GLOBAL text_lcd_function_set
        GLOBAL text_lcd_set_char_gen_addr
        GLOBAL text_lcd_set_xy
        GLOBAL text_lcd_send_char
        GLOBAL text_lcd_send_char_at
        GLOBAL text_lcd_send_text
        GLOBAL text_lcd_send_text_at

; =====================================
; "0" for 4-bit data, "1" for 8-bit data
%define TEXT_LCD_USE_8_BIT 0

; use "0" for bits 3-0 and "1" for bits 7-4
%define TEXT_LCD_DATA_PINS 1

TEXT_LCD_CHARS_PER_LINE EQU     20
TEXT_LCD_LINES          EQU     4

TEXT_LCD_DATA_PORT      EQU     GPIO_PORTC_REG
TEXT_LCD_CTRL_PORT      EQU     GPIO_PORTA_REG
TEXT_LCD_RS_PIN         EQU     0
TEXT_LCD_RW_PIN         EQU     1
TEXT_LCD_E_PIN          EQU     2
; =====================================


TEXT_LCD_LINE_1_ADDR    EQU     0x40


%if (TEXT_LCD_DATA_PORT == GPIO_PORTA_REG)
  TEXT_LCD_DATA_DIR_BITS  EQU     0b00010000
%elif (TEXT_LCD_DATA_PORT == GPIO_PORTB_REG)
  TEXT_LCD_DATA_DIR_BITS  EQU     0b00000010
%else
  %if (TEXT_LCD_USE_8_BIT != 0)
    TEXT_LCD_DATA_DIR_BITS  EQU     0b00001001
  %else
    %if (TEXT_LCD_DATA_PINS == 0)
      TEXT_LCD_DATA_DIR_BITS  EQU     0b00000001
    %else
      TEXT_LCD_DATA_DIR_BITS  EQU     0b00001000
    %endif
  %endif
%endif

; usage: CTRL_PINS_TO_AL [RS, RW, E], for example: CTRL_PINS_TO_AL 0, 1, 0
%macro CTRL_PINS_TO_AL 3
        and al, ~((1 << TEXT_LCD_RS_PIN) | (1 << TEXT_LCD_RW_PIN) | (1 << TEXT_LCD_E_PIN))
%if ((%1 != 0) || (%2 != 0) || (%3 != 0))
        or al, ((%1 << TEXT_LCD_RS_PIN) | (%2 << TEXT_LCD_RW_PIN) | (%3 << TEXT_LCD_E_PIN))
%endif
%endmacro


%macro TEXT_LCD_DELAY 1
        mov cx, ((SYSTEM_CPU_CLK / 16) * %1) / 1000000 + 1  ; in us
%%1:    dec cx              ; clocks: 3
        jnz %%1             ; clocks: 13 if taken, or 4

%endmacro


;--------------------------------------
; void text_lcd_init()
;--------------------------------------
text_lcd_init:
        in al, TEXT_LCD_CTRL_PORT
        CTRL_PINS_TO_AL 0, 0, 0
        out TEXT_LCD_CTRL_PORT, al

        TEXT_LCD_DELAY 15000

        mov ah, 0b00000011
        call _write_ah_to_nibble

        TEXT_LCD_DELAY 4100

        mov ah, 0b00000011
        call _write_ah_to_nibble

        TEXT_LCD_DELAY 100

        mov ah, 0b00000011
        call _write_ah_to_nibble

        TEXT_LCD_DELAY 40

%if (TEXT_LCD_USE_8_BIT == 0)
        mov ah, 0b00000010
%else
        mov ah, 0b00000011
%endif
        call _write_ah_to_nibble
        TEXT_LCD_DELAY 40
        ret


;--------------------------------------
; void text_lcd_clear_display()
;--------------------------------------
text_lcd_clear_display:
        mov ah, 0b00000001
        jmp _write_cmd

;--------------------------------------
; void text_lcd_return_home()
;--------------------------------------
text_lcd_return_home:
        mov ah, 0b00000010
        jmp _write_cmd

;--------------------------------------
; void text_lcd_entry_mode_set(bool increment, bool disp_shift)
;--------------------------------------
text_lcd_entry_mode_set:
        mov bx, sp
        mov ah, 0b00000100
        mov al, [bx + 2]
        test al, al
        jz .1
        or ah, 0b00000010
.1:
        mov al, [bx + 4]
        test al, al
        jz .2
        or ah, 0b00000001
.2:
        jmp _write_cmd

;--------------------------------------
; void text_lcd_display_ctrl(bool is_on, bool cursor_visible, bool cursor_blink)
;--------------------------------------
text_lcd_display_ctrl:
        mov bx, sp
        mov ah, 0b00001000
        mov al, [bx + 2]
        test al, al
        jz .1
        or ah, 0b00000100
.1:
        mov al, [bx + 4]
        test al, al
        jz .2
        or ah, 0b00000010
.2:
        mov al, [bx + 6]
        test al, al
        jz .3
        or ah, 0b00000001
.3:
        jmp _write_cmd

;--------------------------------------
; void text_lcd_function_set()
;--------------------------------------
text_lcd_function_set:
%if (TEXT_LCD_USE_8_BIT == 0)
        mov ah, 0b00101000
%else
        mov ah, 0b00111000
%endif
        jmp _write_cmd

;--------------------------------------
; void text_lcd_set_char_gen_addr(uint8_t addr)
;--------------------------------------
text_lcd_set_char_gen_addr:
        mov bx, sp
        mov ah, [bx + 2]
        and ah, 0b00111111
        or ah, 0b01000000
        jmp _write_cmd

;--------------------------------------
; void text_lcd_set_disp_addr(uint8_t addr)
;--------------------------------------
text_lcd_set_disp_addr:
        mov bx, sp
        mov ah, [bx + 2]
        and ah, 0b01111111
        or ah, 0b10000000
        jmp _write_cmd

;--------------------------------------
; void text_lcd_set_xy(uint8_t x, uint8_t y)
;--------------------------------------
text_lcd_set_xy:
        push bp
        mov bp, sp

        xor dx, dx
        mov al, [bp + 6]
        test al, 0b00000001
        jz .1
        mov dl, TEXT_LCD_LINE_1_ADDR
.1:
        test al, 0b00000010
        jz .2
        add dl, TEXT_LCD_CHARS_PER_LINE
.2:
        mov al, [bp + 4]
        add dl, al
        push dx
        call text_lcd_set_disp_addr
        add sp, 2

        mov sp, bp
        pop bp
        ret

;--------------------------------------
; void text_lcd_send_char(char c)
;--------------------------------------
text_lcd_send_char:
        mov bx, sp
        mov ah, [bx + 2]
        jmp _write_data

;--------------------------------------
; void text_lcd_send_char_at(uint8_t x, uint8_t y, char c)
;--------------------------------------
text_lcd_send_char_at:
        push bp
        mov bp, sp

        mov al, [bp + 6]
        push ax
        mov al, [bp + 4]
        push ax
        call text_lcd_set_xy
        add sp, 4

        mov ah, [bp + 8]
        call _write_data

        mov sp, bp
        pop bp
        ret

;--------------------------------------
; void text_lcd_send_text(const char* str)
;--------------------------------------
text_lcd_send_text:
        push bp
        mov bp, sp

        push si
        mov si, [bp + 4]
        test si, si
        jz .2
.1:
        lodsb
        test al, al
        jz .2
        mov ah, al
        call _write_data
        jmp .1
.2:
        pop si

        mov sp, bp
        pop bp
        ret

;--------------------------------------
; void text_lcd_send_text_at(uint8_t x, uint8_t y, const char* str)
;--------------------------------------
text_lcd_send_text_at:
        push bp
        mov bp, sp

        mov al, [bp + 6]
        push ax
        mov al, [bp + 4]
        push ax
        call text_lcd_set_xy
        add sp, 4

        mov ax, [bp + 8]
        push ax
        call text_lcd_send_text

        mov sp, bp
        pop bp
        ret

; =====================================
; local helper functions
; -------------------------------------

; writes bits 3-0 of AH to the DATA port
; assumes AL contains current CTRL PORT value
_write_ah_to_nibble:
        or al, (1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al

        in al, TEXT_LCD_DATA_PORT
%if (TEXT_LCD_DATA_PINS == 0)
        and ah, 0b00001111
        and al, 0b11110000
%else
        shl ah, 4
        and al, 0b00001111
%endif
        or al, ah
        out TEXT_LCD_DATA_PORT, al

        in al, TEXT_LCD_CTRL_PORT
        and al, ~(1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al
        ret


; expects the command in AH
_write_cmd:
        in al, TEXT_LCD_CTRL_PORT
        CTRL_PINS_TO_AL 0, 0, 0
        out TEXT_LCD_CTRL_PORT, al

        jmp _write_ah_to_data

_write_data:
        in al, TEXT_LCD_CTRL_PORT
        CTRL_PINS_TO_AL 1, 0, 0
        out TEXT_LCD_CTRL_PORT, al

        call _write_ah_to_data
        TEXT_LCD_DELAY 4
        ret

; writes DATA from AH
; assumes AL contains current CTRL PORT value
_write_ah_to_data:
%if (TEXT_LCD_USE_8_BIT == 0)
        mov dl, ah
        shr ah, 4
        call _write_ah_to_nibble
        mov ah, dl
        call _write_ah_to_nibble
%else
        or al, (1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al

        xchg al, ah
        out TEXT_LCD_DATA_PORT, al

        xchg al, ah
        and al, ~(1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al
%endif
_check_busy_flag:
        in al, GPIO_CTRL_REG
        or al, TEXT_LCD_DATA_DIR_BITS
        out GPIO_CTRL_REG, al

        in al, TEXT_LCD_CTRL_PORT
        CTRL_PINS_TO_AL 0, 1, 0
        out TEXT_LCD_CTRL_PORT, al
.1:
        or al, (1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al

        in al, TEXT_LCD_DATA_PORT
        mov ah, al

        in al, TEXT_LCD_CTRL_PORT
        and al, ~(1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al

%if (TEXT_LCD_USE_8_BIT == 0)
        or al, (1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al
        and al, ~(1 << TEXT_LCD_E_PIN)
        out TEXT_LCD_CTRL_PORT, al
%endif

%if ((TEXT_LCD_USE_8_BIT == 0) && (TEXT_LCD_DATA_PINS == 0))
        test ah, (1 << 3)
%else
        test ah, (1 << 7)
%endif
        jnz .1

        CTRL_PINS_TO_AL 0, 0, 0
        out TEXT_LCD_CTRL_PORT, al

        in al, GPIO_CTRL_REG
        and al, ~TEXT_LCD_DATA_DIR_BITS
        out GPIO_CTRL_REG, al

        ret
