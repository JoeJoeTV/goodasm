;;; I don't have any code for the AMI S2000 or the Iskra EMZ1001, so
;;; I'm roughly guessing from the instructions that I know.  Hopefully
;;; these will be close enough to function.

        db 0x00                 ;NOP
        db 0x01                 ;BRK
        db 0x02                 ;RT
        db 0x03                 ;RTS

        db 0x6F                 ;Page Prepare
        db 0xcF                 ;Jump!
        
