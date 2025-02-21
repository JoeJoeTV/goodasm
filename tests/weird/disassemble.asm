        .lang 6805

;;; Assemble these.
        neg a
        neg x
        neg @x
;;; Disassemble these.
        .ib 0x40 0x50 0x70 0x9d ; comment
        .ib 0x20 0x00 0x9d 0x20 0x00 ; Right relative offsets?
        .ib 39                       ; Should match data, not instruction.
self:    bra self
        
        
        
