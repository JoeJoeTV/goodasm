.lang tlcs47
          ld a, @hl                     ; A=mem[HL]
          ld a, @0x35                   ; A=mem4[abs8]
          ld hl, @0x35                  ; HL=mem8[abs8]
          ld a, #0xf                    ; A=imm4
          ld h, #0xf                    ; H=imm4
          ld l, #0xf                    ; L=imm4
          ldl a, @dc                    ; Loads lower nybble of read from data table at DC into the accumulator.
          ldh a, @dc+                   ; Loads higher nybble of read from data table at DC into the accumulator.
          st a, @hl                     ; mem[HL]=A
          st a, @hl+                    ; mem[HL]=A, L++
          st a, @hl-                    ; mem[HL]=A, L--
          st a, @0x35                   ; mem4[imm8]=A
          st #0xf, @hl+                 ; mem4[HL]=imm4, L++
          st #0xf, @0xf                 ; mem[abs4]=imm4.  Zero page.
          mov h, a                      ; H=A
          mov l, a                      ; L=A
          xch a, h                      ; Exchange A and H.
          xch a, l                      ; Exchange A and L.
          xch a, eir                    ; Exchange A for EIR (interrupt enable register).
          xch a, @hl                    ; Exchange A with mem[HL]
          xch a, @0xff                  ; Exchanges A for mem[abs8].
          xch hl, @0x35                 ; Exchanges HL for mem[abs8].
          cmpr a, @hl                   ; Compare A to mem[HL].
          cmpr a, @0xff                 ; Compare A to mem[abs8].
          cmpr a, #0xf                  ; Compare A to imm4.
          cmpr h, #0xf                  ; Compare H to imm4.
          cmpr l, #0xf                  ; Compare L to imm4.
          cmpr @0xF, #0xF               ; Compare mem[abs4] to imm4.  Zero page.
          inc a                         ; A++
          inc l                         ; L++
          inc @hl                       ; mem[HL]++
          dec a                         ; A--
          dec l                         ; L--
          dec @hl                       ; mem[HL]--
          addc a, @hl                   ; Add w/ Carry mem[HL] into A.
          add a, @hl                    ; A=A+mem[HL].
          add a, #0xf                   ; A=A+imm4
          add h, #0xf                   ; Adds imm4 into H.  Use 1 or F for inc/dec.
          add l, #0x7                   ; Adds imm4 into L.  Use 1 or F for inc/dec.
          add @hl, #0xf                 ; mem[HL]+=imm4.
          add @0xf, #0xf                ; mem[abs4]+=imm4.  Zero page.
          subrc a, @hl                  ; A=mem[HL]-A w/ carry.
          subr a, #0xf                  ; A=mem[HL]-A.
          subr @hl, #0xf                ; mem[HL]-=imm4.
          rolc a                        ; Rotates A and Carry flag left .
          rorc a                        ; Rotate A and Carry flag right.
          and a, @hl                    ; A = A & mem[HL].
          and a, #0xf                   ; A = A & imm4.
          and @hl, #0xf                 ; mem[HL] = mem[HL] & imm4.
          or a, @hl                     ; A = A | mem[HL].
          or a, #0xf                    ; A = A | imm4.
          or @hl, #0xf                  ; mem[HL] = mem[HL] | imm4.
          xor a, @hl                    ; A = A ^ mem[HL]
          test cf                       ; Tests the carry flag.
          test a, bit4                  ; Tests a bit of the accuulator.
          test @hl, bit4                ; Tests a bit of mem[HL].
          test @0xf, bit4               ; Tests a bit of mem[abs4].
          test %0xf, bit4               ; Tests bits on an input port.
          test @l                       ; Tests I/O pin from low bits of L.
          testp cf                      ; Tests Carry flag and sets flag.
          testp zf                      ; Test the zero flag.
          testp gf                      ; Test the general flag.
          testp @0xf, bit4              ; Tests a bit of mem[abs4].  Zero page.
          testp %0xf, bit4              ; Test a bit in a port.
          set gf                        ; Set the general flag.
          set @hl, bit4                 ; Set a bit in HL.
          set @0xf, bit4                ; Sets a bit of mem[abs4].Zero page.
          set %0xf, bit4                ; Sets a bit of an IO port.
          set @l                        ; Sets a bit in I/O port from low bits of L.
          clr gf                        ; Clears the general flag.
          clr @hl, bit4                 ; Clears a bit in mem[HL].
          clr @0xf, bit4                ; Clears bit of mem[abs4].  Zero page.
          clr %0xf, bit4                ; Clears bit of I/O port.
          clr @l                        ; Clears I/O from low bits of L.
          clr il, #0x3f                 ; Clears interrupt latches.
          eiclr il, #0x3f               ; Sets interrupt latches.
          diclr il, #0x3f               ; Resets interrupt latches.
          in %0xf, a                    ; A=port4.
          in %0xf, @hl                  ; mem[HL]=port4.
          out a, %0x1f                  ; Output accumulator to a port.
          out @hl, %0x1f                ; Output mem[HL] to a port.
          out #0xf, %0xf                ; Output imm4 to a port.
          outb @hl                      ; Outputs memory at (FE0+HL) to P2,P1 ports.
          bs 0xfff                      ; Branches if SF is set, otherwise sets SF.
loop:	  bss loop                      ; Branches within page is SF is set, otherwise sets SF.
          call 0x7ff                    ; Calls a subroutine, pushing return pointer to stack.
          calls 0xF                     ; Short form CALL.
          ret                           ; Return
          reti                          ; Return from interrrupt.
          nop                           ; No Operation.
