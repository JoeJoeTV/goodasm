;;; This is an example Hello World app for ZSHell on the TI-85, forked
;;; from one of the many examples.  Typical for ZSH, our assembler
;;; does not know the running address of the code, so a lot of fix ups
;;; are needed.

;;; https://www.ticalc.org/programming/columns/85-asm/bergou/lesson1.html

;;; Definitions from TI-85.H
        ;; Relocated functions, unique to each ROM version.
        .def ROM_CALL 0x8c09    ; Calls frequently relocated function by 1-byte index.
        .def CLEARLCD 0x08      ; Clear LCD, but not text or graphics memory.
        .def D_ZT_STR 0x0b      ; Display zero-terminated text string.

        ;; Consistent addresses.  Call normally.
        .def GET_KEY 0x01be     ; Get immediate keystrong into A.
        .def K_EXIT 0x37
        
        ;; Global variables in consistent addresses.
        .def CURSOR_ROW 0x800c   ; Text cursor row, followed by column.
        .def PROGRAM_ADDR 0x8c3c ; Program base address.

        .lang z80
        .org 0

        .db "Hello people.", 0  ; ZShell Menu Text

        call ROM_CALL
        .db CLEARLCD            ; Clear the screen.

        ld hl, #0x0303          ; Load value into HL. 4th row, 4th col.
        ld @CURSOR_ROW, hl      ; Load HL into memory where curso position is stored.

        ld hl, @PROGRAM_ADDR    ; Get program's address into HL register.
        ld de, #text            ; Get string's address into DE register.
        add hl, de              ; Get absolute location of string.
        call ROM_CALL           ; Do ROM call.
        .db D_ZT_STR            ; Display the string whose address is in HL.

keyloop:
        call GET_KEY            ; Load value of last keypress into A.
        cp a, #K_EXIT           ; Compare it to exit key.
        ret z                   ; If result is zero, return to ZSH.
        jr keyloop              ; Return to start of loop.

text:
        .db "Hello, world. :)", 0 ; String to be displayed.


        
        
