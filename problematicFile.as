sub:            add      r3
LOOP:  prn #48 , LIST
mcro  a_mc  
cmp     K, #-6 
  bne  &END 
mcroend
  lea &STR, r6   
                 inc       r6 
                        mov     r3, K 
  sub r1, r4
  bne END
a_mc 
  dec K 
  #400
.entry MAIN  
           jmp  &LOOP      
END:  stop 
STR:            .string "abcd
LIST:            .data 6, -9 
                       .data    -100 
K:            .data 31