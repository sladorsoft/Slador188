        CPU 186
        BITS 16

%include "system_def.inc"

        SECTION .text

        GLOBAL init

        EXTERN __dataoffset
        EXTERN __ldata
        EXTERN __sbss
        EXTERN __lbss

        EXTERN init_int_vectors
        EXTERN set_int_vector
        EXTERN lcd_init
        EXTERN int_ctrl_init
        EXTERN timers_init
        EXTERN main

init:
;        mov ax, BITSET(RELREG_ADDR_BITS, PCB_BASE >> 8, RELREG_ADDR_SIZE)
;        mov dx, RELREG_RESET
;        out dx, al

        mov ax, LMCS_UNUSED_BITS | BITSET(LMCS_MEM_SIZE_BITS, 0xff, LMCS_MEM_SIZE_SIZE) | BITSET(CS_RDY_IGNRDY_BIT) | BITSET(CS_RDY_WAIT_BITS, 0, CS_RDY_WAIT_SIZE) ; 256kB RAM (max) + 0 wait states (not used)
        mov dx, LMCS
        out dx, al

        mov ax, MMCS_UNUSED_BITS | BITSET(MMCS_MEM_SIZE_BITS, 0x40, MMCS_MEM_SIZE_SIZE) | BITSET(CS_RDY_WAIT_BITS, 0, CS_RDY_WAIT_SIZE) ; MCS starts at 0x80000 + READY active
        mov dx, MMCS
        out dx, al

        mov ax, MPCS_UNUSED_BITS | BITSET(MPCS_MEM_SIZE_BITS, (MCS_BLOCK_SIZE >> 13), MPCS_MEM_SIZE_SIZE) | BITSET(MPCS_EX_BIT) | BITSET(CS_RDY_WAIT_BITS, 0, CS_RDY_WAIT_SIZE) ; MCS block size, activate PCS5-6 pins + READY active for PCS4-6
        mov dx, MPCS
        out dx, al

        mov ax, PACS_UNUSED_BITS | BITSET(PACS_MEM_SIZE_BITS, (SYSTEM_IO_BASE >> 10), PACS_MEM_SIZE_SIZE) | BITSET(CS_RDY_WAIT_BITS, 0, CS_RDY_WAIT_SIZE) ; I/O base address for PCS = 0x0000 + READY active for PCS0-3
        mov dx, PACS
        out dx, al

        mov ax, SYSTEM_STACK_SEG
        mov ss, ax
        xor sp, sp
        mov es, ax

        mov ax, cs
        mov ds, ax

        mov si, __dataoffset
        xor di, di
        mov cx, __ldata
        rep movsw

        xor ax, ax
        mov di, __sbss
        mov cx, __lbss
        rep stosw

        call init_int_vectors

        mov ax, ss
        mov ds, ax
        mov es, ax

; configure the 8255:
; - Port A: OUT
; - Port B: IN
; - Port C lower half: IN
; - Port C upper half: OUT
        mov al, 0b10000011
        out GPIO_CTRL_REG, al
        mov al, 0b00001111
        out GPIO_PORTC_REG, al
        mov al, 0b11111111
        out GPIO_PORTB_REG, al

        call lcd_init
        call timers_init

        xor ah, ah
; check initial state of the "step" button
        mov dx, BUTTONS_STATE_REG
        in al, dx
        and al, BTN_DEBUG_STEP
        jnz .1

; set the Trap Flag via the stack if the "step" button is pressed
        pushf
        mov bp, sp
        or word [bp], (1 << 8)  ; TF is the 8th bit in FLAGS
        popf
.1:

        int3

        xor ax, ax
        in al, MULTI_IO_BASE_REG + 0x04
        in al, MULTI_IO_BASE_REG + 0x05
        xor ax, ax
        in ax, MULTI_IO_BASE_REG + 0x04

        mov cx, 20
.2:
        in ax, MULTI_IO_BASE_REG + 0x00
        dec cx
        jnz .2

        xor ax, ax
        out MULTI_IO_BASE_REG + 0x00, al

        mov cx, 10
.3:
        in ax, MULTI_IO_BASE_REG + 0x00
        dec cx
        jnz .3

        int3

        sti
        call main
        jmp $
