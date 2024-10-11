%ifndef DEVICE_REGS_INC
%define DEVICE_REGS_INC

%include "regs_186.inc"

;--------------------------------------
; I/O port numbers
; and constants related to I/O devices
;--------------------------------------
; Multi I/O device base addresses
;--------------------------------------
MULTI_IO_BASE_REG       EQU     (IO_PCS0_BASE + 0x00)
MULTI_IO_REGS           EQU     (MULTI_IO_BASE_REG + 0x00)
DISK_CTRL_REGS          EQU     (MULTI_IO_BASE_REG + 0x10)
KEYB_CTRL_REGS          EQU     (MULTI_IO_BASE_REG + 0x20)
UART_CTRL_REGS          EQU     (MULTI_IO_BASE_REG + 0x30)

;--------------------------------------
; Multi I/O device
;--------------------------------------
MULTI_IO_TODO           EQU     (MULTI_IO_REGS + 0x00)
MULTI_IO_READY_CHECK    EQU     (MULTI_IO_REGS + 0x0f)

;--------------------------------------
; Multi I/O device
;--------------------------------------
DISK_CTRL_STATUS        EQU     (DISK_CTRL_REGS + 0x00)     ; [RO] 8-bit
DISK_CTRL_COMMAND       EQU     (DISK_CTRL_REGS + 0x00)     ; [WO] 8-bit
DISK_CTRL_DATA          EQU     (DISK_CTRL_REGS + 0x01)     ; [RW] 8-bit
DISK_CTRL_DATALEN       EQU     (DISK_CTRL_REGS + 0x02)     ; [RW] 16-bit
DISK_CTRL_LBA           EQU     (DISK_CTRL_REGS + 0x04)     ; [RW] 32-bit

;--------------------------------------
; GPIO ports
;--------------------------------------
GPIO_PORTA_REG          EQU     (IO_PCS1_BASE + 0x00)
GPIO_PORTB_REG          EQU     (IO_PCS1_BASE + 0x01)
GPIO_PORTC_REG          EQU     (IO_PCS1_BASE + 0x02)
GPIO_CTRL_REG           EQU     (IO_PCS1_BASE + 0x03)

;--------------------------------------
; Text LCD
;--------------------------------------
TEXT_LCD_CMD            EQU     (IO_PCS2_BASE + 0x00)
TEXT_LCD_DATA           EQU     (IO_PCS2_BASE + 0x01)

;--------------------------------------
; Push buttons
;--------------------------------------
BUTTONS_STATE_REG       EQU     (IO_PCS3_BASE + 0x00)

;--------------------------------------
; External device
;--------------------------------------
EXTERNAL_BASE_REG       EQU     (IO_PCS6_BASE + 0x00)



;--------------------------------------
; Push button definitions
;--------------------------------------
BTN_DEBUG_STEP          EQU     0b00000001
BTN_DEBUG_RUN           EQU     0b00000010
BTN_2_UNUSED            EQU     0b00000100
BTN_3_UNUSED            EQU     0b00001000
BTN_UP                  EQU     0b00010000
BTN_RIGHT               EQU     0b00100000
BTN_DOWN                EQU     0b01000000
BTN_LEFT                EQU     0b10000000



;--------------------------------------
; Disk controller status bits definitions
;--------------------------------------
DISK_CTRL_STATUS_RW_ERR         EQU     0b00000001
DISK_CTRL_STATUS_CRC_ERR        EQU     0b00000010
DISK_CTRL_STATUS_NOT_FOUND      EQU     0b00000100
DISK_CTRL_STATUS_DEV_MISSING    EQU     0b00001000
DISK_CTRL_STATUS_INVALID_ARG    EQU     0b00010000
DISK_CTRL_STATUS_UNKNOWN_CMD    EQU     0b00100000
DISK_CTRL_STATUS_OTHER_ERR      EQU     0b01000000
DISK_CTRL_STATUS_BUSY           EQU     0b10000000

;--------------------------------------
; Disk controller commands
;--------------------------------------
DISK_CTRL_CMD_READ_SECTOR       EQU 'R'
DISK_CTRL_CMD_WRITE_SECTOR      EQU 'W'

%endif
